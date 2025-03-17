#include "sl_bluetooth.h"
#include "sl_bt_api.h"
#include "gatt_db.h"
#include "ble_device_type.h"

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#include "app.h"
#include "ble.h"
#include "gpio.h"
#include "i2c.h"
#include "lcd.h"
#include <math.h>

#define INTERVAL_MIN  400u  // 250/0.625
#define INTERVAL_MAX  400u  // 250/0.625
#define DEFAULT_DURATION  0u  // Value 0 indicates no advertising duration limit and advertising continues until it is disabled.
#define DEFAULT_MAX_EVENTS  0u  // Value 0 indicates no maximum number limit.
#define CONN_INTERVAL_MIN  60u  // 75/1.25
#define CONN_INTERVAL_MAX  60u  // 75/1.25
#define PERIPHERAL_LATENCY  4u  // 300/75
#define SUPERVISION_TIMEOUT_S  80u            // > ((1 + latency) * max_interval * 2)/10 = 75
#define SUPERVISION_TIMEOUT_C  83u            // = ((1 + slave_latency) * (max_interval * 2) + max_interval)/10 = 83
#define DEFAULT_MIN_CE_LENGTH  0x0000
#define DEFAULT_MAX_CE_LENGTH_S  0xffff
#define DEFAULT_MAX_CE_LENGTH_C 0x4

#define SCAN_INTERVAL 80u // 50/0.625
#define SCAN_WINDOW 40u // 25/0.625

#define FLAGS 0b1111

const uint8_t server_bt_address[6] = SERVER_BT_ADDRESS;

// BLE private data
ble_data_struct_t ble_data;

/****************************************************************************
 * @brief   Retrieves a pointer to the BLE data structure.
 * @param   None
 * @return  Pointer to the ble_data_struct_t structure.
 ****************************************************************************/
ble_data_struct_t* get_ble_data_ptr(void) {
    return &ble_data;
}

uint32_t nextPtr(uint32_t ptr) {
  return (ptr + 1) % QUEUE_DEPTH; // pointer advancement with wrap around
}

int write_queue (indication_para_struct_t data) { //uint16_t charHandle, uint32_t bufLength, uint8_t *buffer) {

  if ((!data.buffer) || (data.bufferLength > MAX_BUFFER_LENGTH) || (data.bufferLength < MIN_BUFFER_LENGTH)) {
    return -1; // function parameters validation
  }

  ble_data_struct_t* ble_data = get_ble_data_ptr();
  if (ble_data->buffer_full) {
    return -1; // Queue is full
  }
  ble_data->indication_buffer[ble_data->wptr] = data;
  ble_data->wptr = nextPtr(ble_data->wptr); // write pointer update after writing
  ble_data->buffer_full = (ble_data->wptr == ble_data->rptr); // full_flag update
  return 0;

} // write_queue()

indication_para_struct_t* read_queue() {
  ble_data_struct_t* ble_data = get_ble_data_ptr();
    if ((ble_data->wptr == ble_data->rptr) && !ble_data->buffer_full) {
        return NULL; // Queue is empty
    }

    indication_para_struct_t* read_data = &ble_data->indication_buffer[ble_data->rptr];
    ble_data->rptr = nextPtr(ble_data->rptr); // Update read pointer
    ble_data->buffer_full = false; // Clear full flag

    return read_data;
} // read_queue

#if DEVICE_IS_BLE_SERVER
/****************************************************************************
 * @brief   Sends the temperature value over bluetooth to the client.
 * @param   None
 * @return  None
 ****************************************************************************/
void ble_send_temp_value(void) {
  uint8_t htm_temperature_buffer[5];
  uint8_t *p = &htm_temperature_buffer[0];
  uint32_t htm_temperature_flt;
  uint8_t flags = 0x00;
  sl_status_t sc;
  ble_data_struct_t* ble_data = get_ble_data_ptr();
  indication_para_struct_t indication_data;

  uint32_t temperature_in_c = get_temp_value();
  UINT8_TO_BITSTREAM(p, flags); // insert the flags byte
  htm_temperature_flt = INT32_TO_FLOAT(temperature_in_c*1000, -3);
  UINT32_TO_BITSTREAM(p, htm_temperature_flt);  // insert the temperature measurement

  sc = sl_bt_gatt_server_write_attribute_value(gattdb_temperature_measurement, 0, 5, &htm_temperature_buffer[0]);
  if (sc != SL_STATUS_OK) {
      LOG_ERROR("sl_bt_gatt_server_write_attribute_value failed with error code: 0x%x", (unsigned int)sc);
  }

  if (ble_data->ok_to_send_htm_indications) {
    if (ble_data->indication_in_flight) {
        indication_data.charHandle = gattdb_temperature_measurement;
        indication_data.bufferLength = 5;
        memcpy(indication_data.buffer, htm_temperature_buffer, 5);
        int ret = write_queue(indication_data);
        if(ret == -1) {
            LOG_ERROR("write_queue failed with error: -1");
        }
    } else {
        sc = sl_bt_gatt_server_send_indication(ble_data->connection_handle, gattdb_temperature_measurement, 5, &htm_temperature_buffer[0]);
        if (sc != SL_STATUS_OK) {
            LOG_ERROR("sl_bt_gatt_server_send_indication failed with error code: 0x%x", (unsigned int)sc);
        } else {
            ble_data->indication_in_flight = true;
        }
        displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp=%d", temperature_in_c);
    }

  }
}

void ble_send_button_state(uint8_t state) {
  uint8_t button_state[2];
  sl_status_t sc;
  ble_data_struct_t* ble_data = get_ble_data_ptr();
  indication_para_struct_t indication_data;

  // Send 0x00 or 0x01
  button_state[0] = 0;
  button_state[1] = state;
  if(ble_data->connection_open) {
      sc = sl_bt_gatt_server_write_attribute_value(gattdb_button_state, 0, 1, &button_state[0]);
      if (sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_gatt_server_write_attribute_value failed with error code: 0x%x", (unsigned int)sc);
      }
      if ((ble_data->ok_to_send_button_indications == true) && (ble_data->bonded == true)) {
          if (ble_data->indication_in_flight) {
              indication_data.charHandle = gattdb_button_state;
              indication_data.bufferLength = 2;
              memcpy(indication_data.buffer, button_state, 2);
              int ret = write_queue(indication_data);
              if(ret == -1) {
                  LOG_ERROR("write_queue failed with error: -1");
              }
          } else {
            sc = sl_bt_gatt_server_send_indication(ble_data->connection_handle, gattdb_button_state, 2, &button_state[0]);
            if (sc != SL_STATUS_OK) {
                LOG_ERROR("sl_bt_gatt_server_send_indication failed with error code: 0x%x", (unsigned int)sc);
            } else {
                ble_data->indication_in_flight = true;
            }
          }
      }
  }
}

#else
static int32_t FLOAT_TO_INT32(const uint8_t *buffer_ptr)
{
  uint8_t signByte = 0;
  int32_t mantissa;
  // input data format is:
  // [0] = flags byte, bit[0] = 0 -> Celsius; =1 -> Fahrenheit
  // [3][2][1] = mantissa (2's complement)
  // [4] = exponent (2's complement)
  // BT buffer_ptr[0] has the flags byte
  int8_t exponent = (int8_t)buffer_ptr[4];
  // sign extend the mantissa value if the mantissa is negative
  if (buffer_ptr[3] & 0x80) { // msb of [3] is the sign of the mantissa
      signByte = 0xFF;
  }
  mantissa = (int32_t) (buffer_ptr[1] << 0) |
  (buffer_ptr[2] << 8) |
  (buffer_ptr[3] << 16) |
  (signByte << 24) ;
  // value = 10^exponent * mantissa, pow() returns a double type
  return (int32_t) (pow(10, exponent) * mantissa);
} // FLOAT_TO_INT32
#endif

/****************************************************************************
 * @brief   Handles Bluetooth events and processes them accordingly.
 * @param   evt  Pointer to the sl_bt_msg_t structure containing the event data.
 * @return  None
 ****************************************************************************/
void handle_ble_event(sl_bt_msg_t *evt) {
  sl_status_t sc;
  ble_data_struct_t* ble_data = get_ble_data_ptr();
  // Handle stack events
  switch (SL_BT_MSG_ID(evt->header)) {
    case sl_bt_evt_system_boot_id:
      displayInit();
      // Read the Bluetooth identity address used by the device
      sc = sl_bt_system_get_identity_address(&ble_data->myAddress, &ble_data->myAddress_type);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_system_get_identity_address failed with error code: 0x%x", (unsigned int)sc);
      }
      displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x", ble_data->myAddress.addr[0], ble_data->myAddress.addr[1], ble_data->myAddress.addr[2], ble_data->myAddress.addr[3], ble_data->myAddress.addr[4], ble_data->myAddress.addr[5]);
      displayPrintf(DISPLAY_ROW_ASSIGNMENT, "A8");
#if DEVICE_IS_BLE_SERVER
      displayPrintf(DISPLAY_ROW_NAME, "Server");
      sc = sl_bt_advertiser_create_set(&ble_data->advertisingSetHandle);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_create_set failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_advertiser_set_timing(ble_data->advertisingSetHandle, INTERVAL_MIN, INTERVAL_MAX, DEFAULT_DURATION, DEFAULT_MAX_EVENTS);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_set_timing failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_sm_delete_bondings();
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_sm_delete_bondings failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_sm_configure(FLAGS, sl_bt_sm_io_capability_displayyesno);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_sm_configure failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_legacy_advertiser_generate_data(ble_data->advertisingSetHandle, sl_bt_advertiser_general_discoverable);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_legacy_advertiser_generate_data failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_legacy_advertiser_start(ble_data->advertisingSetHandle, sl_bt_legacy_advertiser_connectable);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_legacy_advertiser_start failed with error code: 0x%x", (unsigned int)sc);
      }
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
#else
      displayPrintf(DISPLAY_ROW_NAME, "Client");
      sc = sl_bt_scanner_set_parameters(sl_bt_scanner_scan_mode_passive, SCAN_INTERVAL, SCAN_WINDOW);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_scanner_set_parameters failed with error code: 0x%x", (unsigned int)sc);
      }
      sc = sl_bt_connection_set_default_parameters(CONN_INTERVAL_MIN, CONN_INTERVAL_MAX, PERIPHERAL_LATENCY, SUPERVISION_TIMEOUT_C, DEFAULT_MIN_CE_LENGTH, DEFAULT_MAX_CE_LENGTH_C);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_connection_set_default_parameters failed with error code: 0x%x", (unsigned int)sc);
      }
      sc = sl_bt_scanner_start(sl_bt_scanner_scan_phy_1m, sl_bt_scanner_discover_generic);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_scanner_start failed with error code: 0x%x", (unsigned int)sc);
      }
      displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
#endif
      ble_data->connection_open = false;
      ble_data->indication_in_flight = false;
      ble_data->ok_to_send_htm_indications = false;
      ble_data->gatt_procedure_inprogress = false;
      ble_data->bonded = false;
      ble_data->PB0_pressed = false;
      ble_data->ok_to_send_button_indications = false;
      ble_data->wptr = 0;
      ble_data->rptr = 0;
      break;
    case sl_bt_evt_connection_opened_id:
      ble_data->connection_open = true;
      ble_data->ok_to_send_htm_indications = false;
      ble_data->ok_to_send_button_indications = false;
      ble_data->connection_handle = evt->data.evt_connection_opened.connection;

#if DEVICE_IS_BLE_SERVER
      sc = sl_bt_advertiser_stop(ble_data->advertisingSetHandle);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_stop failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_connection_set_parameters(ble_data->connection_handle, CONN_INTERVAL_MIN, CONN_INTERVAL_MAX, PERIPHERAL_LATENCY, SUPERVISION_TIMEOUT_S, DEFAULT_MIN_CE_LENGTH, DEFAULT_MAX_CE_LENGTH_S);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_connection_set_parameters failed with error code: 0x%x", (unsigned int)sc);
      }
#else
      displayPrintf(DISPLAY_ROW_BTADDR2, "%x:%x:%x:%x:%x:%x", server_bt_address[0], server_bt_address[1], server_bt_address[2], server_bt_address[3], server_bt_address[4], server_bt_address[5]);
#endif
      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
      break;
    case sl_bt_evt_connection_closed_id:
      gpioLed0SetOff();
      gpioLed1SetOff();

      ble_data->connection_open = false;
      ble_data->indication_in_flight = false;
      ble_data->ok_to_send_htm_indications = false;
      ble_data->gatt_procedure_inprogress = false;
      ble_data->bonded = false;
      ble_data->PB0_pressed = false;
      ble_data->ok_to_send_button_indications = false;

      displayPrintf(DISPLAY_ROW_PASSKEY, "");
      displayPrintf(DISPLAY_ROW_ACTION, "");
      displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
      displayPrintf(DISPLAY_ROW_9, "");
#if DEVICE_IS_BLE_SERVER
      sc = sl_bt_legacy_advertiser_generate_data(ble_data->advertisingSetHandle, sl_bt_advertiser_general_discoverable);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_legacy_advertiser_generate_data failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_legacy_advertiser_start(ble_data->advertisingSetHandle, sl_bt_legacy_advertiser_connectable);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_legacy_advertiser_start failed with error code: 0x%x", (unsigned int)sc);
      }
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
#else
      // Start the GAP discovery procedure to scan for advertising devices.
      sc = sl_bt_scanner_start(sl_bt_scanner_scan_phy_1m, sl_bt_scanner_discover_generic);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_scanner_start failed with error code: 0x%x", (unsigned int)sc);
      }
      displayPrintf(DISPLAY_ROW_CONNECTION, "Discovering");
#endif
      displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
      displayPrintf(DISPLAY_ROW_BTADDR2, "");
      break;
    case sl_bt_evt_connection_parameters_id:
      // Commented below log out as asked after observing
      //LOG_INFO("Connection parameters: interval: %d, latency: %d, timeout: %d", (int)(evt->data.evt_connection_parameters.interval * 1.25), evt->data.evt_connection_parameters.latency, evt->data.evt_connection_parameters.timeout * 10 );
      break;
    case sl_bt_evt_system_soft_timer_id:
      // This event indicates that a soft timer has expired.
      displayUpdate();
      break;
#if DEVICE_IS_BLE_SERVER
    case sl_bt_evt_sm_confirm_bonding_id:
      sc = sl_bt_sm_bonding_confirm(ble_data->connection_handle, 1);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_sm_bonding_confirm failed with error code: 0x%x", (unsigned int)sc);
      }
      break;
    case sl_bt_evt_sm_confirm_passkey_id:
      ble_data->passkey = evt->data.evt_sm_confirm_passkey.passkey;
      displayPrintf(DISPLAY_ROW_PASSKEY, "%d", ble_data->passkey);
      displayPrintf(DISPLAY_ROW_ACTION, "Confirm with PB0");
      break;
    case sl_bt_evt_sm_bonding_failed_id:
      LOG_ERROR("Bonding failed");
      sc = sl_bt_connection_close(ble_data->connection_handle);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_connection_close failed with error code: 0x%x", (unsigned int)sc);
      }
      break;
    case sl_bt_evt_sm_bonded_id:
      ble_data->bonded = true;
      displayPrintf(DISPLAY_ROW_CONNECTION, "Bonded");
      displayPrintf(DISPLAY_ROW_PASSKEY, "");
      displayPrintf(DISPLAY_ROW_ACTION, "");
      break;
    case sl_bt_evt_gatt_server_characteristic_status_id:
      if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_measurement) {
        if (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config) {
            // check if indication flag is disabled
            if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_disable) {
                ble_data->ok_to_send_htm_indications = false;
                gpioLed0SetOff();
                displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
            }
            //check if indication flag is enabled
            if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_indication) {
                ble_data->ok_to_send_htm_indications = true;
                gpioLed0SetOn();
                if(!ble_data->indication_in_flight) {
                    indication_para_struct_t* data = read_queue();
                    if(data->charHandle == gattdb_temperature_measurement) {
                        sc = sl_bt_gatt_server_send_indication(ble_data->connection_handle, gattdb_temperature_measurement, 5, &data->buffer[0]);
                    }
                }
            }
        }
      }

      if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_button_state) {
        if (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config) {
            // check if indication flag is disabled
            if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_disable) {
                ble_data->ok_to_send_button_indications = false;
                gpioLed1SetOff();
                displayPrintf(DISPLAY_ROW_9, "");
            }
            //check if indication flag is enabled
            if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_indication) {
                ble_data->ok_to_send_button_indications = true;
                gpioLed1SetOn();
                displayPrintf(DISPLAY_ROW_9, "Button Released");
                if(!ble_data->indication_in_flight) {
                    indication_para_struct_t* data = read_queue();
                    if(data->charHandle == gattdb_button_state) {
                        sc = sl_bt_gatt_server_send_indication(ble_data->connection_handle, gattdb_button_state, 2, &data->buffer[0]);
                    }
                }
            }
        }
      }

      if (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_confirmation) {
          ble_data->indication_in_flight = false;
      }
      break;
    case sl_bt_evt_system_external_signal_id:
      if(evt->data.evt_system_external_signal.extsignals == evtPB0_pressed) {
          displayPrintf(DISPLAY_ROW_9, "Button Pressed");

          if(!ble_data->bonded) {
              sc = sl_bt_sm_passkey_confirm(ble_data->connection_handle, 1);
              if(sc != SL_STATUS_OK) {
                  LOG_ERROR("sl_bt_sm_passkey_confirm failed with error code: 0x%x", (unsigned int)sc);
              }
          } else {
              ble_send_button_state(1);
          }
      } else if (evt->data.evt_system_external_signal.extsignals == evtPB0_released) {
          displayPrintf(DISPLAY_ROW_9, "Button Released");
          if(ble_data->bonded) {
              ble_send_button_state(0);
          }
      }
      break;
    case sl_bt_evt_gatt_server_indication_timeout_id:
      ble_data->ok_to_send_htm_indications = false;
      ble_data->ok_to_send_button_indications = false;
      break;
#else
    case sl_bt_evt_scanner_legacy_advertisement_report_id:
      // Is the bd_addr, event_flag and address_type what we expect for our Server?
      if((evt->data.evt_scanner_legacy_advertisement_report.event_flags == (SL_BT_SCANNER_EVENT_FLAG_CONNECTABLE | SL_BT_SCANNER_EVENT_FLAG_SCANNABLE))
          && (evt->data.evt_scanner_legacy_advertisement_report.address_type == sl_bt_gap_public_address)) {
          if(memcmp(evt->data.evt_scanner_legacy_advertisement_report.address.addr, server_bt_address, sizeof(server_bt_address)) == 0) {
              sc = sl_bt_scanner_stop();
              if(sc != SL_STATUS_OK) {
                  LOG_ERROR("sl_bt_scanner_stop failed with error code: 0x%x", (unsigned int)sc);
              }
              sc = sl_bt_connection_open(evt->data.evt_scanner_legacy_advertisement_report.address,
                                    evt->data.evt_scanner_legacy_advertisement_report.address_type,
                                    sl_bt_gap_phy_1m,
                                    NULL);
              if(sc != SL_STATUS_OK) {
                  LOG_ERROR("sl_bt_connection_open failed with error code: 0x%x", (unsigned int)sc);
              }
          }
      }
      break;
    case sl_bt_evt_gatt_service_id:
      // This event is generated after issuing the  sl_bt_gatt_discover_primary_services_by_uuid command.
      ble_data->service_handle = evt->data.evt_gatt_service.service;
      break;
    case sl_bt_evt_gatt_characteristic_id:
      // This event is generated after issuing either the sl_bt_gatt_discover_characteristics_by_uuid command.
      ble_data->characteristic_handle = evt->data.evt_gatt_characteristic.characteristic;
      break;
    case sl_bt_evt_gatt_procedure_completed_id:
      // We get this event when it's ok to call the next GATT command
      ble_data->gatt_procedure_inprogress = false;
      break;
    case sl_bt_evt_gatt_characteristic_value_id:
      // Send a confirmation to a remote GATT server after receiving a characteristic indication
      displayPrintf(DISPLAY_ROW_CONNECTION, "Handling Indications");
      if((evt->data.evt_gatt_characteristic_value.att_opcode == sl_bt_gatt_handle_value_indication) &&
          (evt->data.evt_gatt_characteristic.characteristic == ble_data->characteristic_handle)) {
          sc = sl_bt_gatt_send_characteristic_confirmation(ble_data->connection_handle);
          if(sc != SL_STATUS_OK) {
              LOG_ERROR("sl_bt_gatt_send_characteristic_confirmation failed with error code: 0x%x", (unsigned int)sc);
          }
          uint32_t temperature_in_c = FLOAT_TO_INT32(&(evt->data.evt_gatt_characteristic_value.value.data[0]));
          displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp=%d", temperature_in_c);
      }
      break;
#endif
  }
}

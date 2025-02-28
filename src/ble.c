#include "sl_bluetooth.h"
#include "sl_bt_api.h"
#include "gatt_db.h"

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#include "ble.h"
#include "i2c.h"
#include "lcd.h"

#define INTERVAL_MIN  400u  // 250/0.625
#define INTERVAL_MAX  400u  // 250/0.625
#define DEFAULT_DURATION  0u  // Value 0 indicates no advertising duration limit and advertising continues until it is disabled.
#define DEFAULT_MAX_EVENTS  0u  // Value 0 indicates no maximum number limit.
#define CONN_INTERVAL_MIN  60u  // 75/1.25
#define CONN_INTERVAL_MAX  60u  // 75/1.25
#define PERIPHERAL_LATENCY  4u  // 300/75
#define SUPERVISION_TIMEOUT  80u            // > ((1 + latency) * max_interval * 2)/10 = 75
#define DEFAULT_MIN_CE_LENGTH  0x0000
#define DEFAULT_MAX_CE_LENGTH  0xffff

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

  uint32_t temperature_in_c = get_temp_value();
  UINT8_TO_BITSTREAM(p, flags); // insert the flags byte
  htm_temperature_flt = INT32_TO_FLOAT(temperature_in_c*1000, -3);
  UINT32_TO_BITSTREAM(p, htm_temperature_flt);  // insert the temperature measurement

  sc = sl_bt_gatt_server_write_attribute_value(gattdb_temperature_measurement, 0, 5, &htm_temperature_buffer[0]);
  if (sc != SL_STATUS_OK) {
      LOG_ERROR("sl_bt_gatt_server_write_attribute_value failed with error code: 0x%x", (unsigned int)sc);
  }

  if ((ble_data->connection_open == true) && (ble_data->ok_to_send_htm_indications == true) && (ble_data->indication_in_flight == false)) {
    sc = sl_bt_gatt_server_send_indication(ble_data->connection_handle, gattdb_temperature_measurement, 5, &htm_temperature_buffer[0]);
    if (sc != SL_STATUS_OK) {
        LOG_ERROR("sl_bt_gatt_server_send_indication failed with error code: 0x%x", (unsigned int)sc);
    } else {
        ble_data->indication_in_flight = true;
    }
    displayPrintf(DISPLAY_ROW_TEMPVALUE, "Temp=%d", temperature_in_c);
  }
}

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
      displayPrintf(DISPLAY_ROW_NAME, "Server");

      sc = sl_bt_system_get_identity_address(&ble_data->myAddress, &ble_data->myAddress_type);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_system_get_identity_address failed with error code: 0x%x", (unsigned int)sc);
      }
      displayPrintf(DISPLAY_ROW_BTADDR, "%x:%x:%x:%x:%x:%x", ble_data->myAddress.addr[0], ble_data->myAddress.addr[1], ble_data->myAddress.addr[2], ble_data->myAddress.addr[3], ble_data->myAddress.addr[4], ble_data->myAddress.addr[5]);

      sc = sl_bt_advertiser_create_set(&ble_data->advertisingSetHandle);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_create_set failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_advertiser_set_timing(ble_data->advertisingSetHandle, INTERVAL_MIN, INTERVAL_MAX, DEFAULT_DURATION, DEFAULT_MAX_EVENTS);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_set_timing failed with error code: 0x%x", (unsigned int)sc);
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
      displayPrintf(DISPLAY_ROW_ASSIGNMENT, "A6");
      break;
    case sl_bt_evt_connection_opened_id:
      ble_data->connection_open = true;
      ble_data->connection_handle = evt->data.evt_connection_opened.connection;

      sc = sl_bt_advertiser_stop(ble_data->advertisingSetHandle);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_advertiser_stop failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_connection_set_parameters(ble_data->connection_handle, CONN_INTERVAL_MIN, CONN_INTERVAL_MAX, PERIPHERAL_LATENCY, SUPERVISION_TIMEOUT, DEFAULT_MIN_CE_LENGTH, DEFAULT_MAX_CE_LENGTH);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_connection_set_parameters failed with error code: 0x%x", (unsigned int)sc);
      }
      displayPrintf(DISPLAY_ROW_CONNECTION, "Connected");
      break;
    case sl_bt_evt_connection_closed_id:
      ble_data->connection_open = false;
      sc = sl_bt_legacy_advertiser_generate_data(ble_data->advertisingSetHandle, sl_bt_advertiser_general_discoverable);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_legacy_advertiser_generate_data failed with error code: 0x%x", (unsigned int)sc);
      }

      sc = sl_bt_legacy_advertiser_start(ble_data->advertisingSetHandle, sl_bt_legacy_advertiser_connectable);
      if(sc != SL_STATUS_OK) {
          LOG_ERROR("sl_bt_legacy_advertiser_start failed with error code: 0x%x", (unsigned int)sc);
      }
      displayPrintf(DISPLAY_ROW_CONNECTION, "Advertising");
      displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
      break;
    case sl_bt_evt_connection_parameters_id:
      // Commented below log out as asked after observing
      //LOG_INFO("Connection parameters: interval: %d, latency: %d, timeout: %d", (int)(evt->data.evt_connection_parameters.interval * 1.25), evt->data.evt_connection_parameters.latency, evt->data.evt_connection_parameters.timeout * 10 );
      break;
    case sl_bt_evt_system_external_signal_id:
      break;
    case sl_bt_evt_system_soft_timer_id:
      displayUpdate();
      break;
    case sl_bt_evt_gatt_server_characteristic_status_id:
      if(evt->data.evt_gatt_server_characteristic_status.characteristic == gattdb_temperature_measurement) {
        if (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_client_config) {
            // check if indication flag is disabled
            if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_disable) {
                ble_data->ok_to_send_htm_indications = false;
                displayPrintf(DISPLAY_ROW_TEMPVALUE, "");
            }
            //check if indication flag is enabled
            if(evt->data.evt_gatt_server_characteristic_status.client_config_flags == gatt_indication) {
                ble_data->ok_to_send_htm_indications = true;
            }
        }
        if (evt->data.evt_gatt_server_characteristic_status.status_flags == sl_bt_gatt_server_confirmation) {
            ble_data->indication_in_flight = false;
        }
      }
      break;
    case sl_bt_evt_gatt_server_indication_timeout_id:
      ble_data->ok_to_send_htm_indications = false;
      break;
  }
}

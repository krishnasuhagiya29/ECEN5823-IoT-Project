#include "sl_bgapi.h"

#define UINT8_TO_BITSTREAM(p, n) { *(p)++ = (uint8_t)(n); }
#define UINT32_TO_BITSTREAM(p, n) { *(p)++ = (uint8_t)(n); *(p)++ = (uint8_t)((n) >> 8); \
*(p)++ = (uint8_t)((n) >> 16); *(p)++ = (uint8_t)((n) >> 24); }
#define INT32_TO_FLOAT(m, e) ( (int32_t) (((uint32_t) m) & 0x00FFFFFFU) | (((uint32_t) e) << 24) )

typedef struct {
  // values that are common to servers and clients
  bd_addr myAddress;
  uint8_t myAddress_type;
  // values unique for server
  uint8_t advertisingSetHandle;   // The advertising set handle allocated from Bluetooth stack.
  bool connection_open; //  true when in an open connection
  uint8_t connection_handle;
  bool ok_to_send_htm_indications; // true when client enabled indications
  bool indication_in_flight;  // true when an indication is in-flight
  // values unique for client
} ble_data_struct_t;

/****************************************************************************
 * @brief   Retrieves a pointer to the BLE data structure.
 * @param   None
 * @return  Pointer to the ble_data_struct_t structure.
 ****************************************************************************/
ble_data_struct_t* get_ble_data_ptr(void);

/****************************************************************************
 * @brief   Sends the temperature value over bluetooth to the client.
 * @param   None
 * @return  None
 ****************************************************************************/
void ble_send_temp_value(void);

/****************************************************************************
 * @brief   Handles Bluetooth events and processes them accordingly.
 * @param   evt  Pointer to the sl_bt_msg_t structure containing the event data.
 * @return  None
 ****************************************************************************/
void handle_ble_event(sl_bt_msg_t *evt);

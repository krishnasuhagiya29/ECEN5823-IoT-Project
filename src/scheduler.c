/***************************************************************************
 * @file      scheduler.c
 * @brief     The scheduler functions definitions.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      02-05-2025
 * @resources None
 *
 ******************************************************************************/

#include "sl_power_manager.h"
#include "sl_bt_api.h"
#include "em_core.h"

// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#include "ble.h"
#include "i2c.h"
#include "timers.h"

static uint32_t events = 0;

typedef enum uint32_t {
  STATE_IDLE,
  STATE_SI7021_ON,
  STATE_I2C_SEND,
  STATE_I2C_SEND_COMPLETE,
  STATE_I2C_READ_COMPLETE
} state_t;

/****************************************************************************
 * @brief   Sets the LETIMER0 underflow event in the scheduler.
 * @param   None
 * @return  None
 ****************************************************************************/
void schedulerSetEventUF(void) {
  CORE_DECLARE_IRQ_STATE; // set event
  CORE_ENTER_CRITICAL(); // enter critical, turn off interrupts in NVIC
  sl_bt_external_signal(evtLETIMER0_UF);
  //events |= evtLETIMER0_UF; // RMW 0xb0001
  CORE_EXIT_CRITICAL(); // exit critical, re-enable interrupts in NVIC
} // schedulerSetEventUF

/****************************************************************************
 * @brief   Sets the LETIMER0 COMP1 event in the scheduler.
 * @param   None
 * @return  None
 ****************************************************************************/
void schedulerSetEventCOMP1(void) {
  CORE_DECLARE_IRQ_STATE; // set event
  CORE_ENTER_CRITICAL(); // enter critical, turn off interrupts in NVIC
  sl_bt_external_signal(evtLETIMER0_COMP1);
  //events |= evtLETIMER0_COMP1; // RMW 0xb0010
  CORE_EXIT_CRITICAL(); // exit critical, re-enable interrupts in NVIC
} // schedulerSetEventCOMP1

/****************************************************************************
 * @brief   Sets the I2C0 done event in the scheduler.
 * @param   None
 * @return  None
 ****************************************************************************/
void schedulerSetI2C0Event(void) {
  CORE_DECLARE_IRQ_STATE; // set event
  CORE_ENTER_CRITICAL(); // enter critical, turn off interrupts in NVIC
  sl_bt_external_signal(evtI2C0_done);
  //events |= evtI2C0_done; // RMW 0xb0100
  CORE_EXIT_CRITICAL(); // exit critical, re-enable interrupts in NVIC
} // schedulerSetI2C0Event

/****************************************************************************
 * @brief   Retrieves the next scheduled event.
 * @param   None
 * @return  The next event as a 32-bit unsigned integer.
 ****************************************************************************/
// scheduler routine to return 1 event to main()code and clear that event
uint32_t getNextEvent(void) {
  uint32_t theEvent;
  if(events & evtLETIMER0_UF) {
      theEvent = events & evtLETIMER0_UF; // 1 event to return to the caller
      CORE_DECLARE_IRQ_STATE;
      CORE_ENTER_CRITICAL();  // enter critical section
      events &= ~evtLETIMER0_UF;  // clear the UF event in the data structure, RMW
      CORE_EXIT_CRITICAL(); // exit critical section
  } else if(events & evtLETIMER0_COMP1) {
      theEvent = events & evtLETIMER0_COMP1; // 1 event to return to the caller
      CORE_DECLARE_IRQ_STATE;
      CORE_ENTER_CRITICAL();  // enter critical section
      events &= ~evtLETIMER0_COMP1;  // clear the COMP1 event in the data structure, RMW
      CORE_EXIT_CRITICAL(); // exit critical section
  } else if(events & evtI2C0_done) {
      theEvent = events & evtI2C0_done; // 1 event to return to the caller
      CORE_DECLARE_IRQ_STATE;
      CORE_ENTER_CRITICAL();  // enter critical section
      events &= ~evtI2C0_done;  // clear the I2C0 done event in the data structure, RMW
      CORE_EXIT_CRITICAL(); // exit critical section
  }
  return (theEvent);
} // getNextEvent()

/****************************************************************************
 * @brief   State machine for handling temperature measurement events.
 * @param   Pointer to the sl_bt_msg_t structure.
 * @return  None
 ****************************************************************************/
void temperature_state_machine(sl_bt_msg_t* evt) {
  state_t current_state;
  static state_t next_state = STATE_IDLE;
  ble_data_struct_t* ble_data = get_ble_data_ptr();

  if(SL_BT_MSG_ID(evt->header) == sl_bt_evt_system_external_signal_id) {
      if((ble_data->connection_open == true) && (ble_data->ok_to_send_htm_indications == true)) {
          current_state = next_state;
          switch(current_state) {
            case STATE_IDLE:
              //LOG_INFO("In STATE_IDLE\n");
              next_state = STATE_IDLE;
              if(evt->data.evt_system_external_signal.extsignals == evtLETIMER0_UF){
                  si7021_power_on();
                  timerWaitUs_irq(80000);
                  next_state = STATE_SI7021_ON;
              }
              break;
            case STATE_SI7021_ON:
              //LOG_INFO("In STATE_SI7021_ON\n");
              next_state = STATE_SI7021_ON;
              if(evt->data.evt_system_external_signal.extsignals == evtLETIMER0_COMP1){
                  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
                  send_I2C_command();
                  next_state = STATE_I2C_SEND;
              }
              break;
            case STATE_I2C_SEND:
              //LOG_INFO("In STATE_I2C_SEND\n");
              next_state = STATE_I2C_SEND;
              if(evt->data.evt_system_external_signal.extsignals == evtI2C0_done) {
                  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
                  timerWaitUs_irq(10800);
                  next_state = STATE_I2C_SEND_COMPLETE;
              }
              break;
            case STATE_I2C_SEND_COMPLETE:
              //LOG_INFO("In STATE_I2C_SEND_COMPLETE\n");
              next_state = STATE_I2C_SEND_COMPLETE;
              if(evt->data.evt_system_external_signal.extsignals == evtLETIMER0_COMP1){
                  sl_power_manager_add_em_requirement(SL_POWER_MANAGER_EM1);
                  read_I2C_response();
                  next_state = STATE_I2C_READ_COMPLETE;
              }
              break;
            case STATE_I2C_READ_COMPLETE:
              //LOG_INFO("In STATE_I2C_READ_COMPLETE\n");
              next_state = STATE_I2C_READ_COMPLETE;
              if(evt->data.evt_system_external_signal.extsignals == evtI2C0_done) {
                  //si7021_power_off();
                  sl_power_manager_remove_em_requirement(SL_POWER_MANAGER_EM1);
                  ble_send_temp_value();
                  //uint32_t temp = get_temp_value();
                  //LOG_INFO ("Temperature=%lu\r\n", temp);
                  next_state = STATE_IDLE;
              }
              break;
          }
      }
  }
} // temperature_state_machine


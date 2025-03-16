/***************************************************************************
 * @file      scheduler.h
 * @brief     The scheduler functions declarations.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      02-05-2025
 * @resources None
 *
 ******************************************************************************/

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include "sl_bt_api.h"

/****************************************************************************
 * @brief   Sets the LETIMER0 underflow event in the scheduler.
 * @param   None
 * @return  None
 ****************************************************************************/
void schedulerSetEventUF(void);

/****************************************************************************
 * @brief   Sets the LETIMER0 COMP1 event in the scheduler.
 * @param   None
 * @return  None
 ****************************************************************************/
void schedulerSetEventCOMP1(void);

/****************************************************************************
 * @brief   Sets the I2C0 done event in the scheduler.
 * @param   None
 * @return  None
 ****************************************************************************/
void schedulerSetI2C0Event(void);

void schedulerSetEventPB0Pressed(void);

void schedulerSetEventPB0Released(void);

/****************************************************************************
 * @brief   Retrieves the next scheduled event.
 * @param   None
 * @return  The next event as a 32-bit unsigned integer.
 ****************************************************************************/
uint32_t getNextEvent(void);

#if DEVICE_IS_BLE_SERVER
/****************************************************************************
 * @brief   State machine for handling temperature measurement events on the server side.
 * @param   Pointer to the sl_bt_msg_t structure.
 * @return  None
 ****************************************************************************/
void temperature_state_machine(sl_bt_msg_t* evt);
#else
/****************************************************************************
 * @brief   State machine for handling temperature measurement events on the client side.
 * @param   Pointer to the sl_bt_msg_t structure.
 * @return  None
 ****************************************************************************/
void discovery_state_machine(sl_bt_msg_t* evt);
#endif
#endif /* SCHEDULER_H_ */

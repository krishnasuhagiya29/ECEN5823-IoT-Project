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

/****************************************************************************
 * @brief   Retrieves the next scheduled event.
 * @param   None
 * @return  The next event as a 32-bit unsigned integer.
 ****************************************************************************/
uint32_t getNextEvent(void);

/****************************************************************************
 * @brief   State machine for handling temperature measurement events.
 * @param   event The event to be processed.
 * @return  None
 ****************************************************************************/
void temperature_state_machine(uint32_t event);

#endif /* SCHEDULER_H_ */

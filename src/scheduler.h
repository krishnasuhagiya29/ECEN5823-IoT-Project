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
 * A scheduler function to set the an LETIMER0 underflow event .
 ****************************************************************************/
void schedulerSetEventUF();

/****************************************************************************
 * A function to get the next event.
 ****************************************************************************/
uint32_t getNextEvent();

#endif /* SCHEDULER_H_ */

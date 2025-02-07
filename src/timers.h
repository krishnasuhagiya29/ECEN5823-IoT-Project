/***************************************************************************
 * @file      timers.h
 * @brief     The timer functions declarations.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      01-29-2025
 * @resources None
 *
 ***************************************************************************/

#ifndef TIMERS_H_
#define TIMERS_H_

#include "app.h"

#define ACTUAL_CLK_FREQ CLOCK_FREQ/PRESCALER_VALUE
#define LETIMER_ON_TIME_MS  175
#define LETIMER_PERIOD_MS 3000
#define COMP0_VALUE_TO_LOAD (LETIMER_PERIOD_MS * ACTUAL_CLK_FREQ)/1000
#define COMP1_VALUE_TO_LOAD (LETIMER_ON_TIME_MS * ACTUAL_CLK_FREQ)/1000

/****************************************************************************
 * LETIMER0 Init.
 ****************************************************************************/
void initLETIMER0();

/****************************************************************************
 * Timer wait function for microseconds wait.
 ****************************************************************************/
void timerWaitUs(uint32_t us_wait);

#endif /* TIMERS_H_ */

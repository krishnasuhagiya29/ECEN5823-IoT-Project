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
 * @brief   Initializes the LETIMER0 peripheral.
 * @param   None
 * @return  None
 ****************************************************************************/
void initLETIMER0(void);

/****************************************************************************
 * @brief   Waits for the specified time in microseconds using polling mode.
 * @param   us_wait The wait time in microseconds.
 * @return  None
 ****************************************************************************/
void timerWaitUs_polled(uint32_t us_wait);

/****************************************************************************
 * @brief   Waits for the specified time in microseconds using interrupt mode.
 * @param   us_wait The wait time in microseconds.
 * @return  None
 ****************************************************************************/
void timerWaitUs_irq(uint32_t us_wait);

#endif /* TIMERS_H_ */

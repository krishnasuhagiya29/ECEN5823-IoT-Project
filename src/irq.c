/***************************************************************************
 * @file      irq.c
 * @brief     The interrupt handler definitions.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      01-29-2025
 * @resources None
 *
 ******************************************************************************/

#include "em_letimer.h"
#include "irq.h"
#include "gpio.h"

void LETIMER0_IRQHandler(void) {
  int flags;
  flags = LETIMER_IntGetEnabled(LETIMER0);
  LETIMER_IntClear(LETIMER0, flags);
  if (flags & LETIMER_IF_UF) {
      gpioLed0SetOff(); // Turn the LED off every 2.25 seconds i.e., when underflow is detected
  }

  if (flags & LETIMER_IF_COMP1) {
      gpioLed0SetOn();  // Turn the LED for 175 milliseconds i.e., when COMP1 interrupt is triggered
  }
} // LETIMER0_IRQHandler

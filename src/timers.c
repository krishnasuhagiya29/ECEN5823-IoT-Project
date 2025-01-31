/***************************************************************************
 * @file      timers.c
 * @brief     The timer functions definitions.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      01-29-2025
 * @resources None
 *
 ******************************************************************************/

#include <stdint.h>
#include <stdbool.h>
#include "em_letimer.h"
#include "timers.h"


void initLETIMER0() {
  uint32_t temp;

  const LETIMER_Init_TypeDef letimerInitData = {
    false, // enable; don't enable when init completes, we'll enable last
    true, // debugRun; have the timer running when single-stepping in the debugger
    true, // comp0Top; load COMP0 into CNT on underflow
    false, // bufTop; don't load COMP1 into COMP0 when REP0==0
    0, // out0Pol; 0 default output pin value
    0, // out1Pol; 0 default output pin value
    letimerUFOANone, // ufoa0; no underflow output action
    letimerUFOANone, // ufoa1; no underflow output action
    letimerRepeatFree, // repMode; free running mode i.e. load & go forever
    0 // COMP0(top) Value
  };
  LETIMER_Init (LETIMER0, &letimerInitData); // initialize the timer
  LETIMER_CompareSet(LETIMER0, 0, COMP0_VALUE_TO_LOAD); // Load COMP0 (top)
  LETIMER_CompareSet(LETIMER0, 1, COMP1_VALUE_TO_LOAD); // Load COMP1
  LETIMER_IntClear (LETIMER0, 0xFFFFFFFF);  // Clear all IRQ flags in the LETIMER0 IF status register
  temp = LETIMER_IEN_UF | LETIMER_IEN_COMP1;  // Set UF and COMP1 in LETIMER0_IEN, so that the timer will generate IRQs to the NVIC.
  LETIMER_IntEnable (LETIMER0, temp);
  LETIMER_Enable (LETIMER0, true);  // Enable the timer to starting counting down
}

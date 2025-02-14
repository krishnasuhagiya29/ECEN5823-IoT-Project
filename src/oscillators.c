/***************************************************************************
 * @file      oscillators.c
 * @brief     The oscillator functions definitions.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      01-29-2025
 * @resources None
 *
 ******************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include "em_cmu.h"
#include "oscillators.h"
#include "app.h"

void initOscillator(void) {
#if (LOWEST_ENERGY_MODE==EM3)
  CMU_OscillatorEnable(cmuOsc_ULFRCO, true, true);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_ULFRCO);
#else
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
#endif
  CMU_ClockEnable(cmuClock_LFA, true);  // Enable the Low-frequency A clock
  CMU_ClockDivSet(cmuClock_LETIMER0, PRESCALER_VALUE);  // Set the clock divider
  CMU_ClockEnable(cmuClock_LETIMER0, true); // Enable the LETIMER0 clock

  /*// Enable this code for debugging
  CMU_Select_TypeDef freq_index = CMU_ClockSelectGet(cmuClock_LFA);
  printf("LFA clock selection: %u\r\n", freq_index);
  CMU_ClkDiv_TypeDef div = CMU_ClockDivGet(cmuClock_LETIMER0);
  printf("clock divider value: %lu\r\n", div);
  uint32_t lfa_freq = CMU_ClockFreqGet(cmuClock_LFA);
  printf("LFA clock frequency: %lu\r\n", lfa_freq);
  uint32_t letimer0_freq = CMU_ClockFreqGet(cmuClock_LETIMER0);
  printf("LETIMER0 clock frequency: %lu\r\n", letimer0_freq);
  */
}

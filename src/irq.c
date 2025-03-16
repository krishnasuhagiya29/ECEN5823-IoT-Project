/***************************************************************************
 * @file      irq.c
 * @brief     The interrupt handler definitions.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      01-29-2025
 * @resources None
 *
 ******************************************************************************/

#include "em_letimer.h"
#include "em_gpio.h"

#include "ble.h"
#include "irq.h"
#include "gpio.h"
#include "scheduler.h"
#include "timers.h"

uint32_t rollover_count = 0;

void GPIO_EVEN_IRQHandler(void) {
  int flags;
  flags = GPIO_IntGetEnabled();
  GPIO_IntClear(flags);

  ble_data_struct_t* ble_data = get_ble_data_ptr();
  if(flags & (1 << PB0_pin)){
      uint32_t value = GPIO_PinInGet(PB0_port, PB0_pin);
      if(value) {
          ble_data->PB0_pressed = false;
          schedulerSetEventPB0Released();
      } else {
          ble_data->PB0_pressed = true;
          schedulerSetEventPB0Pressed();
      }
  }
}

void LETIMER0_IRQHandler(void) {
  int flags;
  flags = LETIMER_IntGetEnabled(LETIMER0);
  LETIMER_IntClear(LETIMER0, flags);
  if (flags & LETIMER_IF_UF) {
      schedulerSetEventUF();
      rollover_count++;
  }

  if (flags & LETIMER_IF_COMP1) {
      schedulerSetEventCOMP1();
      LETIMER_IntDisable(LETIMER0, LETIMER_IEN_COMP1);
  }
} // LETIMER0_IRQHandler

uint32_t letimerMilliseconds(void) {
  uint32_t current_time = (rollover_count * LETIMER_PERIOD_MS) + (((COMP0_VALUE_TO_LOAD - LETIMER_CounterGet(LETIMER0)) * ACTUAL_CLK_FREQ)/1000);
  return current_time;
}

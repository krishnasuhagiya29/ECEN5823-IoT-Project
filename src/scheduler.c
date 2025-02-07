/***************************************************************************
 * @file      scheduler.c
 * @brief     The scheduler functions definitions.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      02-05-2025
 * @resources None
 *
 ******************************************************************************/

#include "em_core.h"

#include "app.h"

static uint32_t events = 0;

void schedulerSetEventUF() {
  CORE_DECLARE_IRQ_STATE; // set event
  CORE_ENTER_CRITICAL(); // enter critical, turn off interrupts in NVIC
  events |= evtLETIMER0_UF; // RMW 0xb0001
  CORE_EXIT_CRITICAL(); // exit critical, re-enable interrupts in NVIC
} // schedulerSetEventUF

// scheduler routine to return 1 event to main()code and clear that event
uint32_t getNextEvent() {
  uint32_t theEvent;
  theEvent = events & evtLETIMER0_UF; // 1 event to return to the caller
  CORE_DECLARE_IRQ_STATE;
  CORE_ENTER_CRITICAL();  // enter critical section
  events &= ~evtLETIMER0_UF;  // clear the UF event in your data structure, RMW
  CORE_EXIT_CRITICAL(); // exit critical section
  return (theEvent);
} // getNextEvent()

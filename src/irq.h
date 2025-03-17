/***************************************************************************
 * @file      irq.h
 * @brief     The interrupt handler declarations.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      01-29-2025
 * @resources None
 *
 ***************************************************************************/

#ifndef IRQ_H_
#define IRQ_H_

#include <stdint.h>

/****************************************************************************
 * @brief   Handles the GPIO even pin interrupt.
 * @param   None
 * @return  None
 ****************************************************************************/
void GPIO_EVEN_IRQHandler(void);

/****************************************************************************
 * @brief   Handles the LETIMER0 interrupt.
 * @param   None
 * @return  None
 ****************************************************************************/
void LETIMER0_IRQHandler(void);

/****************************************************************************
 * @brief   Returns the elapsed time in milliseconds based on LETIMER0.
 * @param   None
 * @return  Elapsed time in milliseconds.
 ****************************************************************************/
uint32_t letimerMilliseconds(void);

#endif /* IRQ_H_ */

/***************************************************************************//**
 * @file
 * @brief Application interface provided to main().
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 *******************************************************************************
 *
 * SPDX-License-Identifier: Zlib
 *
 * The licensor of this software is Silicon Laboratories Inc.
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 *
 *
 * Editor: Feb 26, 2022, Dave Sluiter
 * Change: Added comment about use of .h files.
 *
 *
 *
 * Student edit: Add your name and email address here:
 * @student    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 *
 *
 ******************************************************************************/

// Students: Remember, a header file (a .h file) defines an interface
//           for functions defined within an implementation file (a .c file).
//           The .h file defines what a caller (a user) of a .c file requires.
//           At a minimum, the .h file should define the publicly callable
//           functions, i.e. define the function prototypes. #define and type
//           definitions can be added if the caller requires theses.

#ifndef APP_H
#define APP_H

#define EM0 0
#define EM1 1
#define EM2 2
#define EM3 3

// Only define 1 of these to define the lowest energy mode
// EM0 = highest energy mode, EM3 = lowest energy mode
//#define LOWEST_ENERGY_MODE EM0
//#define LOWEST_ENERGY_MODE EM1
#define LOWEST_ENERGY_MODE EM2
//#define LOWEST_ENERGY_MODE EM3

#if (LOWEST_ENERGY_MODE==EM3)
#define CLOCK_FREQ  1000
#define PRESCALER_VALUE 1
#else
#define CLOCK_FREQ  32768
#define PRESCALER_VALUE 4
#endif

typedef enum {
  evtLETIMER0_UF = 1,
  evtLETIMER0_COMP1 = 2,
  evtI2C0_done = 4,
  evtPB0_pressed = 8,
  evtPB0_released = 16
} event_type_t;

/**************************************************************************//**
 * Application Init.
 *****************************************************************************/
void app_init(void);

/**************************************************************************//**
 * Application Process Action.
 *****************************************************************************/
void app_process_action(void);

#endif // APP_H

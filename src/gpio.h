/*
   gpio.h
  
    Created on: Dec 12, 2018
        Author: Dan Walkes

    Updated by Dave Sluiter Sept 7, 2020. moved #defines from .c to .h file.
    Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

    Editor: Feb 26, 2022, Dave Sluiter
    Change: Added comment about use of .h files.

 *
 * @student    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 *
 
 */


// Students: Remember, a header file (a .h file) generally defines an interface
//           for functions defined within an implementation file (a .c file).
//           The .h file defines what a caller (a user) of a .c file requires.
//           At a minimum, the .h file should define the publicly callable
//           functions, i.e. define the function prototypes. #define and type
//           definitions can be added if the caller requires theses.


#ifndef SRC_GPIO_H_
#define SRC_GPIO_H_

#define PB0_port   (gpioPortF)
#define PB0_pin    (6)

// Function prototypes
/****************************************************************************
 * @brief   Initializes the GPIO pins.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioInit();

/****************************************************************************
 * @brief   Turns on LED0.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioLed0SetOn();

/****************************************************************************
 * @brief   Turns off LED0.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioLed0SetOff();

/****************************************************************************
 * @brief   Turns on LED1.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioLed1SetOn();

/****************************************************************************
 * @brief   Turns off LED1.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioLed1SetOff();

/****************************************************************************
 * @brief   Enables power to the Si7021 sensor.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioSi7021SetOn();

/****************************************************************************
 * @brief   Disables power to the Si7021 sensor.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioSi7021SetOff();

/****************************************************************************
 * @brief   Sets the state of the display EXTCOMIN signal.
 * @param   state  Boolean value indicating the desired EXTCOMIN state.
 * @return  None
 ****************************************************************************/
void gpioSetDisplayExtcomin(bool state);



#endif /* SRC_GPIO_H_ */

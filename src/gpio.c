/*
  gpio.c
 
   Created on: Dec 12, 2018
       Author: Dan Walkes
   Updated by Dave Sluiter Dec 31, 2020. Minor edits with #defines.

   March 17
   Dave Sluiter: Use this file to define functions that set up or control GPIOs.
   
   Jan 24, 2023
   Dave Sluiter: Cleaned up gpioInit() to make it less confusing for students regarding
                 drive strength setting. 

 *
 * @student    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 *
 
 */


// *****************************************************************************
// Students:
// We will be creating additional functions that configure and manipulate GPIOs.
// For any new GPIO function you create, place that function in this file.
// *****************************************************************************

#include <stdbool.h>
#include "em_gpio.h"
#include <string.h>

#include "gpio.h"


// See the radio board user guide at https://www.silabs.com/documents/login/user-guides/ug279-brd4104a-user-guide.pdf
// and GPIO documentation at https://siliconlabs.github.io/Gecko_SDK_Doc/efm32g/html/group__GPIO.html
// to determine the correct values for these.
// If these links have gone bad, consult the reference manual and/or the datasheet for the MCU.
// Change to correct port and pins:
#define LED_port   (gpioPortF)
#define LED0_pin   (4)
#define LED1_pin   (5)

#define si7021_port (gpioPortD)
#define si7021_pin  (15)

#define LCD_port (gpioPortD)
#define LCD_pin_extcomin  (13)

/****************************************************************************
 * @brief   Initializes the GPIO pins.
 * @param   None
 * @return  None
 ****************************************************************************/
// Set GPIO drive strengths and modes of operation
void gpioInit()
{
    // Set the port's drive strength. In this MCU implementation, all GPIO cells
    // in a "Port" share the same drive strength setting. 
	//GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthStrongAlternateStrong); // Strong, 10mA
	GPIO_DriveStrengthSet(LED_port, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
	GPIO_DriveStrengthSet(LCD_port, gpioDriveStrengthWeakAlternateWeak); // Weak, 1mA
	
	// Set the 2 GPIOs mode of operation
	GPIO_PinModeSet(LED_port, LED0_pin, gpioModePushPull, false);
	GPIO_PinModeSet(LED_port, LED1_pin, gpioModePushPull, false);

	GPIO_PinModeSet(si7021_port, si7021_pin, gpioModePushPull, false);
	GPIO_PinModeSet(LCD_port, LCD_pin_extcomin, gpioModePushPull, false);
	GPIO_PinModeSet(PB0_port, PB0_pin, gpioModeInputPull, 1);
	GPIO_ExtIntConfig (PB0_port, PB0_pin, PB0_pin, true, true, true);

} // gpioInit()

/****************************************************************************
 * @brief   Turns on LED0.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioLed0SetOn()
{
	GPIO_PinOutSet(LED_port, LED0_pin);
}

/****************************************************************************
 * @brief   Turns off LED0.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioLed0SetOff()
{
	GPIO_PinOutClear(LED_port, LED0_pin);
}

/****************************************************************************
 * @brief   Turns on LED1.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioLed1SetOn()
{
	GPIO_PinOutSet(LED_port, LED1_pin);
}

/****************************************************************************
 * @brief   Turns off LED1.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioLed1SetOff()
{
	GPIO_PinOutClear(LED_port, LED1_pin);
}

/****************************************************************************
 * @brief   Enables power to the Si7021 sensor.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioSi7021SetOn()
{
  GPIO_PinOutSet(si7021_port, si7021_pin);
}

/****************************************************************************
 * @brief   Disables power to the Si7021 sensor.
 * @param   None
 * @return  None
 ****************************************************************************/
void gpioSi7021SetOff()
{
  GPIO_PinOutClear(si7021_port, si7021_pin);
}

/****************************************************************************
 * @brief   Sets the state of the display EXTCOMIN signal.
 * @param   state  Boolean value indicating the desired EXTCOMIN state.
 * @return  None
 ****************************************************************************/
void gpioSetDisplayExtcomin(bool state)
{
  if(state) {
      GPIO_PinOutSet(LCD_port, LCD_pin_extcomin);
  } else {
      GPIO_PinOutClear(LCD_port, LCD_pin_extcomin);
  }
}







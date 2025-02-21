/***************************************************************************
 * @file      i2c.h
 * @brief     The I2C functions declarations for SI7021 Humidity and Temperature Sensor.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      02-05-2025
 * @resources None
 *
 ***************************************************************************/

#ifndef I2C_H_
#define I2C_H_

#define TEMP_MEASURE_COMMAND 0xF3
#define SI7021_DEVICE_ADDR  0x40

/****************************************************************************
 * @brief   Turns on power to the Si7021 sensor.
 * @param   None
 * @return  None
 ****************************************************************************/
void si7021_power_on(void);

/****************************************************************************
 * @brief   Turns off power to the Si7021 sensor.
 * @param   None
 * @return  None
 ****************************************************************************/
void si7021_power_off(void);

/****************************************************************************
 * @brief   Initializes the I2C interface for communication with the Si7021.
 * @param   None
 * @return  None
 ****************************************************************************/
void initI2C(void);

/****************************************************************************
 * @brief   Sends an I2C command to the Si7021 to initiate a temperature
 *          measurement.
 * @param   None
 * @return  None
 ****************************************************************************/
void send_I2C_command(void);

/****************************************************************************
 * @brief   Reads the temperature measurement result from the Si7021 sensor
 *          via I2C.
 * @param   None
 * @return  None
 ****************************************************************************/
void read_I2C_response(void);

/****************************************************************************
 * @brief   Retrieves the temperature value in Celsius after measurement.
 * @param   None
 * @return  Temperature value in Celsius.
 ****************************************************************************/
uint32_t get_temp_value(void);

#endif /* I2C_H_ */

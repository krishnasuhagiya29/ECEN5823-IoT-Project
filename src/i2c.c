/***************************************************************************
 * @file      i2c.c
 * @brief     The i2c functions definitions for SI7021 Humidity and Temperature Sensor.
 * @author    Krishna Suhagiya, krishna.suhagiya@colorado.edu
 * @date      02-05-2025
 * @resources None
 *
 ******************************************************************************/

#include "sl_i2cspm.h"
// Include logging specifically for this .c file
#define INCLUDE_LOG_DEBUG 1
#include "src/log.h"

#include "i2c.h"
#include "gpio.h"
#include "scheduler.h"
#include "timers.h"

I2C_TransferReturn_TypeDef transferStatus;
I2C_TransferSeq_TypeDef transferSequence;
uint8_t cmd_data;
uint8_t read_data[2];

void si7021_power_on(void) {
  gpioSi7021SetOn();
}

void si7021_power_off(void) {
  gpioSi7021SetOff();
}

void I2C0_IRQHandler(void) {
  I2C_TransferReturn_TypeDef transferStatus;
  transferStatus = I2C_Transfer(I2C0);
  if (transferStatus == i2cTransferDone) {
      schedulerSetI2C0Event();
      NVIC_DisableIRQ(I2C0_IRQn);
  }
  if (transferStatus < 0) {
      LOG_ERROR("%d", transferStatus);
  }
}

void initI2C(void) {
  // Initialize the I2C hardware
  I2CSPM_Init_TypeDef I2C_Config = {
    .port = I2C0,
    .sclPort = gpioPortC,
    .sclPin = 10,
    .sdaPort = gpioPortC,
    .sdaPin = 11,
    .portLocationScl = 14,
    .portLocationSda = 16,
    .i2cRefFreq = 0,
    .i2cMaxFreq = I2C_FREQ_STANDARD_MAX,
    .i2cClhr = i2cClockHLRStandard
  };

  I2CSPM_Init(&I2C_Config);
}

void send_I2C_command(void) {
  cmd_data = TEMP_MEASURE_COMMAND;
  transferSequence.addr = SI7021_DEVICE_ADDR << 1; // shift device address left
  transferSequence.flags = I2C_FLAG_WRITE;
  transferSequence.buf[0].data = &cmd_data; // pointer to data to write
  transferSequence.buf[0].len = sizeof(cmd_data);
  NVIC_EnableIRQ(I2C0_IRQn);

  transferStatus = I2C_TransferInit (I2C0, &transferSequence);
  if (transferStatus < 0) {
      LOG_ERROR("I2C_TransferInit() Write error = %d", transferStatus);
  }
}

void read_I2C_response(void) {
  transferSequence.addr = SI7021_DEVICE_ADDR << 1; // shift device address left
  transferSequence.flags = I2C_FLAG_READ;
  transferSequence.buf[0].data = &read_data[0]; // pointer to data to read
  transferSequence.buf[0].len = sizeof(read_data);
  NVIC_EnableIRQ(I2C0_IRQn);

  transferStatus = I2C_TransferInit (I2C0, &transferSequence);
  if (transferStatus < 0) {
      LOG_ERROR ("I2CSPM_Transfer: I2C bus write of cmd=%u failed", TEMP_MEASURE_COMMAND);
  }
}

uint32_t get_temp_value() {
  uint32_t temp_value = ((175.72 * (read_data[0] << 8 | read_data[1]))/65536) - 46.85;
  return temp_value;
}

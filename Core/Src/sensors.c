/*
 * sensors.c
 *
 *  Created on: Dec 4, 2025
 *      Author: TJ
 */

#include "sensors.h"

HAL_StatusTypeDef hts221_init(I2C_HandleTypeDef *handler)
{
  uint8_t buffer[16] = {0};

  if (HAL_I2C_Mem_Read(handler, HTS221_READ, HTS221_WHO_AM_I, 1, buffer, 1, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  if (buffer[0] == 0xBC)
  {
    buffer[0] = 0x1B;	//AVGT = 0b011, AVGH = 0b011
    if (HAL_I2C_Mem_Write(handler, HTS221_WRITE, HTS221_AV_CONF, 1, buffer, 1, SENSOR_COM_TIMEOUT))
    {
      return HAL_ERROR;
    }

    buffer[0] = 0x83;	//PD = active mode, ODR = 12.5Hz
    if (HAL_I2C_Mem_Write(handler, HTS221_WRITE, HTS221_CTRL_REG, 1, buffer, 3, SENSOR_COM_TIMEOUT))
    {
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

HAL_StatusTypeDef hts221_read(I2C_HandleTypeDef *handler, hts221_typedef *data)
{
  uint8_t buffer[16] = {0};

  uint8_t H0_RH_cal = 0;
  uint8_t H1_RH_cal = 0;
  int16_t H0_OUT_cal = 0;
  int16_t H1_OUT_cal = 0;
  int16_t H_OUT_meas = 0;

  uint16_t T0_DegC_cal = 0;
  uint16_t T1_DegC_cal = 0;
  int16_t T0_OUT_cal = 0;
  int16_t T1_OUT_cal = 0;
  int16_t T_OUT_meas = 0;

  if (HAL_I2C_Mem_Read(handler, HTS221_READ, HTS221_DATA_OUT, 1, buffer, 5, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  H_OUT_meas = (buffer[2] << 8) | buffer[1];
  T_OUT_meas = (buffer[4] << 8) | buffer[3];

  if (HAL_I2C_Mem_Read(handler, HTS221_READ, HTS221_CALIBRATION_REG, 1, buffer, 16, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  H0_RH_cal = buffer[0] >> 1;
  H1_RH_cal = buffer[1] >> 1;
  H0_OUT_cal = (buffer[7] << 8) | buffer[6];
  H1_OUT_cal = (buffer[11] << 8) | buffer[10];

  data->humidity = (float)(H_OUT_meas - H0_OUT_cal) * (float)(H1_RH_cal - H0_RH_cal) / (float)(H1_OUT_cal - H0_OUT_cal) + (float)H0_RH_cal;

  T0_DegC_cal = (((buffer[5] & 0b00000011) << 8) | buffer[2]) >> 3;
  T1_DegC_cal = (((buffer[5] & 0b00001100) << 6) | buffer[3]) >> 3;
  T0_OUT_cal = (buffer[13] << 8) | buffer[12];
  T1_OUT_cal = (buffer[15] << 8) | buffer[14];

  data->temperatureC = (float)(T_OUT_meas - T0_OUT_cal) * (float)(T1_DegC_cal - T0_DegC_cal) / (float)(T1_OUT_cal - T0_OUT_cal) + (float)T0_DegC_cal;
  data->temperatureF = (data->temperatureC * 1.8) + 32;

  return HAL_OK;
}

HAL_StatusTypeDef lis3mdl_init(I2C_HandleTypeDef *handler)
{
  uint8_t buffer[16] = {0};

  if (HAL_I2C_Mem_Read(handler, LIS3MDL_READ, LIS3MDL_WHO_AM_I, 1, buffer, 1, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  if (buffer[0] == 0x3D)
  {
    buffer[0] = 0x10;	//DO = 0b100
    if (HAL_I2C_Mem_Write(handler, LIS3MDL_WRITE, LIS3MDL_CTRL_REG, 1, buffer, 5, SENSOR_COM_TIMEOUT))
    {
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

HAL_StatusTypeDef lis3mdl_read(I2C_HandleTypeDef *handler, lis3mdl_typedef *data)
{
  uint8_t buffer[16] = {0};
  int16_t raw_x = 0;
  int16_t raw_y = 0;
  int16_t raw_z = 0;

  if (HAL_I2C_Mem_Read(handler, LIS3MDL_READ, LIS3MDL_DATA_OUT, 1, buffer, 9, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  raw_x = (buffer[2] << 8) | buffer[1];
  raw_y = (buffer[4] << 8) | buffer[3];
  raw_z = (buffer[6] << 8) | buffer[5];

  data->x = (float)raw_x * 0.146;
  data->y = (float)raw_y * 0.146;
  data->z = (float)raw_z * 0.146;

  return HAL_OK;
}

HAL_StatusTypeDef lps22hb_init(I2C_HandleTypeDef *handler)
{
  uint8_t buffer[16] = {0};

  if (HAL_I2C_Mem_Read(handler, LPS22HB_READ, LPS22HB_WHO_AM_I, 1, buffer, 1, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  if (buffer[0] == 0xB1)
  {
    buffer[0] = 0x30;	//ODR = 25Hz
    buffer[1] = 0x10;	//IF_ADD_INC = enable
    buffer[2] = 0x00;
    if (HAL_I2C_Mem_Write(handler, LPS22HB_WRITE, LPS22HB_CTRL_REG, 1, buffer, 3, SENSOR_COM_TIMEOUT))
    {
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

HAL_StatusTypeDef lps22hb_read(I2C_HandleTypeDef *handler, lps22hb_typedef *data)
{
  uint8_t buffer[16] = {0};
  int32_t raw_pressure = 0;

  if (HAL_I2C_Mem_Read(handler, LPS22HB_READ, LPS22HB_DATA_OUT, 1, buffer, 4, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  raw_pressure = (buffer[3] << 16) | (buffer[2] << 8) | buffer[1];

  data->pressure = (float)raw_pressure / 4096;

  return HAL_OK;
}

HAL_StatusTypeDef lsm6dsl_init(I2C_HandleTypeDef *handler)
{
  uint8_t buffer[16] = {0};

  if (HAL_I2C_Mem_Read(handler, LSM6DSL_READ, LSM6DSL_WHO_AM_I, 1, buffer, 1, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  if (buffer[0] == 0x6A)
  {
    buffer[0] = 0x10;	//ODR_XL = 12.5Hz
    buffer[1] = 0x10;	//ODR_G = 12.5Hz
    buffer[2] = 0x44;	//BDU = continuous update, IF_INC = enable
    if (HAL_I2C_Mem_Write(handler, LSM6DSL_WRITE, LSM6DSL_CTRL_REG, 1, buffer, 3, SENSOR_COM_TIMEOUT))
    {
      return HAL_ERROR;
    }
  }
  return HAL_OK;
}

HAL_StatusTypeDef lsm6dsl_read(I2C_HandleTypeDef *handler, lsm6dsl_typedef *data)
{
  uint8_t buffer[16] = {0};
  int16_t raw_gyro_x = 0;
  int16_t raw_gyro_y = 0;
  int16_t raw_gyro_z = 0;
  int16_t raw_acc_x = 0;
  int16_t raw_acc_y = 0;
  int16_t raw_acc_z = 0;

  if (HAL_I2C_Mem_Read(handler, LSM6DSL_READ, LSM6DSL_DATA_OUT, 1, buffer, 16, SENSOR_COM_TIMEOUT))
  {
    return HAL_ERROR;
  }

  raw_gyro_x = (buffer[5] << 8) | buffer[4];
  raw_gyro_y = (buffer[7] << 8) | buffer[6];
  raw_gyro_z = (buffer[9] << 8) | buffer[8];
  raw_acc_x = (buffer[11] << 8) | buffer[10];
  raw_acc_y = (buffer[13] << 8) | buffer[12];
  raw_acc_z = (buffer[15] << 8) | buffer[14];

  data->gyro_x = (float)raw_gyro_x * 8.75;
  data->gyro_y = (float)raw_gyro_y * 8.75;
  data->gyro_z = (float)raw_gyro_z * 8.75;
  data->acc_x = (float)raw_acc_x * 0.061;
  data->acc_y = (float)raw_acc_y * 0.061;
  data->acc_z = (float)raw_acc_z * 0.061;

  return HAL_OK;
}

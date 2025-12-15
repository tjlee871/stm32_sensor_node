/*
 * sensors.h
 *
 *  Created on: Dec 4, 2025
 *      Author: TJ
 */

#ifndef INC_SENSORS_H_
#define INC_SENSORS_H_

#include "stm32l4xx_hal.h"

typedef struct {
  float humidity;
  float temperatureC;
  float temperatureF;
} hts221_typedef;

typedef struct {
  float x;
  float y;
  float z;
} lis3mdl_typedef;

typedef struct {
  float pressure;
} lps22hb_typedef;

typedef struct {
  float acc_x;
  float acc_y;
  float acc_z;
  float gyro_x;
  float gyro_y;
  float gyro_z;
} lsm6dsl_typedef;

#define SENSOR_COM_TIMEOUT 100

#define HTS221_READ 0xBF
#define HTS221_WRITE 0xBE
#define HTS221_WHO_AM_I 0x0F
#define HTS221_AV_CONF 0x10
#define HTS221_CTRL_REG 0xA0
#define HTS221_DATA_OUT 0xA7
#define HTS221_CALIBRATION_REG 0xB0

#define LIS3MDL_READ 0x3D
#define LIS3MDL_WRITE 0x3C
#define LIS3MDL_WHO_AM_I 0x0F
#define LIS3MDL_CTRL_REG 0xA0
#define LIS3MDL_DATA_OUT 0xA7

#define LPS22HB_READ 0xBB
#define LPS22HB_WRITE 0xBA
#define LPS22HB_WHO_AM_I 0x0F
#define LPS22HB_CTRL_REG 0x10
#define LPS22HB_DATA_OUT 0x27

#define LSM6DSL_READ 0xD5
#define LSM6DSL_WRITE 0xD4
#define LSM6DSL_WHO_AM_I 0x0F
#define LSM6DSL_CTRL_REG 0x10
#define LSM6DSL_DATA_OUT 0x1E

HAL_StatusTypeDef hts221_init(I2C_HandleTypeDef *handler);
HAL_StatusTypeDef hts221_read(I2C_HandleTypeDef *handler, hts221_typedef *data);

HAL_StatusTypeDef lis3mdl_init(I2C_HandleTypeDef *handler);
HAL_StatusTypeDef lis3mdl_read(I2C_HandleTypeDef *handler, lis3mdl_typedef *data);

HAL_StatusTypeDef lps22hb_init(I2C_HandleTypeDef *handler);
HAL_StatusTypeDef lps22hb_read(I2C_HandleTypeDef *handler, lps22hb_typedef *data);

HAL_StatusTypeDef lsm6dsl_init(I2C_HandleTypeDef *handler);
HAL_StatusTypeDef lsm6dsl_read(I2C_HandleTypeDef *handler, lsm6dsl_typedef *data);

#endif /* INC_SENSORS_H_ */

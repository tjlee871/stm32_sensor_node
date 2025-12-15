# STM32 Sensor Node with FreeRTOS

## Introduction
This project enables an STM32 microcontroller to interface with four ST sensors:

- **HTS221** – Humidity and temperature sensor  
- **LIS3MDL** – Magnetometer  
- **LPS22HB** – Pressure sensor  
- **LSM6DSL** – Accelerometer and gyroscope  

**FreeRTOS** is used to handle multiple sensor data acquisition tasks along with a data output task.  
**I2C** is used for sensor communication, and **USART** is used for `printf` output.

---

## Hardware Requirements
- STM32 Discovery Kit IoT Node  
  *(alternatively, STM32 Nucleo boards)*
- HTS221
- LIS3MDL
- LPS22HB
- LSM6DSL

---

## Software Requirements
- STM32CubeIDE (or any compatible IDE)
- STM32CubeMX
- STM32Cube MCU Package
- X-CUBE-FREERTOS

---

## Instructions
1. Ensure that the four ST sensors are connected to the I2C pins of the microcontroller.
2. Use **STM32CubeMX** to:
   - Enable **I2C**
   - Enable **USART**
   - Enable **X-CUBE-FREERTOS** with **CMSIS_V2**
   - Generate a project for the target MCU
3. Import the sensor driver files (`sensors.h` and `sensors.c`) into the project.
4. Update the generated source files with the source files from this repository and resolve any conflicts.
5. Build and run the project.

---

## I2C Configuration
- **Speed Frequency:** 100 kHz  
- **Pull-up Resistors:** Internal pull-ups enabled for SCL and SDA pins

---

## USART Configuration
- **Baud Rate:** 115200 bits/s  
- **Word Length:** 8 bits  
- **Parity:** None  
- **Stop Bits:** 1

---

## FreeRTOS Configuration
Refer to `FreeRTOSConfig.h` for detailed configuration settings.

---

## References
- HTS221 Datasheet: https://www.st.com/resource/en/datasheet/hts221.pdf  
- LIS3MDL Datasheet: https://www.st.com/resource/en/datasheet/lis3mdl.pdf  
- LPS22HB Datasheet: https://www.st.com/resource/en/datasheet/lps22hb.pdf  
- LSM6DSL Datasheet: https://www.st.com/resource/en/datasheet/lsm6dsl.pdf  

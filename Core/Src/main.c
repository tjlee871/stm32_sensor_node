/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

UART_HandleTypeDef huart1;

/* Definitions for outputDataTask */
osThreadId_t outputDataTaskHandle;
const osThreadAttr_t outputDataTask_attributes = {
  .name = "outputDataTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for readHumTempTask */
osThreadId_t readHumTempTaskHandle;
const osThreadAttr_t readHumTempTask_attributes = {
  .name = "readHumTempTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readMagTask */
osThreadId_t readMagTaskHandle;
const osThreadAttr_t readMagTask_attributes = {
  .name = "readMagTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readPresTask */
osThreadId_t readPresTaskHandle;
const osThreadAttr_t readPresTask_attributes = {
  .name = "readPresTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for readAccGyroTask */
osThreadId_t readAccGyroTaskHandle;
const osThreadAttr_t readAccGyroTask_attributes = {
  .name = "readAccGyroTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for humtempQueue */
osMessageQueueId_t humtempQueueHandle;
const osMessageQueueAttr_t humtempQueue_attributes = {
  .name = "humtempQueue"
};
/* Definitions for magQueue */
osMessageQueueId_t magQueueHandle;
const osMessageQueueAttr_t magQueue_attributes = {
  .name = "magQueue"
};
/* Definitions for presQueue */
osMessageQueueId_t presQueueHandle;
const osMessageQueueAttr_t presQueue_attributes = {
  .name = "presQueue"
};
/* Definitions for accgyroQueue */
osMessageQueueId_t accgyroQueueHandle;
const osMessageQueueAttr_t accgyroQueue_attributes = {
  .name = "accgyroQueue"
};
/* Definitions for i2cMutex */
osMutexId_t i2cMutexHandle;
const osMutexAttr_t i2cMutex_attributes = {
  .name = "i2cMutex"
};
/* Definitions for uartMutex */
osMutexId_t uartMutexHandle;
const osMutexAttr_t uartMutex_attributes = {
  .name = "uartMutex"
};
/* USER CODE BEGIN PV */
osThreadId_t ledTaskHandle;
const osThreadAttr_t ledTask_attributes = {
  .name = "LedTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_USART1_UART_Init(void);
void OutputDataTask(void *argument);
void ReadHumTempTask(void *argument);
void ReadMagTask(void *argument);
void ReadPresTask(void *argument);
void ReadAccGyroTask(void *argument);

/* USER CODE BEGIN PFP */
void PeripheralReadyWait(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C2_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  PeripheralReadyWait();
  hts221_init(&hi2c2);
  lis3mdl_init(&hi2c2);
  lps22hb_init(&hi2c2);
  lsm6dsl_init(&hi2c2);
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of i2cMutex */
  i2cMutexHandle = osMutexNew(&i2cMutex_attributes);

  /* creation of uartMutex */
  uartMutexHandle = osMutexNew(&uartMutex_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of humtempQueue */
  humtempQueueHandle = osMessageQueueNew (2, sizeof(hts221_typedef), &humtempQueue_attributes);

  /* creation of magQueue */
  magQueueHandle = osMessageQueueNew (2, sizeof(lis3mdl_typedef), &magQueue_attributes);

  /* creation of presQueue */
  presQueueHandle = osMessageQueueNew (2, sizeof(lps22hb_typedef), &presQueue_attributes);

  /* creation of accgyroQueue */
  accgyroQueueHandle = osMessageQueueNew (2, sizeof(lsm6dsl_typedef), &accgyroQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of outputDataTask */
  outputDataTaskHandle = osThreadNew(OutputDataTask, NULL, &outputDataTask_attributes);

  /* creation of readHumTempTask */
  readHumTempTaskHandle = osThreadNew(ReadHumTempTask, NULL, &readHumTempTask_attributes);

  /* creation of readMagTask */
  readMagTaskHandle = osThreadNew(ReadMagTask, NULL, &readMagTask_attributes);

  /* creation of readPresTask */
  readPresTaskHandle = osThreadNew(ReadPresTask, NULL, &readPresTask_attributes);

  /* creation of readAccGyroTask */
  readAccGyroTaskHandle = osThreadNew(ReadAccGyroTask, NULL, &readAccGyroTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */

  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.Timing = 0x10D19CE4;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c2, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c2, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED2_GPIO_Port, LED2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : LED2_Pin */
  GPIO_InitStruct.Pin = LED2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED2_GPIO_Port, &GPIO_InitStruct);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
PUTCHAR_PROTOTYPE
{
  HAL_UART_Transmit(&huart1, (uint8_t *)&ch, 1, 0xFFFF);
  return ch;
}

void PeripheralReadyWait(void)
{
  while (HAL_I2C_GetState(&hi2c2) != HAL_I2C_STATE_READY) {}
  while (HAL_UART_GetState(&huart1) != HAL_UART_STATE_READY) {}
  printf("\r\nPeripherals ready\r\n");
}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_OutputDataTask */
/**
  * @brief  Function implementing the outputDataTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_OutputDataTask */
void OutputDataTask(void *argument)
{
  /* USER CODE BEGIN 5 */
  hts221_typedef hts221_data = {0};
  lis3mdl_typedef lis3mdl_data = {0};
  lps22hb_typedef lps22hb_data = {0};
  lsm6dsl_typedef lsm6dsl_data = {0};
  /* Infinite loop */
  for(;;)
  {
    HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);
    if (osMutexAcquire(uartMutexHandle, osWaitForever) == osOK)
    {
      printf("\r\n=== Sensor Data ===\r\n");
      if (osMessageQueueGet(humtempQueueHandle, &hts221_data, 0, 0) == osOK)
      {
	printf("Humidity:       %7.1frH\r\n", hts221_data.humidity);
	printf("temperature C:  %7.1fC\r\n", hts221_data.temperatureC);
	printf("temperature F:  %7.1fF\r\n", hts221_data.temperatureF);
      }
      if (osMessageQueueGet(magQueueHandle, &lis3mdl_data, 0, 0) == osOK)
      {
	printf("Mag Field X:    %7.1fmG\r\n", lis3mdl_data.x);
	printf("Mag Field Y:    %7.1fmG\r\n", lis3mdl_data.y);
	printf("Mag Field Z:    %7.1fmG\r\n", lis3mdl_data.z);
      }
      if (osMessageQueueGet(presQueueHandle, &lps22hb_data, 0, 0) == osOK)
      {
	printf("Pressure:       %7.1fhPa\r\n", lps22hb_data.pressure);
      }
      if (osMessageQueueGet(accgyroQueueHandle, &lsm6dsl_data, 0, 0) == osOK)
      {
	printf("Acceleration X: %7.1fmg\r\n", lsm6dsl_data.acc_x);
	printf("Acceleration Y: %7.1fmg\r\n", lsm6dsl_data.acc_y);
	printf("Acceleration Z: %7.1fmg\r\n", lsm6dsl_data.acc_z);
	printf("Angular rate X: %7.1fmdps\r\n", lsm6dsl_data.gyro_x);
	printf("Angular rate Y: %7.1fmdps\r\n", lsm6dsl_data.gyro_y);
	printf("Angular rate Z: %7.1fmdps\r\n", lsm6dsl_data.gyro_z);
      }
      osMutexRelease(uartMutexHandle);
    }
    osDelay(500);
  }
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_ReadHumTempTask */
/**
* @brief Function implementing the readHumTempTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ReadHumTempTask */
void ReadHumTempTask(void *argument)
{
  /* USER CODE BEGIN ReadHumTempTask */
  hts221_typedef hts221_data = {0};
  /* Infinite loop */
  for(;;)
  {
    if (osMutexAcquire(i2cMutexHandle, osWaitForever) == osOK)
    {
      hts221_read(&hi2c2, &hts221_data);
      osMutexRelease(i2cMutexHandle);
      osMessageQueuePut(humtempQueueHandle, &hts221_data, 0, 0);
    }
    osDelay(100);
  }
  /* USER CODE END ReadHumTempTask */
}

/* USER CODE BEGIN Header_ReadMagTask */
/**
* @brief Function implementing the readMagTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ReadMagTask */
void ReadMagTask(void *argument)
{
  /* USER CODE BEGIN ReadMagTask */
  lis3mdl_typedef lis3mdl_data = {0};
  /* Infinite loop */
  for(;;)
  {
    if (osMutexAcquire(i2cMutexHandle, osWaitForever) == osOK)
    {
      lis3mdl_read(&hi2c2, &lis3mdl_data);
      osMutexRelease(i2cMutexHandle);
      osMessageQueuePut(magQueueHandle, &lis3mdl_data, 0, 0);
    }
    osDelay(100);
  }
  /* USER CODE END ReadMagTask */
}

/* USER CODE BEGIN Header_ReadPresTask */
/**
* @brief Function implementing the readPresTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ReadPresTask */
void ReadPresTask(void *argument)
{
  /* USER CODE BEGIN ReadPresTask */
  lps22hb_typedef lps22hb_data = {0};
  /* Infinite loop */
  for(;;)
  {
    if (osMutexAcquire(i2cMutexHandle, osWaitForever) == osOK)
    {
      lps22hb_read(&hi2c2, &lps22hb_data);
      osMutexRelease(i2cMutexHandle);
      osMessageQueuePut(presQueueHandle, &lps22hb_data, 0, 0);
    }
    osDelay(100);
  }
  /* USER CODE END ReadPresTask */
}

/* USER CODE BEGIN Header_ReadAccGyroTask */
/**
* @brief Function implementing the readAccGyroTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_ReadAccGyroTask */
void ReadAccGyroTask(void *argument)
{
  /* USER CODE BEGIN ReadAccGyroTask */
  lsm6dsl_typedef lsm6dsl_data = {0};
  /* Infinite loop */
  for(;;)
  {
    if (osMutexAcquire(i2cMutexHandle, osWaitForever) == osOK)
    {
      lsm6dsl_read(&hi2c2, &lsm6dsl_data);
      osMutexRelease(i2cMutexHandle);
      osMessageQueuePut(accgyroQueueHandle, &lsm6dsl_data, 0, 0);
    }
    osDelay(100);
  }
  /* USER CODE END ReadAccGyroTask */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

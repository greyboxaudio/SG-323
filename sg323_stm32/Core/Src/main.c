/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2024 STMicroelectronics.
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

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "sg323.h"
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
UART_HandleTypeDef huart2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */
inline void calculateAddressRow(unsigned int result)
{
  __uint8_t bit6 = result << 1;
  bit6 = bit6 >> 7;
  __uint8_t MSB = result;
  MSB = MSB >> 7;
  delayCarryOut = result >> 8;
  rowDelay = result << 2;
  rowDelay = (rowDelay >> 1) | bit6 | (MSB << 7);
}
inline void calculateAddressColumn(unsigned int result, unsigned int rowDelay)
{
  __uint8_t colDelay = result << 2;
  colDelay = colDelay >> 2;
  delayAddress = (rowDelay) + (colDelay * 256);
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
__uint16_t dram[16384];
__uint8_t preDelay = 3;  // predelay from low to high (3,2,0,1,5,4,6,7,11,10,8,9,13,12,14,15)
__uint8_t program = 7;   // program from low to high (3,2,0,1,5,4,6,7)
__uint8_t decayTime = 7; // decay times from low to high (3,2,0,1,5,4,6,7)
__uint8_t rateLvl = 0;   // 0,1,3,7,15 RNG for testing
__uint16_t inputSample = 0;
__uint16_t feedbackSample = 0;
__uint16_t outputSampleLeft = 0;
__uint16_t outputSampleRight = 0;
float gainValue = 0;
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
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	HAL_GPIO_TogglePin(STATUS_GPIO_Port, STATUS_Pin);
    // calculate base address factors
    gainBaseAddr = (decayTime << 5) | (program << 8);
    preDelay_high = preDelay >> 3;
    preDelay_low = preDelay << 5;
    preDelay_low = preDelay_low >> 5;
    delayBaseAddr = (preDelay_low << 6) | (program << 9) | (preDelay_high << 12);
    // calculate write address
    // calculate address row
    result = nROW;
    calculateAddressRow(result);
    // calculate address column
    result = nCOLUMN + delayCarryOut;
    calculateAddressColumn(result, rowDelay);
    // store sample
    dram[delayAddress] = inputSample;
    // set up variables for feedback taps
    dly_mod_addr = delayModBaseAddr + 7;
    dly_addr = delayBaseAddr + 16;
    gainModContAddress = gainModContBaseAddr + 8;
    gainAddress = gainBaseAddr + 8;
    // calculate feedback taps
    for (int d = 0; d < 15; d++)
    {
      // calculate address to read from memory
      result = delayModData[dly_mod_addr + d] + nROW;
      calculateAddressRow(result);
      result = delayData[dly_addr + (d * 2)] + nCOLUMN + delayCarryOut;
      calculateAddressColumn(result, rowDelay);
      // calculate gain value
      gainModContOut = gainModCtrlData[gainModContAddress + d];
      nGainModEnable = gainModContOut >> 3;
      gainModContOut = gainModContOut << 5;
      gainModContOut = gainModContOut >> 5;
      gainModAddress = gainModContOut | gainModBaseAddr;
      gainModOut = gainModData[gainModAddress];
      gainOut = gainData[gainAddress + d];
      nGSN = gainOut >> 7;
      gainOut = gainOut << 1;
      if (gainModOut < gainOut && nGainModEnable == 0)
      {
        gainOut = gainModOut;
      }
      gainValue = gainOut * 0.00390625;
      // calculate sign
      if (nGSN == 0)
      {
        gainValue = gainValue * -1;
      }
      // read feedbackSample
      feedbackSample = (int)(dram[delayAddress] * gainValue);
    }
    // set up variables for output taps
    gainAddress = gainBaseAddr + 23;
    dly_mod_addr = delayBaseAddr + 45;
    dly_addr = delayBaseAddr + 46;
    // calculate left output taps
    for (int d = 0; d < 4; d++)
    {
      // calculate address to read from memory
      result = delayData[dly_mod_addr + (d * 2)] + nROW;
      calculateAddressRow(result);
      result = delayData[dly_addr + (d * 2)] + nCOLUMN + delayCarryOut;
      calculateAddressColumn(result, rowDelay);
      // calculate gain value
      gainOut = gainData[gainAddress + d];
      nGSN = gainOut >> 7;
      gainOut = gainOut << 1;
      gainValue = (float)(gainOut * 0.00390625);
      // calculate sign
      if (nGSN == 0)
      {
        gainValue = gainValue * -1;
      }
      // read left output sample
      outputSampleLeft = (int)(dram[delayAddress] * gainValue);
    }
    // calculate right output taps
    for (int d = 4; d < 8; d++)
    {
      // calculate address to read from memory
      result = delayData[dly_mod_addr + (d * 2)] + nROW;
      calculateAddressRow(result);
      result = delayData[dly_addr + (d * 2)] + nCOLUMN + delayCarryOut;
      calculateAddressColumn(result, rowDelay);
      // calculate gain value
      gainOut = gainData[gainAddress + d];
      nGSN = gainOut >> 7;
      gainOut = gainOut << 1;
      gainValue = (float)(gainOut * 0.00390625);
      // calculate sign
      if (nGSN == 0)
      {
        gainValue = gainValue * -1;
      }
      // read right output sample
      outputSampleRight = (int)(dram[delayAddress] * gainValue);
    }
    // mod rate counter
    modClockOut = modClockOut + 1;
    if (modClockOut == 16)
    {
      modRateCount = rateLvl | (program << 4);
      modClockOut = modRateCtrlData[modRateCount];
    }
    modCarry = (modClockOut + 1) >> 4;
    // advance write address & wraparound if < 0
    writeAddressCount = writeAddressCount - 1;
    if (writeAddressCount < 0)
    {
      writeAddressCount = 16383;
    }
    nROW = writeAddressCount;
    nCOLUMN = writeAddressCount >> 8;
    MCCK = modCarry;
    if (MCCK == 1)
    {
      modCount = modCount + 1;
      if (modCount > 8191)
      {
        modCount = 0;
      }

      gainModContBaseAddr = (modCount >> 6) << 5;
      gainModBaseAddr = modCount << 7;
      gainModBaseAddr = gainModBaseAddr >> 4;
      delayModCount = modCount >> 6;
      delayModBaseAddr = delayModCount << 5;
    }
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 16;
  RCC_OscInitStruct.PLL.PLLN = 360;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

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
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(STATUS_GPIO_Port, STATUS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : STATUS_Pin */
  GPIO_InitStruct.Pin = STATUS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  HAL_GPIO_Init(STATUS_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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

#ifdef  USE_FULL_ASSERT
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

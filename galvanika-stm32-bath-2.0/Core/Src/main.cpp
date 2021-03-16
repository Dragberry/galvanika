/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include <main.hpp>
#include <stdio.h>
#include <string.h>
#include <bath.hpp>
#include <flash.hpp>
#include <baths.hpp>
#include <displays/display.hpp>
#include <displays/graphic_display.hpp>
#include <displays/jlx12864_display_interface.hpp>
#include <displays/gmg12864_display_interface.hpp>
#include <displays/ssd1306_display_interface.hpp>
#include <displays/SansSerif11.h>

#define SCL_PIN     GPIO_PIN_6
#define SCL_PORT    GPIOB

#define SDA_PIN     GPIO_PIN_7
#define SDA_PORT    GPIOB


//void HAL_GPIO_WRITE_ODR(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
//{
//  /* Check the parameters */
//  assert_param(IS_GPIO_PIN(GPIO_Pin));
//
//  GPIOx->ODR |= GPIO_Pin;
//} // Call initialization function.
//
//void HAL_I2C_ClearBusyFlagErrata_2_14_7(I2C_HandleTypeDef *hi2c) {
//
//    static uint8_t resetTried = 0;
//    if (resetTried == 1) {
//        return ;
//    }
//    GPIO_InitTypeDef GPIO_InitStruct;
//
//    // 1
//    __HAL_I2C_DISABLE(hi2c);
//
//    // 2
//    GPIO_InitStruct.Pin = SDA_PIN|SCL_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_WRITE_ODR(GPIOB, SDA_PIN);
//    HAL_GPIO_WRITE_ODR(GPIOB, SCL_PIN);
//
//    // 3
//    GPIO_PinState pinState;
//    if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_RESET) {
//        for(;;){}
//    }
//    if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_RESET) {
//        for(;;){}
//    }
//
//    // 4
//    GPIO_InitStruct.Pin = SDA_PIN;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_TogglePin(GPIOB, SDA_PIN);
//
//    // 5
//    if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_SET) {
//        for(;;){}
//    }
//
//    // 6
//    GPIO_InitStruct.Pin = SCL_PIN;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_TogglePin(GPIOB, SCL_PIN);
//
//    // 7
//    if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_SET) {
//        for(;;){}
//    }
//
//    // 8
//    GPIO_InitStruct.Pin = SDA_PIN;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_WRITE_ODR(GPIOB, SDA_PIN);
//
//    // 9
//    if (HAL_GPIO_ReadPin(GPIOB, SDA_PIN) == GPIO_PIN_RESET) {
//        for(;;){}
//    }
//
//    // 10
//    GPIO_InitStruct.Pin = SCL_PIN;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//    HAL_GPIO_WRITE_ODR(GPIOB, SCL_PIN);
//
//    // 11
//    if (HAL_GPIO_ReadPin(GPIOB, SCL_PIN) == GPIO_PIN_RESET) {
//        for(;;){}
//    }
//
//    // 12
//    GPIO_InitStruct.Pin = SDA_PIN|SCL_PIN;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
//    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
//
//   // 13
//   hi2c->Instance->CR1 |= I2C_CR1_SWRST;
//
//   // 14
//   hi2c->Instance->CR1 ^= I2C_CR1_SWRST;
//
//   // 15
//   __HAL_I2C_ENABLE(hi2c);
//
//   resetTried = 1;
//}

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
ADC_HandleTypeDef hadc1;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim4;

UART_HandleTypeDef huart1;

//class STM32_I2C : public I2C {
//public:
//  void send(uint8_t address, uint8_t *data, uint8_t length) {
//    while (true) {
//      HAL_StatusTypeDef status = HAL_I2C_Master_Transmit(&hi2c1, (uint16_t) (address << 1), data, length, HAL_MAX_DELAY);
//      if (status == HAL_BUSY) {
//        HAL_I2C_ClearBusyFlagErrata_2_14_7(&hi2c1);
//        continue;
//      }
//      break;
//    }
//  }
//} i2c;
//
//SSD1306DisplayInterface ssd1306 = SSD1306DisplayInterface(&i2c, 0x3C);

/* USER CODE BEGIN PV */

JLX12864DisplayInterface<__IO uint32_t, uint16_t> jxl12864 = JLX12864DisplayInterface<__IO uint32_t, uint16_t>(
    {&GPIOB->BSRR, GPIO_PIN_5},
    {&GPIOB->BSRR, GPIO_PIN_6},
    {&GPIOB->BSRR, GPIO_PIN_7},
    {&GPIOB->BSRR, GPIO_PIN_8},
    {&GPIOB->BSRR, GPIO_PIN_9}
);
GraphicDisplay<128, 64> display = GraphicDisplay<128, 64>(&jxl12864);

//STM32::Flash<Data> storage = STM32::Flash<Data>(0x800FC00, 1);

Bath baths[3] = {
    Bath(
        'C', 2,
        {ADC_CHANNEL_4, ADC_CHANNEL_8, ADC_CHANNEL_7},
        {&GPIOB->ODR, GPIO_PIN_12}, // PWM
        {&GPIOB->IDR, GPIO_PIN_15}, // BRANCH
        {&GPIOA->IDR, GPIO_PIN_15}, // MODE
        {&GPIOB->IDR, GPIO_PIN_11}, // POWER
        0x800F400,
        [](float value) -> float { return (value - 0.023) * 0.9615; },
        [](float value) -> float { return value + 0.013; }
    ),
    Bath(
        'B', 4,
        {ADC_CHANNEL_1, ADC_CHANNEL_5, ADC_CHANNEL_6},
        {&GPIOB->ODR, GPIO_PIN_14},
        {&GPIOA->IDR, GPIO_PIN_11},
        {&GPIOB->IDR, GPIO_PIN_3},
        {&GPIOB->IDR, GPIO_PIN_10},
        0x800F800,
        [](float value) -> float { return value - (0.01 + (value / 15)); },
        [](float value) -> float {
          float a = 0;
          if (value < 0.12) {
            a = 0.1416 * (0.12 - value);
          }
          return value + a;
        }
    ),
    Bath(
        'A', 6,
        {ADC_CHANNEL_0, ADC_CHANNEL_3, ADC_CHANNEL_2},
        {&GPIOB->ODR, GPIO_PIN_13},
        {&GPIOA->IDR, GPIO_PIN_12},
        {&GPIOB->IDR, GPIO_PIN_4},
        {&GPIOB->IDR, GPIO_PIN_1},
        0x800FC00,
        [](float value) -> float {
          float a = 0.66;
          if (value < 0.055) {
            a = a - (0.055 - value) * 10;
          }
          return value * a;
        },
        [](float value) -> float {
          float a = 0;
          if (value < 0.33) {
            a = 0.09 * (0.33 - value);
          }
          return value + a;
        }
    )
};
Baths<3> bs = Baths<3>(baths);


volatile bool refresh_display = true;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_TIM1_Init(void);
static void MX_ADC1_Init(void);
static void MX_TIM4_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim->Instance == TIM4) {
    bs.step_up();
  }
  if (htim->Instance == TIM1) { //check if the interrupt comes from TIM1
    refresh_display = true;
  }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *hadc) {
  if (hadc->Instance == ADC1) {
    bs.apply_adc(HAL_ADC_GetValue(&hadc1));

    ADC_ChannelConfTypeDef sConfig;
    sConfig.Channel = bs.get_adc_channel();
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
      Error_Handler();
    }

    HAL_ADC_Start_IT(&hadc1);
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  bs.change_button_state(GPIO_Pin);
}
/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
  bs.iterate([&](Bath& bath) -> void {
    bath.load();
  });


  /* USER CODE BEGIN SysInit */
//  __HAL_RCC_I2C1_CLK_ENABLE();
//  HAL_Delay(100);
//  __HAL_RCC_I2C1_FORCE_RESET();
//  HAL_Delay(100);
//  __HAL_RCC_I2C1_RELEASE_RESET();
//  HAL_Delay(100);

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
//  MX_USART1_UART_Init();
//  MX_I2C1_Init();
  MX_ADC1_Init();
  MX_TIM1_Init();
  MX_TIM4_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_ADCEx_Calibration_Start(&hadc1);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  display.init([]() -> void {
    // initialize your port to output
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9|GPIO_PIN_8|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5, GPIO_PIN_RESET);
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_8|GPIO_PIN_7|GPIO_PIN_6|GPIO_PIN_5;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  });

  HAL_ADC_Start_IT(&hadc1);

  uint8_t top = 0;
  uint8_t bottom = 63;
  display.vram_fill(false);
  display.vram_put_str({8,  16}, &microsoftSansSerif_11ptFontInfo, "CUPRUMBERRY", PointMode::PIXEL_XOR);
  display.vram_put_str({8,  32}, &microsoftSansSerif_11ptFontInfo, "GALVANIKA V2.2", PointMode::PIXEL_XOR);
  display.vram_update();
  while (top < 64) {
    display.vram_put_line({0,  top}, {127,  top}, PointMode::PIXEL_XOR);
    display.vram_put_line({0,  bottom}, {127,  bottom}, PointMode::PIXEL_XOR);
    display.vram_update();
    top++;
    bottom--;
    HAL_Delay(50);
  }

  display.clear();
  display.set_cursor(0, 0);
  display.put_string("CuprumBerry V2.2");
  while (true) {
    /* USER CODE END WHILE */
    if (refresh_display) {
      refresh_display = false;
      bs.iterate([&](Bath& bath) -> void {
        bath.build();
        bath.show(display);
      });
    }
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
  RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
  RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
  RCC_PeriphCLKInitTypeDef PeriphClkInit = { 0 };

  /** Initializes the RCC Oscillators according to the specified parameters
   * in the RCC_OscInitTypeDef structure.
   */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
   */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
      | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
  PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV8;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
    Error_Handler();
  }

}


/**
 * @brief ADC1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_ADC1_Init(void) {

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = { 0 };

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */
  /** Common config
   */
  hadc1.Instance = ADC1;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.ContinuousConvMode = ENABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  if (HAL_ADC_Init(&hadc1) != HAL_OK) {
    Error_Handler();
  }
  /** Configure Regular Channel
   */
  sConfig.Channel = ADC_CHANNEL_0;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */
  /* USER CODE END ADC1_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

  /* USER CODE BEGIN TIM1_Init 0 */

  /* USER CODE END TIM1_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
  TIM_MasterConfigTypeDef sMasterConfig = { 0 };

  /* USER CODE BEGIN TIM1_Init 1 */

  /* USER CODE END TIM1_Init 1 */
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 7199;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 2131;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM1_Init 2 */

  /* USER CODE END TIM1_Init 2 */

}

/**
 * @brief TIM4 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM4_Init(void) {

  /* USER CODE BEGIN TIM4_Init 0 */

  /* USER CODE END TIM4_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
  TIM_MasterConfigTypeDef sMasterConfig = { 0 };

  /* USER CODE BEGIN TIM4_Init 1 */

  /* USER CODE END TIM4_Init 1 */
  htim4.Instance = TIM4;
  htim4.Init.Prescaler = 7199;
  htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim4.Init.Period = 9;
  htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim4.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim4) != HAL_OK) {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim4, &sClockSourceConfig) != HAL_OK) {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim4, &sMasterConfig) != HAL_OK) {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM4_Init 2 */

  /* USER CODE END TIM4_Init 2 */

}

/**
 * @brief USART1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_USART1_UART_Init(void) {

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
  if (HAL_UART_Init(&huart1) != HAL_OK) {
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
static void MX_GPIO_Init(void) {
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9, GPIO_PIN_RESET);

  /*Configure GPIO pins : PB1 PB10 PB11 PB15 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_10|GPIO_PIN_11|GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PB12 PB13 PB14 PB5
                           PB6 PB7 PB8 PB9 */
  GPIO_InitStruct.Pin = GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_5
                          |GPIO_PIN_6|GPIO_PIN_7|GPIO_PIN_8|GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : PA11 PA12 */
  GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA15 */
  GPIO_InitStruct.Pin = GPIO_PIN_15;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : PB3 PB4 */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);

  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1) {
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

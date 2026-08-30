/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define ENCO_0A_Pin GPIO_PIN_2
#define ENCO_0A_GPIO_Port GPIOE
#define ENCO_0A_EXTI_IRQn EXTI2_IRQn
#define ENCO_1A_Pin GPIO_PIN_3
#define ENCO_1A_GPIO_Port GPIOE
#define ENCO_1A_EXTI_IRQn EXTI3_IRQn
#define ENCO_2A_Pin GPIO_PIN_4
#define ENCO_2A_GPIO_Port GPIOE
#define ENCO_2A_EXTI_IRQn EXTI4_IRQn
#define ENCO_3A_Pin GPIO_PIN_5
#define ENCO_3A_GPIO_Port GPIOE
#define ENCO_3A_EXTI_IRQn EXTI9_5_IRQn
#define ENCO_0B_Pin GPIO_PIN_6
#define ENCO_0B_GPIO_Port GPIOE
#define SDMMC2_DETECT_Pin GPIO_PIN_13
#define SDMMC2_DETECT_GPIO_Port GPIOC
#define ENCO_6B_Pin GPIO_PIN_14
#define ENCO_6B_GPIO_Port GPIOC
#define ENCO_7B_Pin GPIO_PIN_15
#define ENCO_7B_GPIO_Port GPIOC
#define ENCO_6A_Pin GPIO_PIN_8
#define ENCO_6A_GPIO_Port GPIOF
#define ENCO_6A_EXTI_IRQn EXTI9_5_IRQn
#define ENCO_7A_Pin GPIO_PIN_9
#define ENCO_7A_GPIO_Port GPIOF
#define ENCO_7A_EXTI_IRQn EXTI9_5_IRQn
#define BUTTON_Pin GPIO_PIN_10
#define BUTTON_GPIO_Port GPIOF
#define BUTTON_EXTI_IRQn EXTI15_10_IRQn
#define LED0_Pin GPIO_PIN_1
#define LED0_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_2
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_3
#define LED2_GPIO_Port GPIOC
#define MX_A_INT_Pin GPIO_PIN_0
#define MX_A_INT_GPIO_Port GPIOA
#define MX_A_INT_EXTI_IRQn EXTI0_IRQn
#define MX_B_INT_Pin GPIO_PIN_1
#define MX_B_INT_GPIO_Port GPIOA
#define MX_B_INT_EXTI_IRQn EXTI1_IRQn
#define ENCO_1B_Pin GPIO_PIN_5
#define ENCO_1B_GPIO_Port GPIOA
#define ENCO_2B_Pin GPIO_PIN_6
#define ENCO_2B_GPIO_Port GPIOA
#define ENCO_3B_Pin GPIO_PIN_7
#define ENCO_3B_GPIO_Port GPIOA
#define LCD_D0_Pin GPIO_PIN_0
#define LCD_D0_GPIO_Port GPIOB
#define LCD_D1_Pin GPIO_PIN_1
#define LCD_D1_GPIO_Port GPIOB
#define LCD_D2_Pin GPIO_PIN_2
#define LCD_D2_GPIO_Port GPIOB
#define LCD_D10_Pin GPIO_PIN_10
#define LCD_D10_GPIO_Port GPIOB
#define LCD_D11_Pin GPIO_PIN_11
#define LCD_D11_GPIO_Port GPIOB
#define LCD_D12_Pin GPIO_PIN_12
#define LCD_D12_GPIO_Port GPIOB
#define LCD_D13_Pin GPIO_PIN_13
#define LCD_D13_GPIO_Port GPIOB
#define LCD_D14_Pin GPIO_PIN_14
#define LCD_D14_GPIO_Port GPIOB
#define LCD_D15_Pin GPIO_PIN_15
#define LCD_D15_GPIO_Port GPIOB
#define MX_X_INT_Pin GPIO_PIN_11
#define MX_X_INT_GPIO_Port GPIOD
#define MX_X_INT_EXTI_IRQn EXTI15_10_IRQn
#define ENCO_4B_Pin GPIO_PIN_2
#define ENCO_4B_GPIO_Port GPIOG
#define ENCO_5B_Pin GPIO_PIN_3
#define ENCO_5B_GPIO_Port GPIOG
#define ENCO_4A_Pin GPIO_PIN_6
#define ENCO_4A_GPIO_Port GPIOG
#define ENCO_4A_EXTI_IRQn EXTI9_5_IRQn
#define ENCO_5A_Pin GPIO_PIN_7
#define ENCO_5A_GPIO_Port GPIOG
#define ENCO_5A_EXTI_IRQn EXTI9_5_IRQn
#define LCD_CS_Pin GPIO_PIN_2
#define LCD_CS_GPIO_Port GPIOD
#define LCD_RD_Pin GPIO_PIN_3
#define LCD_RD_GPIO_Port GPIOD
#define LCD_WR_Pin GPIO_PIN_4
#define LCD_WR_GPIO_Port GPIOD
#define LCD_RS_Pin GPIO_PIN_5
#define LCD_RS_GPIO_Port GPIOD
#define LCD_RESET_Pin GPIO_PIN_13
#define LCD_RESET_GPIO_Port GPIOG
#define LCD_BL_Pin GPIO_PIN_14
#define LCD_BL_GPIO_Port GPIOG
#define LCD_D3_Pin GPIO_PIN_3
#define LCD_D3_GPIO_Port GPIOB
#define LCD_D4_Pin GPIO_PIN_4
#define LCD_D4_GPIO_Port GPIOB
#define LCD_D5_Pin GPIO_PIN_5
#define LCD_D5_GPIO_Port GPIOB
#define LCD_D6_Pin GPIO_PIN_6
#define LCD_D6_GPIO_Port GPIOB
#define LCD_D7_Pin GPIO_PIN_7
#define LCD_D7_GPIO_Port GPIOB
#define LCD_D8_Pin GPIO_PIN_8
#define LCD_D8_GPIO_Port GPIOB
#define LCD_D9_Pin GPIO_PIN_9
#define LCD_D9_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

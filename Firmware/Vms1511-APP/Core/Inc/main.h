/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "stm32f0xx_hal.h"

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
#define MCU_ON_Pin GPIO_PIN_0
#define MCU_ON_GPIO_Port GPIOA
#define RS485_D_Pin GPIO_PIN_2
#define RS485_D_GPIO_Port GPIOA
#define RS485_R_Pin GPIO_PIN_3
#define RS485_R_GPIO_Port GPIOA
#define RS485_DE_Pin GPIO_PIN_4
#define RS485_DE_GPIO_Port GPIOA
#define RS485_RE_Pin GPIO_PIN_5
#define RS485_RE_GPIO_Port GPIOA
#define I2C_NRST_Pin GPIO_PIN_12
#define I2C_NRST_GPIO_Port GPIOB
#define RS485_ADDRESS0_Pin GPIO_PIN_3
#define RS485_ADDRESS0_GPIO_Port GPIOB
#define RS485_ADDRESS1_Pin GPIO_PIN_4
#define RS485_ADDRESS1_GPIO_Port GPIOB
#define RS485_ADDRESS2_Pin GPIO_PIN_5
#define RS485_ADDRESS2_GPIO_Port GPIOB
#define RS485_ADDRESS3_Pin GPIO_PIN_6
#define RS485_ADDRESS3_GPIO_Port GPIOB
#define RS485_ADDRESS4_Pin GPIO_PIN_7
#define RS485_ADDRESS4_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

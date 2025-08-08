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
#include "stm32f1xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


#include "software_Time.h"
#include "user_function.h"
#include "user_init.h"
#include "uart.h"
#include "user_log.h"
#include "stdio.h"
#include "control.h"
#include "pelco_d_protocol.h"

extern SPI_HandleTypeDef hspi2;

extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim3;
extern TIM_HandleTypeDef htim4;

extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;



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
#define VD12_Pin GPIO_PIN_0
#define VD12_GPIO_Port GPIOA
#define VD34_Pin GPIO_PIN_1
#define VD34_GPIO_Port GPIOA
#define VD56_Pin GPIO_PIN_2
#define VD56_GPIO_Port GPIOA
#define VD78_Pin GPIO_PIN_3
#define VD78_GPIO_Port GPIOA
#define VD9A_Pin GPIO_PIN_6
#define VD9A_GPIO_Port GPIOA
#define SPI_CS_Pin GPIO_PIN_12
#define SPI_CS_GPIO_Port GPIOB
#define MS_RSTB_Pin GPIO_PIN_8
#define MS_RSTB_GPIO_Port GPIOA
#define ZOOM3_PI_CO_Pin GPIO_PIN_12
#define ZOOM3_PI_CO_GPIO_Port GPIOA
#define ZOOM1_PI_CO_Pin GPIO_PIN_15
#define ZOOM1_PI_CO_GPIO_Port GPIOA
#define FOCUS_PI_CO_Pin GPIO_PIN_3
#define FOCUS_PI_CO_GPIO_Port GPIOB
#define ZOOM2_PI_CO_Pin GPIO_PIN_4
#define ZOOM2_PI_CO_GPIO_Port GPIOB
#define IRIS_PI_CO_Pin GPIO_PIN_5
#define IRIS_PI_CO_GPIO_Port GPIOB
#define MS_PLS2_Pin GPIO_PIN_6
#define MS_PLS2_GPIO_Port GPIOB
#define MS_PLS1_Pin GPIO_PIN_7
#define MS_PLS1_GPIO_Port GPIOB
#define IRCUT_2_Pin GPIO_PIN_8
#define IRCUT_2_GPIO_Port GPIOB
#define IRCUT_1_Pin GPIO_PIN_9
#define IRCUT_1_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

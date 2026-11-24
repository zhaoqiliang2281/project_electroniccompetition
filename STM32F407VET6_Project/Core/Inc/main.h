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
/*==============================================================================
 * main.h —— CubeMX 引脚/外设宏定义 + 公共 include
 * 下方一批 Sensors / NRF24L01 / Key1 宏是早期“小车+无线”方案的遗留定义，
 * 本工程（云台追踪）并不使用；真正在用的是：
 *   OLED_CLK(PB8) / OLED_SDA(PB9) / Buzzer(PA8) / Key_Stats(PD12) / LED0~3(PC0~3)
 *============================================================================*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */


/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

//串口接收与发送缓冲区
extern uint8_t usart1_rece[128];
extern char usart1_send[128];
extern uint8_t usart2_rece[128];
extern char usart2_send[128];
extern uint8_t usart3_rece[128];
extern char usart3_send[128];
extern uint8_t usart4_rece[128];
extern char usart4_send[128];
extern uint8_t usart6_rece[128];
//extern char usart6_send[128];
extern uint8_t g_usart6_receivedata;

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

extern uint8_t KeyNum;

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define Sensors9_Pin GPIO_PIN_2
#define Sensors9_GPIO_Port GPIOE
#define Sensors12_Pin GPIO_PIN_5
#define Sensors12_GPIO_Port GPIOE
#define Sensors10_Pin GPIO_PIN_13
#define Sensors10_GPIO_Port GPIOC
#define LED0_Pin GPIO_PIN_0
#define LED0_GPIO_Port GPIOC
#define LED1_Pin GPIO_PIN_1
#define LED1_GPIO_Port GPIOC
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOC
#define LED3_Pin GPIO_PIN_3
#define LED3_GPIO_Port GPIOC
#define NRF24L01_CSN_Pin GPIO_PIN_0
#define NRF24L01_CSN_GPIO_Port GPIOB
#define NRF24L01_MOSI_Pin GPIO_PIN_1
#define NRF24L01_MOSI_GPIO_Port GPIOB
#define NRF24L01_IRQ_Pin GPIO_PIN_2
#define NRF24L01_IRQ_GPIO_Port GPIOB
#define NRF24L01_CE_Pin GPIO_PIN_7
#define NRF24L01_CE_GPIO_Port GPIOE
#define NRF24L01_SCK_Pin GPIO_PIN_8
#define NRF24L01_SCK_GPIO_Port GPIOE
#define NRF24L01_MISO_Pin GPIO_PIN_10
#define NRF24L01_MISO_GPIO_Port GPIOE
#define Key_Stats_Pin GPIO_PIN_12
#define Key_Stats_GPIO_Port GPIOD
#define Buzzer_Pin GPIO_PIN_8
#define Buzzer_GPIO_Port GPIOA
#define Sensors1_Pin GPIO_PIN_15
#define Sensors1_GPIO_Port GPIOA
#define Sensors2_Pin GPIO_PIN_12
#define Sensors2_GPIO_Port GPIOC
#define Sensors4_Pin GPIO_PIN_0
#define Sensors4_GPIO_Port GPIOD
#define Key1_Pin GPIO_PIN_1
#define Key1_GPIO_Port GPIOD
#define Sensors5_Pin GPIO_PIN_2
#define Sensors5_GPIO_Port GPIOD
#define Sensors3_Pin GPIO_PIN_3
#define Sensors3_GPIO_Port GPIOD
#define Sensors6_Pin GPIO_PIN_4
#define Sensors6_GPIO_Port GPIOD
#define Sensors7_Pin GPIO_PIN_3
#define Sensors7_GPIO_Port GPIOB
#define Sensors8_Pin GPIO_PIN_5
#define Sensors8_GPIO_Port GPIOB
#define OLED_CLK_Pin GPIO_PIN_8
#define OLED_CLK_GPIO_Port GPIOB
#define OLED_SDA_Pin GPIO_PIN_9
#define OLED_SDA_GPIO_Port GPIOB
#define Sensors11_Pin GPIO_PIN_0
#define Sensors11_GPIO_Port GPIOE

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

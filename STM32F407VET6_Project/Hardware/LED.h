/**
 ******************************************************************************
 * @file    LED.h
 * @brief   LED / 激光接口与闪烁模式变量
 * @note    LEDx_Mode 由业务代码设置（0~5），LED_Tick 负责按模式驱动引脚。
 ******************************************************************************
 */

#ifndef __LED_H
#define __LED_H

#include "stm32f4xx_hal.h"

extern uint8_t LED1_Mode;
extern uint8_t LED2_Mode;
extern uint8_t LED3_Mode;

void LED_Init(void);

void Jiguang_ON(void);
void Jiguang_OFF(void);

void LED0_ON(void);
void LED0_OFF(void);
void LED1_ON(void);
void LED1_OFF(void);
void LED2_ON(void);
void LED2_OFF(void);
void LED3_ON(void);
void LED3_OFF(void);

void LED_Tick(void);
	
#endif

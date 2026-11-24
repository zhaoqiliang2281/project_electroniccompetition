/**
 ******************************************************************************
 * @file    Key.h
 * @brief   按键接口：Key_GetNum 取事件、Key_Tick 定时扫描
 ******************************************************************************
 */

#ifndef __KEY_H
#define __KEY_H

#include "stm32f4xx_hal.h"

uint8_t Key_GetNum(void);
uint8_t Key_GetState(void);
void Key_Tick(void);	//定时器扫描
	
#endif

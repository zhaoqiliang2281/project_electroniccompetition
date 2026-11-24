/**
 ******************************************************************************
 * @file    BUZZER.h
 * @brief   蜂鸣器接口
 * @note    本文件名是全大写，而源文件名是 Buzzer.h —— Windows 下不区分大小写能编过，
 *          但在 Linux/CI 上会找不到头文件，跨平台前请统一大小写。
 ******************************************************************************
 */

#ifndef __BUZZER_H
#define __BUZZER_H

#include "stm32f4xx_hal.h"

extern uint8_t Buzzer_Mode;

void Buzzer_Init(void);

void Buzzer_ON(void);
void Buzzer_OFF(void);
void Buzzer_Turn(void);

void Buzzer_Tick(void);
void Buzzer_SetTimer(uint16_t Count);
void Buzzer_scan(void);

#endif	 
	 
	 

/**
 ******************************************************************************
 * @file    HWT101.h
 * @brief   HWT101 陀螺仪接口（全局角度变量 + 解帧函数）
 * @note    Roll/Pitch/Yaw 为全局变量，直接 extern 使用即可，单位是度。
 ******************************************************************************
 */

#ifndef __HWT101_H
#define __HWT101_H

#include "main.h" 
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>

extern float Roll,Pitch,Yaw;

void HWT101_ReceiveData(uint8_t RxData);

void HWT101_zero(void);

#endif

/**
 ******************************************************************************
 * @file    Serial.h
 * @brief   串口发送接口与蓝牙遥控入口声明
 * @note    本文件里出现 #include "Serial.h"（自包含），无实际作用，可删。
 ******************************************************************************
 */

#ifndef __SERIAL_H
#define __SERIAL_H

#include "stm32f4xx_hal.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
#include "main.h"
#include "can.h"
#include "HWT101.h"
#include "Emm_V5.h"
#include "motor.h"
#include "chassis_behaviour.h"
#include "stm32f4xx_it.h"
#include "OLED.h"
#include "Key.h"
#include "LED.h"
#include "BUZZER.h"
#include "Serial.h"
#include "douji.h"
#include "Serial2.h"

extern char Serial_RxPacket[100];				//定义接收数据包数组，数据包格式"@MSG\r\n"
extern uint8_t Serial_RxFlag;					//定义接收数据包标志位

void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t *Array, uint16_t Length);
void Serial_SendString(char *String);
void Serial_SendNumber(uint32_t Number, uint8_t Length);
void Serial_Printf(char *format, ...);

void lanya_Receive(uint8_t Data);

#endif



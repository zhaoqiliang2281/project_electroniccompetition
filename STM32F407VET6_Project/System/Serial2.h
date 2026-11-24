/**
 ******************************************************************************
 * @file    Serial2.h
 * @brief   MaixCAM 解析接口与接收缓冲声明
 * @note    buffer3 的真实定义在 Serial2.c（256 字节），别在别处再定义一次。
 ******************************************************************************
 */

#ifndef __SERIAL2_H__
#define __SERIAL2_H__

#include "main.h"
#include "usart.h"
#include <stdio.h>
#include <stdarg.h>
#include "Serial.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include <stdlib.h>

extern uint8_t buffer3[256];
extern uint8_t USAR3_ReceiveData_len;
extern int x3;
extern int y3;

void MaixCAM_data_manage(void);

#endif







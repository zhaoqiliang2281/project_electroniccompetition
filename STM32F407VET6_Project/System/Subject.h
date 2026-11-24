/**
 ******************************************************************************
 * @file    Subject.h
 * @brief   三个科目策略函数声明
 * @note    KeyNum/mode_on 是跨文件共享的按键事件与回零标志，改语义时全工程都要看。
 ******************************************************************************
 */

#ifndef __SUBJECT_H
#define __SUBJECT_H

#include "stm32f4xx_hal.h"

extern uint8_t KeyNum;
extern uint8_t mode_on,shi_on;

void KM2(void);
void KM3(void);
void KM4(void);

#endif

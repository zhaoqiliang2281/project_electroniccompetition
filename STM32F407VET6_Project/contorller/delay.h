/**
 ******************************************************************************
 * @file    delay.h
 * @brief   微秒延时接口（delaynus）；delay_us/delay_ms 的声明已注释
 ******************************************************************************
 */

#ifndef __DELAY_H
#define __DELAY_H

#include "stm32f4xx_hal.h"
#include "tim.h"
#include "stdint.h"

void delaynus(uint32_t nus);

//void delay_us(uint16_t us);
//void delay_ms(uint32_t xms);
//void delay_s(uint32_t xs);

#endif

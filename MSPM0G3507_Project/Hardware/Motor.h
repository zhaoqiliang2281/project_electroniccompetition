/**
 ******************************************************************************
 * @file    Motor.h
 * @brief   直流电机驱动接口：四路独立调速 + 整车直行/停车
 * @note    速度单位为“PWM 计数值”，正负号代表方向，不是 RPM。
 ******************************************************************************
 */

#ifndef __MOTOR_H
#define __MOTOR_H

#include "All_include.h"

/* 初始化电机驱动（启动 PWM 计数并把四路占空比清 0） */
void Motor_Init(void);

//右下电机
/* 右下电机：正数正转、负数反转 */
void Motor_RDN(int16_t Speed);
//左下电机
/* 左下电机：正数正转、负数反转 */
void Motor_LDN(int16_t Speed);
//右上电机
/* 右上电机：正数正转、负数反转 */
void Motor_RUP(int16_t Speed);
//左上电机
/* 左上电机：正数正转、负数反转 */
void Motor_LUP(int16_t Speed);

/* 整车直行（左右各给一个速度） */
void car_go(int16 LSpeed,int16 RSpeed);
/* 整车停车（四路占空比归零） */
void car_stop(void);

#endif


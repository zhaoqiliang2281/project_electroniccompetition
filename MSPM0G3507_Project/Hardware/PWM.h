/**
 ******************************************************************************
 * @file    PWM.h
 * @brief   PWM 接口声明，按定时器分组（TIMA0=电机、TIMG6=舵机、TIMG7=预留）
 * @note    Compare 参数是定时器计数值：电机 0~1000，舵机/预留 0~20000。
 ******************************************************************************
 */

#ifndef __PWM_H
#define __PWM_H

#include "All_include.h"

void Motor_PWM_Init(void);
void Servo_PWM_Init(void);

//TIMG6
void Servo_PWM_SetCompare0(uint16_t Compare);
void Servo_PWM_SetCompare1(uint16_t Compare);

//TIMG7
void PWM_SetCompare0(uint16_t Compare);
void PWM_SetCompare1(uint16_t Compare);

//TIMA0
void Motor_PWM_SetCompare0(uint16_t Compare);
void Motor_PWM_SetCompare1(uint16_t Compare);
void Motor_PWM_SetCompare2(uint16_t Compare);
void Motor_PWM_SetCompare3(uint16_t Compare);


#endif

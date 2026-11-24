/**
 ******************************************************************************
 * @file    Servo.h
 * @brief   舵机接口（角度 -> PWM 比较值）与占空比换算说明
 * @note    文件中的 SERVO_MOTOR_DUTY 等宏为早期版本残留，当前换算在 Servo.c 内完成；
 *           舵机在本工程未启用，保留作为通用模板。
 ******************************************************************************
 */

#ifndef __SERVO_H
#define __SERVO_H

#include "All_include.h"

#include "PWM.h"

//#define SERVO_MOTOR_FREQ            (100)                           // 定义主板上舵机频率  请务必注意范围 50-300
//#define PWM_DUTY_MAX     1000                 // PWM最大占空比  最大占空比越大占空比的步进值越小

// ------------------ 舵机占空比计算方式 ------------------
// 舵机对应的 0-180 活动角度对应 控制脉冲的 0.5ms-2.5ms 高电平

// 那么不同频率下的占空比计算方式就是
// PWM_DUTY_MAX/(1000/freq)*(1+Angle/180) 在 50hz 时就是 PWM_DUTY_MAX/(1000/50)*(1+Angle/180)

// 那么 100hz 下 90度的打角 即高电平时间1.5ms 计算套用为
// PWM_DUTY_MAX/(1000/100)*(1+90/180) = PWM_DUTY_MAX/10*1.5

//#define SERVO_MOTOR_DUTY(x)         ((float)PWM_DUTY_MAX/(1000.0/(float)SERVO_MOTOR_FREQ)*(0.5+(float)(x)/90.0))

//#define SERVO_MOTOR_MID             (84)                            // 定义主板上舵机活动范围 角度
//#define SERVO_MOTOR_L_MAX           (64)                            // 中间角度
//#define SERVO_MOTOR_R_MAX           (104)                           // 定义主板上舵机活动范围 角度

void Servo_Init(void);
void Servo_Set(uint32 Steer_duty);



#endif

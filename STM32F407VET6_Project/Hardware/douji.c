/**
 ******************************************************************************
 * @file    douji.c
 * @brief   舵机角度控制（4 通道），当前整体未启用
 * @details
 *   设计意图：Compare = 角度/180*2000 + 500，即 0.5 ms ~ 2.5 ms 脉宽（50 Hz 舵机标准）。
 * @note
 *   douji.c / douji.h 中所有 HAL_TIM_PWM_Start 与 __HAL_TIM_SET_COMPARE 都被注释，
 *   属于“接口留好、硬件没接”的状态；要用舵机需先把这些行打开，
 *   并确认 tim.h 里对应的定时器（原设计是 TIM2 四通道）已配置为 50 Hz。
 ******************************************************************************
 */

#include "douji.h"

void douji_Init(void)
{
//	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_1);
//	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_2);
//	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_3);
//	HAL_TIM_PWM_Start(&htim2,TIM_CHANNEL_4);
}

void douji_SetAngle1(float Angle)
{
//	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_1,Angle / 180 * 2000 + 500);
}

void douji_SetAngle2(float Angle)
{
//	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_2,Angle / 180 * 2000 + 500);	
}

void douji_SetAngle3(float Angle)
{
//	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_3,Angle / 180 * 2000 + 500);	
}

void douji_SetAngle4(float Angle)
{
//	__HAL_TIM_SET_COMPARE(&htim2,TIM_CHANNEL_4,Angle / 180 * 2000 + 500);	
}


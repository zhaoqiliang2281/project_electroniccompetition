/**
 ******************************************************************************
 * @file    Dloud_deck.h
 * @brief   云台限幅、三套 PID 参数与对外接口
 * @note
 *   参数按科目分组，调参时只改本文件的宏：
 *     科二 Cloud_platform_Speedx/Speedy_*       (P 0.08 / 0.12)
 *     科三 Cloud_platform_Speed3x/Speed3y_*     (P 0.08 / 0.12，I 更大)
 *     科四 T_Cloud_platform_Speedx/Speedy_*     (P 0.11 / 0.10，位置式)
 *   P 越大越“跟得紧”，I 用来消静态偏差，D 抑制超调；本工程 D 取得都很小。
 ******************************************************************************
 */

#ifndef __DLOUD_DECK_H
#define __DLOUD_DECK_H

#include "stm32f4xx_hal.h"

/* 位置模式与速度模式的机械限幅：位置 400 脉冲（约 45 度），速度 300 RPM */
#define limit_locationx   400 
#define limit_locationy   400	
	
#define limit_Speedx   		300 
#define limit_Speedy   		300	

//科二速度环
#define Cloud_platform_Speedx_KP   0.08 	// 比例系数
#define Cloud_platform_Speedx_KI   0.00014 	// 积分系数
#define Cloud_platform_Speedx_KD   0.001 	// 微分系数

#define Cloud_platform_Speedy_KP   0.12   	// 比例系数
#define Cloud_platform_Speedy_KI   0.0002 	// 积分系数
#define Cloud_platform_Speedy_KD   0.001 	// 微分系数

//科三速度环
#define Cloud_platform_Speed3x_KP   0.08 		// 比例系数
#define Cloud_platform_Speed3x_KI   0.00024 	// 积分系数
#define Cloud_platform_Speed3x_KD   0.002 		// 微分系数

#define Cloud_platform_Speed3y_KP   0.12   		// 比例系数
#define Cloud_platform_Speed3y_KI   0.0009 	 	// 积分系数0.0002
#define Cloud_platform_Speed3y_KD   0.0004 	 	// 微分系数


//科四速度环
//x
#define T_Cloud_platform_Speedx_KP   0.11 	// 比例系数
#define T_Cloud_platform_Speedx_KI   0.0 	// 积分系数
#define T_Cloud_platform_Speedx_KD   0.002	// 微分系数
//y
#define T_Cloud_platform_Speedy_KP   0.10   	// 比例系数
#define T_Cloud_platform_Speedy_KI   0.0 	 // 积分系数
#define T_Cloud_platform_Speedy_KD   0.05 	 // 微分系数


extern int16_t Cloud_platform_result;

extern int16_t Cloud_platform_Speedresultx;
extern int16_t Cloud_platform_Speedresulty;

extern int16_t Cloud_platform_Speedresult3x;
extern int16_t Cloud_platform_Speedresult3y;

extern int16_t T_Cloud_platform_Speedresultx;
extern int16_t T_Cloud_platform_Speedresulty;

extern int X3Temp;
extern int Y3Temp;
extern uint8_t kozhi;

void cloud_platform(int16_t locationx,int16_t locationy);
void cloud_platformSpeed(int16_t Speedx,int16_t Speedy);

void Cloud_platform_PID_Init(void);
void Cloud_platform_PID_Scan(void);

#endif	 


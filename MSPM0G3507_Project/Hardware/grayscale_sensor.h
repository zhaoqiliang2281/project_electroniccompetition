/**
 ******************************************************************************
 * @file    grayscale_sensor.h
 * @brief   灰度循迹模块接口与 PID 参数（KP/KI/KD 在此处调参）
 * @note    修改循迹手感优先改这里的宏，不要在 .c 里硬编码魔数。
 ******************************************************************************
 */

#ifndef __GRAYSCALE_SENSOR_H__
#define __GRAYSCALE_SENSOR_H__

#include "All_include.h"

#include <stdint.h>
#include "PID.h"

#define NUM_SENSORS 8

//#define automatic_KP   16        // 比例系数
//#define automatic_KI   0.00001 	// 积分系数
//#define automatic_KD   0.03 	// 微分系数
//1
#define automatic_KP   13        // 比例系数
#define automatic_KI   0.0 	// 积分系数
#define automatic_KD   0.003 	// 微分系数

//#define automatic_KP   10        // 比例系数
//#define automatic_KI   0.0 	// 积分系数
//#define automatic_KD   0.003 	// 微分系数

extern pid_type_def PID_xunji;

extern uint8_t location_flag;//读取灰度位置状态值

extern uint8_t sensor_values[];

void OLED_SENSOR_Show(void);

void automatic(void);

// 读取12路红外传感器的值
void Read_Sensors(void);


float Calculate_Error(void);

// 初始化 PID 控制器
void automatic_Init(void);
	

#endif




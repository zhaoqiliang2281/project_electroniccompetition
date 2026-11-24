/**
 ******************************************************************************
 * @file    douji.h
 * @brief   舵机接口声明（4 通道角度设置）
 * @note    文件末尾注释里的模块测试程序可以直接抄到 main 里做单轴验证。
 ******************************************************************************
 */

#ifndef __DOUJI_H__
#define __DOUJI_H__

#include "stm32f4xx_hal.h"
#include "tim.h"

void douji_Init(void);
void douji_SetAngle1(float Angle);
void douji_SetAngle2(float Angle);
void douji_SetAngle3(float Angle);
void douji_SetAngle4(float Angle);

#endif

/*模块测试程序
float Angle;			//定义角度变量
	Key_Init();	
	douji_Init();
	douji_SetAngle(90);

//		douji_SetAngle(150);
//		Delay_ms(1000);
//		douji_SetAngle(30);
//		Delay_ms(1000);
		KeyNum=Key_GetNum();
		if(KeyNum==1)
		{
			Angle+=30;
			if(Angle>180)
			{
				Angle=0;
			}
		}
		douji_SetAngle(Angle);
		OLED_ShowNum(1, 7, Angle, 3);	
*/

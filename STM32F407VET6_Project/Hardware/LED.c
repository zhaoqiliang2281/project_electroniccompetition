/**
 ******************************************************************************
 * @file    LED.c
 * @brief   LED 与激光指示：状态灯 + 定时闪烁 + 激光开关
 * @details
 *   引脚（低电平点亮）：PC0 LED0、PC1 LED1、PC2 LED2、PC3 LED3；
 *   PA1 为激光控制（高电平打开），PA8 为蜂鸣器（见 BUZZER.c）。
 *   LED_Tick() 每 1 ms 调一次，用 LEDx_Mode 选择工作方式：
 *     0=常灭 1=常亮 2=1 Hz 慢闪 3=10 Hz 快闪 4=短亮长灭(100/900) 5=长亮短灭(900/100)
 * @note
 *   LED_Init() 里同时把 PA1/PA2/PA3 复位，作用是上电时确保激光关闭（安全要求）。
 *   Subject.c 里“打中目标”时直接操作 PC0 亮灯，等效于 LED0_ON()。
 ******************************************************************************
 */

#include "LED.h"

uint8_t LED1_Mode;
uint8_t LED2_Mode;
uint8_t LED3_Mode;

uint16_t LED1_Count;
uint16_t LED2_Count;
uint16_t LED3_Count;


/* 上电默认状态：三路 LED 熄灭、激光关闭 */
void LED_Init(void)	//LED初始化关闭LED
{                         
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
	
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_2,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_3,GPIO_PIN_RESET);
}



/* 激光开关（PA1，高电平有效）：命中目标时才允许打开 */
void Jiguang_ON(void)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_SET); //高电平
}
void Jiguang_OFF(void)
{
	HAL_GPIO_WritePin(GPIOA,GPIO_PIN_1,GPIO_PIN_RESET);   //
}




//LED开与关

void LED0_ON(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET); //
}
void LED0_OFF(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);   //高电平
}




void LED1_ON(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_RESET); //
}
void LED1_OFF(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_1,GPIO_PIN_SET);   //高电平
}

void LED2_ON(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_RESET);
}
void LED2_OFF(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_2,GPIO_PIN_SET);
}

void LED3_ON(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_RESET);
}
void LED3_OFF(void)
{
	HAL_GPIO_WritePin(GPIOC,GPIO_PIN_3,GPIO_PIN_SET);
}

//void LED1_color
//定时器扫描(定时器设置每1ms调用一次)
/**
 * @brief  LED 状态机（1 ms 调用一次，内部按 Mode 计数分频）
 * @note   同一套循环写了三遍（LED1/2/3），改闪烁节奏时三处一起改。
 */
void LED_Tick(void)
{
	/********************LED灯1*************************/
	if (LED1_Mode == 0)	//如果是0就关闭
	{
		LED1_OFF();
	}
	else if (LED1_Mode == 1)	//否则如果为1就开
	{
		LED1_ON();
	}
	else if (LED1_Mode == 2)	//否则如果为2就进入相同时间的闪烁模式
	{
		LED1_Count ++;	//设置周期为1000ms
		LED1_Count %= 1000;//if(LED1_Count>999)LED1_Count=0;
		
		if (LED1_Count < 500)	//设置亮灭时间一致
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
	else if (LED1_Mode == 3)	//否则如果为3就进入快速闪烁模式
	{
		LED1_Count ++;
		LED1_Count %= 100;	//设置周期为100ms
		
		if (LED1_Count < 50)
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
	else if (LED1_Mode == 4)	//否则如果为3就进入延迟闪烁模式(亮周期小于灭周期)
	{
		LED1_Count ++;
		LED1_Count %= 1000;
		
		if (LED1_Count < 100)	//在小于100ms内亮
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}
	else if (LED1_Mode == 5)	//否则如果为3就进入延迟闪烁模式(亮周期大于灭周期)
	{
		LED1_Count ++;
		LED1_Count %= 1000;
		
		if (LED1_Count < 900)	//在小于100ms内亮
		{
			LED1_ON();
		}
		else
		{
			LED1_OFF();
		}
	}	
/***********************LED灯2**************************/
	if (LED2_Mode == 0)
	{
		LED2_OFF();
	}
	else if (LED2_Mode == 1)
	{
		LED2_ON();
	}
	else if (LED2_Mode == 2)
	{
		LED2_Count ++;
		LED2_Count %= 1000;
		
		if (LED2_Count < 500)
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
	else if (LED2_Mode == 3)
	{
		LED2_Count ++;
		LED2_Count %= 100;
		
		if (LED2_Count < 50)
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
	else if (LED2_Mode == 4)
	{
		LED2_Count ++;
		LED2_Count %= 1000;
		
		if (LED2_Count < 100)
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
	else if (LED2_Mode == 5)
	{
		LED2_Count ++;
		LED2_Count %= 1000;
		
		if (LED2_Count < 900)
		{
			LED2_ON();
		}
		else
		{
			LED2_OFF();
		}
	}
	/****************LED3**************/
	if (LED3_Mode == 0)
	{
		LED3_OFF();
	}
	else if (LED3_Mode == 1)
	{
		LED3_ON();
	}
	else if (LED3_Mode == 2)
	{
		LED3_Count ++;
		LED3_Count %= 1000;
		
		if (LED3_Count < 500)
		{
			LED3_ON();
		}
		else
		{
			LED3_OFF();
		}
	}
	else if (LED3_Mode == 3)
	{
		LED3_Count ++;
		LED3_Count %= 100;
		
		if (LED3_Count < 50)
		{
			LED3_ON();
		}
		else
		{
			LED3_OFF();
		}
	}
	else if (LED3_Mode == 4)
	{
		LED3_Count ++;
		LED3_Count %= 1000;
		
		if (LED3_Count < 100)
		{
			LED3_ON();
		}
		else
		{
			LED3_OFF();
		}
	}
	else if (LED3_Mode == 5)
	{
		LED3_Count ++;
		LED3_Count %= 1000;
		
		if (LED3_Count < 900)
		{
			LED3_ON();
		}
		else
		{
			LED3_OFF();
		}
	}
}



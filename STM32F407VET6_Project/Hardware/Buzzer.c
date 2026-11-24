/**
 ******************************************************************************
 * @file    Buzzer.c
 * @brief   蜂鸣器：常响/常闭 + “响 N 毫秒”一次性定时
 * @details
 *   引脚 PA8，高电平响。两种用法：
 *     Buzzer_SetTimer(n) + Buzzer_scan()  一次性响 n 个 1 ms 节拍（命中目标提示音）；
 *     Buzzer_Mode + Buzzer_Tick()         常响/慢闪/快闪等长时间模式。
 * @note
 *   1. Buzzer_scan 与 Buzzer_Tick 会互相抢同一个引脚，二者不要同时启用；
 *   2. 文件里 Buzzer_Count 出现两次定义（C 语言允许多次 tentative definition，
 *      只要其中一次带初值），可读性差但不影响编译，清理时保留一处即可。
 ******************************************************************************
 */

#include "BUZZER.h"

uint8_t Buzzer_Mode;

uint16_t Buzzer_Count;


void Buzzer_Init(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
}

void Buzzer_ON(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);	//蜂鸣器开
}
void Buzzer_OFF(void)
{
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);	//蜂鸣器关
}

void Buzzer_Turn(void)
{
	if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_8)==0)//为读取当前端口的电平(为0时)
	{
		HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_8);
	}
}	

uint16_t Buzzer_Count;    // 蜂鸣器响时间计数
uint16_t Buzzer_ScanNum;  // 定时器扫描计数
uint8_t Buzzer_Start;   // 蜂鸣器是否开始响
uint8_t Buzzer_ScanTemp;  //
/************************************************
 *  函数名称: 蜂鸣器设置响时间：Buzzer_SetTimer(uint8_t Start,uint32 Count)
 *  功能说明：引脚：BUZZER_PIN  
 *  参数说明：
 *  参   数：Count：蜂鸣器响时间
 *  函数返回：无
 *  修改时间：2025年6月25日
 *  备   注：
 ************************************************/
/**
 * @brief  一次性鸣响：响 Count 个 1 ms 节拍（Count=50 约 50 ms）
 * @note   必须配合 Buzzer_scan() 使用，后者负责倒计时并关断。
 */
void Buzzer_SetTimer(uint16_t Count)
{
    Buzzer_Count = Count;   //给中断变量赋值
    Buzzer_Start = 1;       //打开蜂鸣器
    Buzzer_ScanTemp=1;
}
// 定时器扫描函数
//定时器扫描(定时器设置每1ms调用一次)
/**
 * @brief  一次性鸣响的倒计时（1 ms 调用）
 * @note   Buzzer_ScanTemp 作为“只关断一次”的标志，避免重复进入关断分支。
 */
void Buzzer_scan(void)
{
    if(Buzzer_Start == 1)
    {
        Buzzer_ScanNum++;
        Buzzer_ON(); // 蜂鸣器响
        if((Buzzer_ScanNum >= Buzzer_Count) && Buzzer_ScanTemp ==1)
        {
            Buzzer_ScanNum = 0;
            Buzzer_OFF(); // 蜂鸣器停止响
            Buzzer_Start = 0;
            Buzzer_ScanTemp = 0;
        }
    }
}


//定时器扫描(定时器设置每1ms调用一次)
/* 蜂鸣器长时模式状态机（1 ms 调用）：与 Buzzer_scan 二选一 */
void Buzzer_Tick(void)
{
	/*********************Buzzer************************/
	if (Buzzer_Mode == 0)	//如果是0就关闭
	{
		Buzzer_OFF();
	}
	else if (Buzzer_Mode == 1)	//否则如果为1就开
	{
		Buzzer_ON();
	}
	else if (Buzzer_Mode == 2)	//否则如果为2就进入相同时间
	{
		Buzzer_Count ++;	//设置周期为1000ms
		Buzzer_Count %= 1000;//if(Buzzer_Count>999)Buzzer_Count=0;
		
		if (Buzzer_Count < 500)
		{
			Buzzer_ON();
		}
		else
		{
			Buzzer_OFF();
		}
	}
	else if (Buzzer_Mode == 3)
	{
		Buzzer_Count ++;
		Buzzer_Count %= 100;	//设置周期为100ms
		
		if (Buzzer_Count < 50)
		{
			Buzzer_ON();
		}
		else
		{
			Buzzer_OFF();
		}
	}
	else if (Buzzer_Mode == 4)
	{
		Buzzer_Count ++;
		Buzzer_Count %= 1000;
		
		if (Buzzer_Count < 100)
		{
			Buzzer_ON();
		}
		else
		{
			Buzzer_OFF();
		}
	}
	else if (Buzzer_Mode == 5)
	{
		Buzzer_Count ++;
		Buzzer_Count %= 1000;
		
		if (Buzzer_Count < 900)
		{
			Buzzer_ON();
		}
		else
		{
			Buzzer_OFF();
		}
	}
}

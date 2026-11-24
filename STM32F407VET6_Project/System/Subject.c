/**
 ******************************************************************************
 * @file    Subject.c
 * @brief   三个“科目”策略：KM2 单轴追踪、KM3 双轴扫图、KM4 双轴纯追踪
 * @details
 *   共同结构：
 *     1) 按键 1 = 开启控制（kozhi=1）、按键 2 = 关闭（kozhi=0）；
 *        首次开启时用 Emm_V5_Origin_Trigger_Return() 让云台回零，保证朝向已知；
 *     2) 视觉像素落在“命中窗口”内则开激光 + 短鸣提示（buzzer_kai 保证只响一次）；
 *     3) 每轮都调用 cloud_platformSpeed() 把 PID 输出下发到云台。
 *   三种策略的差别：
 *     KM2 命中窗口 x:165~175，只用 x 轴 PID；
 *     KM3 命中窗口 x:165~175 且 y:115~128，双轴 PID；丢目标时按 zhuanqu 左/右慢速扫图；
 *     KM4 命中窗口 x:165~175，双轴位置式 PID，不扫图。
 * @note
 *   1. 命中窗口的数值就是“目标在画面里的像素位置”，换摄像头分辨率/安装角度必须重测；
 *   2. mode_on 一旦置 1 就不会再回零，重跑科目要复位板子；
 *   3. KeyNum 由 main.c 读取并清零，本文件只消费；KM3/KM4 内部再清零是为了
 *      避免同一次按键在两处分支里重复触发。
 ******************************************************************************
 */

#include "Subject.h"
#include "Dloud_deck.h"
#include "OLED.h"
#include "Key.h"
#include "LED.h"
#include "can.h"
#include "Emm_V5.h"
#include "BUZZER.h"

uint8_t KeyNum;
uint8_t mode_on,shi_on;

uint8_t zhuanqu;

uint8_t buzzer_kai;

/**
 * @brief  科目二：只追 x 轴（水平），x 命中 165~175 即判命中
 * @note   本策略不扫图，丢目标时云台保持不动。
 */
void KM2(void)
{
	if(KeyNum==1)//上面按键
	{
		kozhi=1;//PID控制标志位，1为开启
		buzzer_kai = 0;
		OLED_ShowNum(1,16,1,1);
		if(kozhi==1 && mode_on==0)
		{
			Emm_V5_Origin_Trigger_Return(2,0,0);
			mode_on = 1;
		}
	}
	if(KeyNum==2)//中间按键
	{
		kozhi=0;
		OLED_ShowNum(1,16,0,1);
	}
	
	if(X3Temp>=165 && X3Temp<=175)	//只要满足范围就进入
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);//开启扳上指示灯
		Jiguang_ON();//开启激光
		if(buzzer_kai==0)
		{
			buzzer_kai = 1;
			Buzzer_SetTimer(50);
		}
		
	}
	else
	{
		Jiguang_OFF();//关闭激光
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_SET);//关闭扳上指示灯
	}

	cloud_platformSpeed(Cloud_platform_Speedresultx,Cloud_platform_Speedresulty);	//x轴测试Cloud_platform_Speedresulty
}


/**
 * @brief  科目三：x/y 双轴追踪 + 丢目标自动扫图
 * @details
 *   按键 1 -> 云台回零后 zhuanqu=1（丢目标时向左慢扫 speed=15）；
 *   按键 2 -> 云台回零后 zhuanqu=2（向右慢扫 speed=-15）；
 *   一旦重新捕获目标就切回双轴 PID 输出。
 * @note   扫图速度 15 RPM 是试出来的“既不过冲又能快速扫到”的值。
 */
void KM3(void)
{
	if(KeyNum==1)
	{
		KeyNum = 0;	//按键清零
		kozhi=1;	//开启PID控制
		buzzer_kai = 0;	//开启到点蜂鸣器响应
		OLED_ShowString(4,15,"y");
		if(kozhi==1 && mode_on==0)
		{
			Emm_V5_Origin_Trigger_Return(1,0,0);
			HAL_Delay(20);
			Emm_V5_Origin_Trigger_Return(2,0,0);
			HAL_Delay(20);
			zhuanqu = 1;
			mode_on = 1;
		}
	}
	if(KeyNum==2)//下面按键
	{
		KeyNum = 0;
		kozhi=1;
		buzzer_kai = 0;
		if(kozhi==1 && mode_on==0)
		{
			Emm_V5_Origin_Trigger_Return(1,0,0);
			HAL_Delay(20);
			Emm_V5_Origin_Trigger_Return(2,0,0);
			HAL_Delay(20);
			zhuanqu = 2;

			mode_on = 1;
		}
		OLED_ShowString(1,16,"y");
	}
	
	if(X3Temp>=165 && X3Temp<=175 && Y3Temp>=115 && Y3Temp<=128)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
		Jiguang_ON();
		if(buzzer_kai==0)
		{
			buzzer_kai = 1;
			Buzzer_SetTimer(50);
		}
	}
	if(zhuanqu==1)
	{
		if(X3Temp == 640 && Y3Temp==480)//没有看到图
		{
			OLED_ShowString(4,15,"n");
			cloud_platformSpeed(15,0);	//左转
		}
	}
	if(zhuanqu==2)
	{
		if(X3Temp == 640 && Y3Temp==480)//没有看到图
		{
			OLED_ShowString(4,15,"n");
			cloud_platformSpeed(-15,0);	//右转
		}
	}
	if(X3Temp != 640 && Y3Temp!=480)
	{
		OLED_ShowString(4,15,"y");
		cloud_platformSpeed(Cloud_platform_Speedresult3x,Cloud_platform_Speedresult3y);	//x，y轴PID
	}
		
}

/**
 * @brief  科目四：x/y 双轴纯追踪（位置式 PID 参数 T_ 前缀）
 * @note   与 KM3 的差别是不做扫图、也不看 y 的命中窗口，只用 x 判命中。
 */
void KM4(void)
{
	if(KeyNum==1)
	{
		KeyNum = 0;
		kozhi=1;
		buzzer_kai = 0;
		OLED_ShowNum(1,16,1,1);
		if(kozhi==1 && mode_on==0)//
		{
			zhuanqu = 1;
			mode_on = 1;
		}
	}
	if(KeyNum==2)//下面按键
	{
		KeyNum = 0;

	}
	
	if(X3Temp>=165 && X3Temp<=175)
	{
		HAL_GPIO_WritePin(GPIOC,GPIO_PIN_0,GPIO_PIN_RESET);
		Jiguang_ON();
		if(buzzer_kai==0)
		{
			buzzer_kai = 1;
			Buzzer_SetTimer(50);
		}
	}
	cloud_platformSpeed(T_Cloud_platform_Speedresultx,T_Cloud_platform_Speedresulty);	//xy轴测试T_Cloud_platform_resulty
		
}



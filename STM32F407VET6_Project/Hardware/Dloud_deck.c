/**
 ******************************************************************************
 * @file    Dloud_deck.c
 * @brief   二轴激光云台控制：像素误差 -> 三套增量式 PID -> CAN 速度下发
 * @details
 *   机械结构：两个 Emm_V5 闭环步进（16 细分，3200 脉冲/圈），ID1 与 ID2 各带一轴。
 *   控制流程：
 *     MaixCAM 给出目标像素 (x,y) -> Cloud_platform_PID_Scan()（10 ms 一次）
 *     与“画面中心目标像素”做增量式 PID -> 得到 x/y 速度
 *     -> cloud_platformSpeed() 用 CAN 下发速度指令 -> 电机转动 -> 目标回中。
 *   三套参数按“科目”区分（对应 Subject.c 的 KM2/KM3/KM4）：
 *     科二 取目标 (170, 115)，只用 x 轴；
 *     科三 取目标 (170, 118)，x/y 双轴，配合左右扫图；
 *     科四 取目标 (170, 115)，x/y 双轴，位置式 PID（T_ 前缀）。
 * @warning
 *   1. cloud_platformSpeed() 里每个速度下发后面都有 while(can.rxFrameFlag == false)
 *      的忙等，属于“发一帧等一帧”的同步写法；CAN 异常时程序会停在这里；
 *   2. 代码注释写“ID1 控制 Y 轴、ID2 控制 X 轴”，但实参是 Speedx 给 ID1，
 *      现场若发现左右/上下反了，以实际接线为准调整这里的对应关系；
 *   3. PID 输出限幅 100（速度 RPM），云台机械限位另由 limit_Speedx/Speedy 限制。
 ******************************************************************************
 */

#include "Dloud_deck.h"
#include "can.h"
#include "HWT101.h"
#include "Emm_V5.h"
#include "motor.h"
#include "PID.h"
#include "Serial2.h"
#include "subject.h"

//科二速度环
pid_type_def PID_Cloud_platform_Speedx;
pid_type_def PID_Cloud_platform_Speedy;

int16_t Cloud_platform_Speedresultx;
int16_t Cloud_platform_Speedresulty;

//科三速度环
pid_type_def PID_Cloud_platform_Speed3x;
pid_type_def PID_Cloud_platform_Speed3y;

int16_t Cloud_platform_Speedresult3x;
int16_t Cloud_platform_Speedresult3y;


//科四速度环
pid_type_def T_PID_Cloud_platform_Speedx;
pid_type_def T_PID_Cloud_platform_Speedy;

int16_t T_Cloud_platform_Speedresultx;
int16_t T_Cloud_platform_Speedresulty;

/************************************************
 *  函数名称: 云台控制(位置环)：Acloud_platform(int16_t locationx,int16_t locationy)
 *  功能说明：速度与位置PID串级
 *  参数说明：locationx为X轴方向，locationy为Y轴方向
 *  参   数： X,Y轴的输入脉冲数  （16细分下发送3200个脉冲电机转一圈）
 *  函数返回：无
 *  修改时间：2025年7月31日
 *  备   注：目前位置环只有X轴
 ************************************************/
/**
 * @brief  位置模式控制云台（开环走固定脉冲数，当前未被主流程调用）
 * @param  locationx/locationy 目标脉冲数，内部按 400 限幅
 * @note   16 细分下 3200 脉冲 = 1 圈，400 脉冲约 45 度；
 *         正负号决定正反转，方向参数由 Emm_V5_Pos_Control 的第 2 个入参给出。
 */
void cloud_platform(int16_t locationx,int16_t locationy)
{
	if(locationx >= limit_locationx){locationx=limit_locationx;}
	else if(locationx <= -limit_locationx){locationx=-limit_locationx;}
	
	if(locationy >= limit_locationy){locationy=limit_locationy;}
	else if(locationy <= -limit_locationy){locationy=-limit_locationy;}
	
	if(locationx>0)
	{
		Emm_V5_Pos_Control(1, 0, 10, 0, locationx, 0, 0);	//电机ID1控制Y轴方向
//		while(can.rxFrameFlag == false){};can.rxFrameFlag = false;
		HAL_Delay(2);	
	}
	else
	{
		Emm_V5_Pos_Control(1, 1, 10, 0, -locationx, 0, 0);	
//		while(can.rxFrameFlag == false){};can.rxFrameFlag = false;
		HAL_Delay(2);
	}
	if(locationy>0)
	{
		Emm_V5_Pos_Control(2, 0, 10, 0, locationy, 0, 0);	//电机ID2控制X轴方向
//		while(can.rxFrameFlag == false){};can.rxFrameFlag = false;
		HAL_Delay(2);
	}
	else
	{
		Emm_V5_Pos_Control(2, 1, 10, 0, -locationy, 0, 0);	
//		while(can.rxFrameFlag == false){};can.rxFrameFlag = false;
		HAL_Delay(2);
	}
}
/************************************************
 *  函数名称: 云台控制(速度环)：cloud_platformSpeed(int16_t Speedx,int16_t Speedy)
 *  功能说明：速度与位置PID串级
 *  参数说明：Speedx为X轴方向速度，Speedy为Y轴方向速度
 *  参   数： 
 *  函数返回：无
 *  修改时间：2025年8月1日
 *  备   注：
 ************************************************/
/**
 * @brief  速度模式控制云台（闭环追踪主用）
 * @param  Speedx 水平轴速度（RPM）；Speedy 俯仰轴速度（RPM）
 * @note   先按 300 RPM 限幅，再按符号拆成“方向 + 绝对速度”下发；
 *         每帧都阻塞等回帧，因此本函数在主循环里被调用时耗时约 1~2 ms。
 */
void cloud_platformSpeed(int16_t Speedx,int16_t Speedy)
{
	if(Speedx >= limit_Speedx){Speedx=limit_Speedx;}
	else if(Speedx <= -limit_Speedx){Speedx=-limit_Speedx;}
	
	if(Speedy >= limit_Speedy){Speedy=limit_Speedy;}
	else if(Speedy <= -limit_Speedy){Speedy=-limit_Speedy;}
	
	if(Speedx>0)
	{
		Emm_V5_Vel_Control(1, 0, Speedx, 0, 0);	//电机ID1控制Y轴方向
		while(can.rxFrameFlag == false){}; can.rxFrameFlag = false;
	}
	else
	{
		Emm_V5_Vel_Control(1, 1, -Speedx, 0, 0);	
		while(can.rxFrameFlag == false){}; can.rxFrameFlag = false;
	}
	if(Speedy>0)
	{
		Emm_V5_Vel_Control(2, 0, Speedy, 0, 0);	//电机ID2控制X轴方向
		while(can.rxFrameFlag == false){}; can.rxFrameFlag = false;
	}
	else
	{
		Emm_V5_Vel_Control(2, 1, -Speedy, 0, 0);
		while(can.rxFrameFlag == false){}; can.rxFrameFlag = false;
	}
}

/**
 * @brief  云台三套 PID 初始化
 * @note   科二/科三用 PID_DELTA（增量式，适合速度环）；
 *         科四用 PID_POSITION（位置式），参数在 Dloud_deck.h 里按科目分组。
 *         输出限幅 100、积分限幅 100。
 */
void Cloud_platform_PID_Init(void)
{
	//科二速度环
	const  float Cloud_platformPID_SpeedValuex[3]={Cloud_platform_Speedx_KP,Cloud_platform_Speedx_KI,Cloud_platform_Speedx_KD};
	PID_init(&PID_Cloud_platform_Speedx,PID_DELTA,Cloud_platformPID_SpeedValuex,100,100);
	
	const  float Cloud_platformPID_SpeedValuey[3]={Cloud_platform_Speedy_KP,Cloud_platform_Speedy_KI,Cloud_platform_Speedy_KD};
	PID_init(&PID_Cloud_platform_Speedy,PID_DELTA,Cloud_platformPID_SpeedValuey,100,100);
	
	//科三速度环
	const  float Cloud_platformPID_SpeedValue3x[3]={Cloud_platform_Speed3x_KP,Cloud_platform_Speed3x_KI,Cloud_platform_Speed3x_KD};
	PID_init(&PID_Cloud_platform_Speed3x,PID_DELTA,Cloud_platformPID_SpeedValue3x,100,100);
	
	const  float Cloud_platformPID_SpeedValue3y[3]={Cloud_platform_Speed3y_KP,Cloud_platform_Speed3y_KI,Cloud_platform_Speed3y_KD};
	PID_init(&PID_Cloud_platform_Speed3y,PID_DELTA,Cloud_platformPID_SpeedValue3y,100,100);

	//科四速度环
	const  float T_Cloud_platformPID_SpeedValuex[3]={T_Cloud_platform_Speedx_KP,T_Cloud_platform_Speedx_KI,T_Cloud_platform_Speedx_KD};
	PID_init(&T_PID_Cloud_platform_Speedx,PID_POSITION,T_Cloud_platformPID_SpeedValuex,100,100);
	
	const  float T_Cloud_platformPID_SpeedValuey[3]={T_Cloud_platform_Speedy_KP,T_Cloud_platform_Speedy_KI,T_Cloud_platform_Speedy_KD};
	PID_init(&T_PID_Cloud_platform_Speedy,PID_POSITION,T_Cloud_platformPID_SpeedValuey,100,100);

}


int X3Temp=0;
int Y3Temp=0;
uint8_t kozhi;
/**
 * @brief  云台控制主计算：每 10 ms 由 TIM6 回调调用一次
 * @details
 *   1. 把串口解析出的 x3/y3 拷到 X3Temp/Y3Temp（供 OLED 与策略层读取）；
 *   2. 若 (640,480) 表示“目标丢失”，此时不更新 PID（保持上一次输出，云台停住）；
 *   3. kozhi==1（按键开启控制）才真正算 PID，避免上电乱动。
 * @note   PID_calc 的形参顺序是 (反馈, 目标)，这里反馈=像素坐标，目标=画面中心像素。
 */
void Cloud_platform_PID_Scan(void)
{
	//速度环
	X3Temp = x3;
	Y3Temp = y3;

	if(X3Temp!=640 && Y3Temp!=480)
	{
		if(kozhi==1)	//开启PID
		{
			//科二
			Cloud_platform_Speedresultx = PID_calc(&PID_Cloud_platform_Speedx,X3Temp,170);
//			Cloud_platform_Speedresulty = PID_calc(&PID_Cloud_platform_Speedy,Y3Temp,115);
			
			//科三
			Cloud_platform_Speedresult3x = PID_calc(&PID_Cloud_platform_Speed3x,X3Temp,170);
			Cloud_platform_Speedresult3y = PID_calc(&PID_Cloud_platform_Speed3y,Y3Temp,118);
			
			//科四
			T_Cloud_platform_Speedresultx = PID_calc(&T_PID_Cloud_platform_Speedx,X3Temp,170);
			T_Cloud_platform_Speedresulty = PID_calc(&T_PID_Cloud_platform_Speedy,Y3Temp,115);
		}		
	}
}


/**
 ******************************************************************************
 * @file    Motor.c
 * @brief   四路直流减速电机驱动（方向 GPIO + PWM 占空比调速）
 * @details
 *   每路电机用两个方向引脚组成 H 桥逻辑（D1/D2 互补，正转 D2=1 D1=0，反转相反），
 *   速度大小由 TIMA0 对应通道的 PWM 占空比给出（周期 1000，5 kHz）。
 *   输入速度范围建议 -1000 ~ +1000：正数正转、负数反转、0 停转。
 * @note
 *   同一侧电机共用一路 PWM（本车把同侧两轮并在一起驱动），
 *   因此 Motor_LUP / Motor_LDN / Motor_RUP / Motor_RDN 里各自调用的
 *   Motor_PWM_SetCompareX 不同，改动时务必与接线核对，否则会出现“给右轮
 *   速度却动了左轮”的现象。若发现整车反向，交换对应电机的 D1/D2 引脚即可。
 ******************************************************************************
 */

#include "All_include.h"
#include "PWM.h"

/**
 * @brief  电机驱动初始化：启动 TIMA0 并把四路占空比清 0
 * @note   只依赖 PWM 层，必须在 Motor_PWM 时钟就绪后被调用（main 中在 SysConfig 之后）。
 */
void Motor_Init(void)
{
	Motor_PWM_Init();
	Motor_PWM_SetCompare0(0);
	Motor_PWM_SetCompare1(0);
	Motor_PWM_SetCompare2(0);
	Motor_PWM_SetCompare3(0);
}
//右上电机
/**
 * @brief  右上电机调速
 * @param  Speed 目标速度：>0 正转、<0 反转，建议 |Speed| <= 1000
 * @note   正/反转通过 PB11、PB14 两个方向脚的电平组合切换。
 */
void Motor_RUP(int16_t Speed)  //直流电机设置速度//Speed 要设置的速度，范围：-1000~1000
{
	if (Speed>0)	//如果设置正转的速度值
	{
//		Motor_PWM_SetCompare0(Speed);
//		Motor_PWM_SetCompare1(Speed);
//		Motor_PWM_SetCompare2(Speed);
		Motor_PWM_SetCompare3(Speed);
		
		DL_GPIO_clearPins(GPIOB,DL_GPIO_PIN_11);// 
		DL_GPIO_setPins(GPIOB,DL_GPIO_PIN_14);
	}
	else		//否则，即设置反转的速度值
	{
//		Motor_PWM_SetCompare0(-Speed);
//		Motor_PWM_SetCompare1(-Speed);
//		Motor_PWM_SetCompare2(-Speed);
		Motor_PWM_SetCompare3(-Speed);
		
		DL_GPIO_clearPins(GPIOB,DL_GPIO_PIN_14);// 
		DL_GPIO_setPins(GPIOB,DL_GPIO_PIN_11);
	}
}
//左上电机
/**
 * @brief  左上电机调速（方向脚 PB20/PB21）
 * @param  Speed 目标速度：>0 正转、<0 反转，建议 |Speed| <= 1000
 */
void Motor_LUP(int16_t Speed)  //直流电机设置速度//Speed 要设置的速度，范围：-100~100
{
	if (Speed>0)	//如果设置正转的速度值
	{
//		Motor_PWM_SetCompare0(Speed);
//		Motor_PWM_SetCompare1(Speed);
//		Motor_PWM_SetCompare2(Speed);
		Motor_PWM_SetCompare3(Speed);
		
		DL_GPIO_clearPins(GPIOB,DL_GPIO_PIN_20);// 
		DL_GPIO_setPins(GPIOB,DL_GPIO_PIN_21);
	}
	else		//否则，即设置反转的速度值
	{
//		Motor_PWM_SetCompare0(-Speed);
//		Motor_PWM_SetCompare1(-Speed);
//		Motor_PWM_SetCompare2(-Speed);
		Motor_PWM_SetCompare3(-Speed);
		
		DL_GPIO_clearPins(GPIOB,DL_GPIO_PIN_21);// 
		DL_GPIO_setPins(GPIOB,DL_GPIO_PIN_20);
	}
}
//右下电机
/**
 * @brief  右下电机调速（方向脚 PB24/PB25，PWM 用 TIMA0 通道 2）
 * @param  Speed 目标速度：>0 正转、<0 反转，建议 |Speed| <= 1000
 */
void Motor_RDN(int16_t Speed)  //直流电机设置速度//Speed 要设置的速度，范围：-1000~1000
{
	if(Speed>0)	//如果设置正转的速度值
	{
//		Motor_PWM_SetCompare0(Speed);
//		Motor_PWM_SetCompare1(Speed);
		Motor_PWM_SetCompare2(Speed);
//		Motor_PWM_SetCompare3(Speed);

		DL_GPIO_clearPins(GPIOB,DL_GPIO_PIN_24);// 正转
		DL_GPIO_setPins(GPIOB,DL_GPIO_PIN_25);//编码器全正

	}
	else		//否则，即设置反转的速度值
	{
//		Motor_PWM_SetCompare0(-Speed);
//		Motor_PWM_SetCompare1(-Speed);
		Motor_PWM_SetCompare2(-Speed);
//		Motor_PWM_SetCompare3(-Speed);

		DL_GPIO_clearPins(GPIOB,DL_GPIO_PIN_25);// //编码器全反
		DL_GPIO_setPins(GPIOB,DL_GPIO_PIN_24);
	}
}
//左下电机
/**
 * @brief  左下电机调速（方向脚 PB22/PB23，PWM 用 TIMA0 通道 3）
 * @param  Speed 目标速度：>0 正转、<0 反转，建议 |Speed| <= 1000
 * @note   本车循迹主要调用的就是这一路 + 右下电机。
 */
void Motor_LDN(int16_t Speed)  //直流电机设置速度//Speed 要设置的速度，范围：-1000~1000
{
	if (Speed>0)	//如果设置正转的速度值
	{
//		Motor_PWM_SetCompare0(Speed);
//		Motor_PWM_SetCompare1(Speed);
//		Motor_PWM_SetCompare2(Speed);
		Motor_PWM_SetCompare3(Speed);
		
		DL_GPIO_clearPins(GPIOB,DL_GPIO_PIN_22);// 
		DL_GPIO_setPins(GPIOB,DL_GPIO_PIN_23);
	}
	else		//否则，即设置反转的速度值
	{
//		Motor_PWM_SetCompare0(-Speed);
//		Motor_PWM_SetCompare1(-Speed);
//		Motor_PWM_SetCompare2(-Speed);
		Motor_PWM_SetCompare3(-Speed);
		
		DL_GPIO_clearPins(GPIOB,DL_GPIO_PIN_23);// 
		DL_GPIO_setPins(GPIOB,DL_GPIO_PIN_22);
	}
}

/**
 * @brief  整车直行：左右两侧同时给定速度
 * @param  LSpeed 左侧两轮速度；RSpeed 右侧两轮速度（单位与单电机一致）
 */
void car_go(int16 LSpeed,int16 RSpeed)//
{
	Motor_RUP(RSpeed);
	Motor_RDN(RSpeed);
	
	Motor_LUP(LSpeed);
	Motor_LDN(LSpeed);

}

/**
 * @brief  整车停止：四路速度全部置 0（不是刹车，只是占空比归零）
 */
void car_stop(void)//
{
	Motor_RUP(0);
	Motor_RDN(0);
	
	Motor_LUP(0);
	Motor_LDN(0);
}

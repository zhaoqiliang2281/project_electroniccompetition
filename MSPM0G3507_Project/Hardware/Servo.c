/**
 ******************************************************************************
 * @file    Servo.c
 * @brief   舵机角度控制（50 Hz 类 PWM 占空比换算）
 * @details
 *   Servo_Set() 把 0 ~ 180 度线性映射成 500 ~ 2500 的比较值，
 *   对应 0.5 ms ~ 2.5 ms 的高电平宽度（标准舵机的位置信号）。
 * @note
 *   1. 比较值能否真的变成 0.5~2.5 ms，取决于 SysConfig 中 TIMG6 的周期：
 *      当前工程 TIMG6 周期为 20000 @10 MHz（约 2 ms），若要驱动标准 50 Hz 舵机，
 *      需要把周期改成 20 ms 级（例如 10 MHz 下 200000），并同步修改换算系数。
 *   2. 本车 main.c 中 Servo_Init() 处于注释状态，舵机功能未启用，仅供移植参考。
 ******************************************************************************
 */

#include "All_include.h"
#include "Servo.h"

/************************************************
 *  函数名称：舵机初始化：Servo_Init(void)
 *  功能说明：
 *  参数说明：
 *  参   数： 无
 *  函数返回：无
 *  修改时间：2025年6月12日
 *  备    注：
 ************************************************/
/**
 * @brief  舵机初始化：启动舵机 PWM 定时器
 * @note   默认不预置角度，避免上电瞬间舵机“甩头”；需要中位时调用 Servo_Set(90)。
 */
void Servo_Init(void)               //舵机初始化
{
	Servo_PWM_Init();
//	Servo_Set(SERVO_MOTOR_MID);
}
//void servo_angle(DL_TIMER_CC_INDEX ccIndex, uint8_t angle) {
//  uint16_t value;
//  if(angle >= 0 && angle <= 180)
//    value = angle / 180.0 * 100 + 875;
//  else
//    value = 875;

//}
/************************************************
 *  函数名称：舵机角度设置：Servo_Set(uint32 Steer_duty)
 *  功能说明：
 *  参数说明：范围：75~95~115
 *  参   数：PWM占空比
 *  函数返回：无
 *  修改时间：2025年6月12日
 *  备    注：
 ************************************************/
/**
 * @brief  设置舵机角度
 * @param  Steer_duty 角度值，0 ~ 180（度）
 * @note   500 为中位偏置（0.5 ms），2000 为 180 度对应的脉宽增量，
 *         即 Compare = 角度/180 * 2000 + 500。
 */
void Servo_Set(uint32 Steer_duty)    //舵机角度
{
//    //舵机限幅
//    if(Steer_duty < SERVO_MOTOR_L_MAX){Steer_duty=SERVO_MOTOR_L_MAX;}   //
//    if(Steer_duty > SERVO_MOTOR_R_MAX){Steer_duty=SERVO_MOTOR_R_MAX;}   //

//  if(Steer_duty >= 0 && Steer_duty <= 180)
//    Steer_duty = Steer_duty / 180.0 * 100 + 875;
//  else
//    Steer_duty = 875;
//    Servo_PWM_SetCompare0((uint32)SERVO_MOTOR_DUTY(Steer_duty)); //舵机PWM输出
	Servo_PWM_SetCompare0(Steer_duty / 180 * 2000 + 500);
}

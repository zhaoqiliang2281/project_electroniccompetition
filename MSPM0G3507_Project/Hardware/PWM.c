/**
 ******************************************************************************
 * @file    PWM.c
 * @brief   PWM 输出封装（电机 / 舵机 / 预留通道）
 * @details
 *   SysConfig 里的三组定时器分工：
 *     TIMA0  周期 1000 @5 MHz -> 5 kHz，四通道驱动电机（0 ~ 1000 对应 0 ~ 100 %）
 *     TIMG6  周期 20000 @10 MHz -> 约 2 ms 周期，用于舵机（本工程未启用）
 *     TIMG7  周期 20000 @10 MHz -> 约 2 ms 周期，预留通用 PWM
 *   本文件只负责“启动计数器”和“写比较值”，引脚复用已由 ti_msp_dl_config.c 完成。
 * @note
 *   setCaptureCompareValue 的参数是“计数值”而非百分比，换算关系：
 *   占空比 = Compare / 周期计数值。
 ******************************************************************************
 */

#include "All_include.h"

/**
 * @brief  启动电机 PWM 定时器（TIMA0）
 * @note   TIMA0 在 SysConfig 中配置为 startTimer = START，此处再次启动是幂等的。
 */
void Motor_PWM_Init(void)
{
	DL_TimerG_startCounter(TIMA0);
}
/**
 * @brief  启动舵机 PWM 定时器（TIMG6）
 * @note   TIMG7 的启动语句被注释掉，需要用到预留通道时再打开。
 */
void Servo_PWM_Init(void)
{
	DL_TimerG_startCounter(TIMG6);
//	DL_TimerG_startCounter(TIMG7);
}

//TIMG6
/* 舵机 PWM 通道 0 / 1：写比较值（TIMG6） */
void Servo_PWM_SetCompare0(uint16_t Compare)
{
	DL_TimerG_setCaptureCompareValue(TIMG6, Compare, DL_TIMER_CC_0_INDEX);
}
/* 舵机 PWM 通道 1（TIMG6） */
void Servo_PWM_SetCompare1(uint16_t Compare)
{
	DL_TimerG_setCaptureCompareValue(TIMG6, Compare, DL_TIMER_CC_1_INDEX);
}

//TIMG7
/* 预留通用 PWM 通道 0 / 1（TIMG7） */
void PWM_SetCompare0(uint16_t Compare)
{
	DL_TimerG_setCaptureCompareValue(TIMG7, Compare, DL_TIMER_CC_0_INDEX);
}
/* 预留通用 PWM 通道 1（TIMG7） */
void PWM_SetCompare1(uint16_t Compare)
{
	DL_TimerG_setCaptureCompareValue(TIMG7, Compare, DL_TIMER_CC_1_INDEX);
}

//TIMA0
/* 电机 PWM：TIMA0 四个通道分别对应四路电机（通道与车轮的对应关系见 Motor.c） */
void Motor_PWM_SetCompare0(uint16_t Compare)
{
	DL_TimerG_setCaptureCompareValue(TIMA0, Compare, DL_TIMER_CC_0_INDEX);
}
void Motor_PWM_SetCompare1(uint16_t Compare)
{
	DL_TimerG_setCaptureCompareValue(TIMA0, Compare, DL_TIMER_CC_1_INDEX);
}
void Motor_PWM_SetCompare2(uint16_t Compare)
{
	DL_TimerG_setCaptureCompareValue(TIMA0, Compare, DL_TIMER_CC_2_INDEX);
}
void Motor_PWM_SetCompare3(uint16_t Compare)
{
	DL_TimerG_setCaptureCompareValue(TIMA0, Compare, DL_TIMER_CC_3_INDEX);
}

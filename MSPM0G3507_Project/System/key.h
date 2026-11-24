/**
 ******************************************************************************
 * @file    key.h
 * @brief   按键 / 蜂鸣器接口
 * @note    Key_GetNumXxx 是阻塞式（按下期间会卡住），key_scan 是非阻塞式，
 *          按用途二选一，不要在同一个工程里混用同一组按键。
 ******************************************************************************
 */

#ifndef __KEY_H__
#define __KEY_H__

#include "All_include.h"

/* 阻塞式读 PA30：返回 1 表示“按下并已松手” */
uint8_t Key_GetNum(void);
/* 阻塞式读 PA31：总开关 */
uint8_t Key_GetNum1(void);
/* 阻塞式读 PB10：切换运行模式 */
uint8_t Key_GetNum10(void);
/* 阻塞式读 PB17 / PB18 / PB19（备用键，模式 1 中用于加减基础速度） */
uint8_t Key_GetNum17(void);
/* 阻塞式读 PB18 */
uint8_t Key_GetNum18(void);
/* 阻塞式读 PB19 */
uint8_t Key_GetNum19(void);


/* 非阻塞扫描：置 key1_flag / key2_flag */
void key_scan(void);


/* 蜂鸣器：ON 响、OFF 停 */
void Buzzer_ON(void);
/* 蜂鸣器关 */
void Buzzer_OFF(void);
	
extern uint8_t key1_flag;
extern uint8_t key2_flag;
//extern uint8_t key3_flag;
//extern uint8_t key4_flag;

#endif


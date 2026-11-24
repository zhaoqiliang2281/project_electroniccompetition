/**
 ******************************************************************************
 * @file    OLED.h
 * @brief   OLED 显示接口（4 行 x 16 列字符模式）
 * @note    坐标均为 1 基：Line 1~4，Column 1~16；
 *          OLED_Wave / OLED_ShowChinese / OLED_ShowFloat 为扩展功能，按需使用。
 ******************************************************************************
 */

#ifndef __OLED_H
#define __OLED_H

#include "ti_msp_dl_config.h"

/* 初始化屏（含复位时序） */
void OLED_Init(void);
/* 清屏（写满 0） */
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
/* 显示字符串 / 单个字符 */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
/* 显示无符号数（Length 决定占几位） */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
/* 显示有符号数（会额外占 1 位符号位） */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
/* 显示 16 进制 / 2 进制 */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
/* 显示二进制数 */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

/* 简易波形刷新（把数值画成柱状），调试 PID 输出很方便 */
void OLED_Wave(uint8_t Wave_Sum);
/* 显示内置汉字（字模在 OLED_Font.h，按序号取字） */
void OLED_ShowChinese(uint8_t Line,uint8_t Column,uint8_t ChineseLoca);
/* 显示浮点数（固定 1 位小数） */
void OLED_ShowFloat(uint8_t Line, uint8_t Column,float Parameter);


#endif

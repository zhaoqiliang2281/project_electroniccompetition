/**
 ******************************************************************************
 * @file    OLED.h
 * @brief   OLED 显示接口（4 行 x 16 列字符模式）
 * @note    坐标 1 基：Line 1~4、Column 1~16；ShowNum 的 Length 指“占几列”。
 ******************************************************************************
 */

#ifndef __OLED_H
#define __OLED_H

#include "main.h"
void HAL_Delay_us(uint32_t us, uint16_t CLK_MHz );
void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char);
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String);
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length);
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length);

void OLED_Wave(uint8_t Wave_Sum);
void OLED_ShowChinese(uint8_t Line,uint8_t Column,uint8_t ChineseLoca);
void OLED_ShowFloat(uint8_t Line, uint8_t Column,float Parameter);

#endif

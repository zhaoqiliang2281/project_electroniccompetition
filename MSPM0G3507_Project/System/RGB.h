/**
 ******************************************************************************
 * @file    RGB.h
 * @brief   RGB 指示灯接口与颜色枚举（低电平点亮）
 ******************************************************************************
 */

#ifndef __RGB_H__
#define __RGB_H__

typedef enum {
	
	RGB_Red = 1,
	RGB_Green = 2,	
	RGB_Blue = 3

}RGB_Colour;

void Boards_RGB_OFF(void);
void Boards_RGB_Colour(RGB_Colour Colour);
	
#endif

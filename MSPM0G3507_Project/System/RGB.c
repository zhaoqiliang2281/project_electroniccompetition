/**
 ******************************************************************************
 * @file    RGB.c
 * @brief   板载 RGB 指示灯（PA29 / PA30 / PA31，低电平点亮）
 * @details
 *   RGB 是共阳接法：引脚输出低电平对应的颜色才亮，因此“全灭”= 三个引脚都给 1。
 *   RGB_Colour 枚举：1=红(PA29)、2=绿(PA31)、3=蓝(PA30)，一次只亮一种颜色。
 * @note   本工程 main.c 中未启用（#include "RGB.h" 被注释），保留作为状态指示模板。
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"
#include "RGB.h"

/************************************************
 *  函数名称：RGB灯全部关闭：Boards_RGB_OFF(void)
 *  功能说明：关闭全部RGB灯
 *  参数说明：
 *  参    数：无
 *  函数返回：无
 *  修改时间：2025年7月14日
 *  备   注：
 ************************************************/
/**
 * @brief  三色全灭
 * @note   上面三行注释掉的是“用 SysConfig 宏名”的写法，改成直接写 GPIOA+引脚号，
 *         换板子时改这三行即可。
 */
void Boards_RGB_OFF(void) 
{
//  DL_GPIO_setPins(RGB_PORT, RGB_RGB_Red_PIN);
//  DL_GPIO_setPins(RGB_PORT, RGB_RGB_Green_PIN);
//  DL_GPIO_setPins(RGB_PORT, RGB_RGB_Blue_PIN);
	DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_29);
	DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_30);
	DL_GPIO_setPins(GPIOA, DL_GPIO_PIN_31);
}

/************************************************
 *  函数名称：选择点亮那个RGB灯颜色：Boards_RGB_Colour(RGB_Colour Colour)
 *  功能说明：关闭全部RGB灯
 *  参数说明：枚举：RGB_Colour
 *  参    数：RGB_Red，RGB_Green，RGB_Blue 选择对应的颜色
 *  函数返回：无
 *  修改时间：2025年7月14日
 *  备   注：
 ************************************************/
/**
 * @brief  点亮指定颜色（其余颜色先熄）
 * @param  Colour RGB_Red / RGB_Green / RGB_Blue
 * @note   先调 Boards_RGB_OFF() 再拉低对应引脚，避免出现混色。
 */
void Boards_RGB_Colour(RGB_Colour Colour)
{
	//自画板RGB
	switch (Colour)
	{
		case 1:
		{
			Boards_RGB_OFF();
			DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_29);//低电平
		}
			break;
		case 2:
		{
			Boards_RGB_OFF();
			DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_31);//低电平
		}
			break;
		case 3:
		{
			Boards_RGB_OFF();
			DL_GPIO_clearPins(GPIOA,DL_GPIO_PIN_30);//低电平
		}
			break;
	}
}

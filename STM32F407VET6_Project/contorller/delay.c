/**
 ******************************************************************************
 * @file    delay.c
 * @brief   微秒级延时（硬件定时器计数法），供 OLED 软件 I2C 使用
 * @details
 *   TIM14 被配置为 1 MHz 计数（APB1 定时器时钟 84 MHz / 分频 84），
 *   即 1 个计数 = 1 us。delaynus() 先把计数器设到 (0xffff - nus - 5)，
 *   再忙等到计数超过 0xffff-5，差值即为延时长度；减 5 是补偿启动/停止开销。
 * @note
 *   1. 单次最多约 65.5 ms（受 16 位计数器限制），更长的延时请循环调用；
 *   2. 该延时依赖 TIM14 的时钟配置（见 tim.c），改主频后必须重新校准；
 *   3. 文件内大段注释是 TIM7 版本的旧实现与原理说明，保留作为移植参考。
 ******************************************************************************
 */

/*这个是oled的延时函数*/
#include "tim.h"
#include "delay.h"

/**
 * @brief  微秒级延时（忙等，期间 CPU 不能做别的事）
 * @param  nus 延时时长（微秒），建议 <= 60000
 * @note   里层 while 靠读 TIM14 计数器退出，若定时器未初始化会死循环。
 */
void delaynus(uint32_t nus)
{
 
 uint16_t  differ = 0xffff-nus-5;
 //设置定时器2的技术初始值
  __HAL_TIM_SetCounter(&htim14,differ);
  //开启定时器
  HAL_TIM_Base_Start(&htim14);
 
  while( differ<0xffff-5)
 {
  differ = __HAL_TIM_GetCounter(&htim14);
 };
 //关闭定时器
  HAL_TIM_Base_Stop(&htim14);
//基于定时器的us延时，此为tim2在apb1总线的84mhz，分频系数83，周期65535
//主函数记者第一个tim2初始化。

}




/**AQI
  *采用基本定时器7做延迟
  * @brief  微秒级延时
  * @param  xus 延时时长，范围：0~233015
  * @retval 无
  
对于延迟函数解释 ，首先在STM32CubeMx配置定时器中勾选Activated(激活)，不需要开启NVIC中断 
void delay_us(uint16_t us)  
这段代码是一个用于在微控制器上实现微秒级延时的函数，使用了STM32F407VET6的硬件定时器（TIM7）来实现精确的延时。下面是对这段代码的逐行解释：

void delay_us(uint16_t us):

这是一个名为 delay_us 的函数，它接受一个 uint16_t 类型的参数 us，表示要延时的微秒数。
uint16_t differ=0xffff-us-5;:

计算定时器的初始值 differ。0xffff 是16位定时器的最大值，减去要延时的微秒数 us，再减去5。减去5是为了补偿定时器启动和停止时的额外时间开销，确保延时的准确性。
HAL_TIM_Base_Start(&htim7);:

启动定时器7（htim7）。HAL_TIM_Base_Start 是HAL库提供的函数，用于启动一个定时器的基本功能.
__HAL_TIM_SetCounter(&htim7,differ);:

设置定时器7的计数器值为 differ。__HAL_TIM_SetCounter 是HAL库提供的宏，用于设置定时器的计数器值。
while(differ < 0xffff-5):

这是一个循环，用于等待定时器计数器达到 0xffff-5。0xffff-5 是为了确保延时的准确性，减去5是为了补偿定时器启动和停止时的额外时间开销.
{ differ = __HAL_TIM_GetCounter(&htim7); }:

在循环中，不断读取定时器7的当前计数器值，并将其赋值给 differ。__HAL_TIM_GetCounter 是HAL库提供的宏，用于获取定时器的当前计数器值。
HAL_TIM_Base_Stop(&htim7);:

停止定时器7。HAL_TIM_Base_Stop 是HAL库提供的函数，用于停止一个定时器的基本功能.
这个函数通过设置定时器的初始值，然后等待定时器计数器达到一个特定的值来实现延时。使用硬件定时器可以实现比软件延时更精确和稳定的延时效果。
**/
 
//void delay_us(uint16_t us)
//{
//	
//	uint16_t differ=0xffff-us-5; 

//	HAL_TIM_Base_Start(&htim7);
//	__HAL_TIM_SetCounter(&htim7,differ); 
//	while(differ < 0xffff-5) 
//	{ 
//		differ = __HAL_TIM_GetCounter(&htim7);
//		
//	} 
//	HAL_TIM_Base_Stop(&htim7);
//	
//}


///**
//  * @brief  毫秒级延时
//  * @param  xms 延时时长，范围：0~4294967295
//  * @retval 无
//  */
//void delay_ms(uint32_t xms)
//{
//	while(xms--)
//	{
//		delay_us(1000);
//	}
//}
// 
///**
//  * @brief  秒级延时
//  * @param  xs 延时时长，范围：0~4294967295
//  * @retval 无
//  */
//void delay_s(uint32_t xs)
//{
//	while(xs--)
//	{
//		delay_ms(1000);
//	}
//}

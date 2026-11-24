/**
 ******************************************************************************
 * @file    Delay.c
 * @brief   三种延时实现：微秒级忙等、毫秒级 SysTick、粗略毫秒循环
 * @details
 *   1. delay_us()  : 调 DriverLib 的 delay_cycles(sx*8)，纯忙等，精度取决于主频；
 *   2. delay_ms()  : 置初值后靠 SysTick 中断递减，CPU 期间可继续跑别的循环，
 *                    必须让 SysConfig 打开 SysTick（1 ms）并保证这个中断服务函数
 *                    是工程里唯一的一份，否则会出现“延时变长/变短”；
 *   3. delayms()   : 100 次 delay_cycles(320*ms) 的粗糙实现，保留给老代码调用。
 * @note
 *   delay_us 里的系数 8 是按本板 80 MHz 主频实测标定的经验值，换主频必须重新标定，
 *   否则所有微秒级时序（OLED 软件 I2C、单总线传感器等）都会失配。
 ******************************************************************************
 */

#include "ti_msp_dl_config.h"

//void delay_ms(unsigned int sx)
//{
//		sx=sx*32000;
//		delay_cycles(sx);
//}

/**
 * @brief  微秒级忙等延时
 * @param  sx 延时时长（微秒），只保证“不少于”这个时间，不保证精确
 * @note   在中断里调用会拉长控制周期，循迹中断里应避免使用。
 */
void delay_us(unsigned int sx)
{
		sx=sx*8;
		delay_cycles(sx);
}

volatile unsigned int delay_times = 0;

//搭配滴答定时器实现的精确ms延时
/**
 * @brief  毫秒级延时（基于 SysTick 中断递减计数）
 * @param  ms 延时时长（毫秒）
 * @note   delay_times 必须是 volatile，否则编译器优化后 while 可能直接跳过。
 */
void delay_ms(unsigned int ms)
{
    delay_times = ms;
    while( delay_times != 0 );
}
/* 粗糙毫秒延时（忙等）：精度差、CPU 占用高，仅为兼容旧代码保留 */
void delayms(int ms)
{
	for(int i=0;i<100;i++)
	delay_cycles(320*ms);
}

//滴答定时器中断服务函数
/**
 * @brief  SysTick 中断服务：给 delay_ms 提供 1 ms 时基
 * @note   该函数名由启动文件固定，工程内只能有一处定义。
 */
void SysTick_Handler(void)
{
    if( delay_times != 0 )
    {
        delay_times--;
    }
}

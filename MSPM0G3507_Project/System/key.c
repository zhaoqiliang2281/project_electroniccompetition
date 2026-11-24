/**
 ******************************************************************************
 * @file    key.c
 * @brief   按键与蜂鸣器：两种按键读取风格 + 松手触发标志
 * @details
 *   【电平约定】按键一端接 GPIO、另一端接地，按下读到 0、松开读到 1。
 *   【两种读法】
 *     1) Key_GetNumXxx()：阻塞式。检测到按下 -> 延时 10 ms 消抖 -> 死等松手 ->
 *        再延时 10 ms -> 返回 1。适合主循环中“按一次改一次参数”的场合，
 *        但按下期间程序会卡住，不能在中断或高频任务里调用。
 *     2) key_scan()：非阻塞。20 ms 调一次，靠“上电状态 vs 当前状态”的边沿
 *        判断松手事件，只置标志（key1_flag/key2_flag），由业务代码自行消费，
 *        适合放定时中断里做真实时的人机交互。
 *   【引脚】PA30 = 圈数键、PA31 = 总开关、PB10 = 模式、PB17/PB18/PB19 备用；
 *           PA2 = 蜂鸣器（输出 1 响、0 停，高电平有效）。
 * @note
 *   1. 每个按键读数都写了“直到松手”的 while，若按键卡死会让程序停在这里，
 *      调试时若发现 OLED 不刷新，先查按键是否被按住；
 *   2. key_scan 的边沿判据是“上一次=0(按下) 且 当前=1(松开)”，即松手才置位，
 *      这是为了避开手指抖动导致的一次按键触发多次；
 *   3. 文件内大段 ////注释 是早期 3 键版本的写法，保留作为参考，不参与编译。
 ******************************************************************************
 */

#include "All_include.h"

/**
 * @brief  阻塞式读取 PA30（目标圈数键）
 * @retval 1=本次确实按下并已松手；0=没按
 * @note   内部“等待松手”的 while 会占住 CPU，属于典型的阻塞式按键写法。
 *         其余 Key_GetNum1/10/17/18/19 结构完全一致，只是引脚不同，不再重复注释。
 */
uint8_t Key_GetNum(void)
{
	uint8_t KeyNum=0;
	if(DL_GPIO_readPins(GPIOA, DL_GPIO_PIN_30) == 0)
	{
		delay_ms(10);
		while(DL_GPIO_readPins(GPIOA, DL_GPIO_PIN_30) == 0);//直到松手
		delay_ms(10);
		KeyNum = 1;//返回值为1
	}
	return KeyNum;
}
uint8_t Key_GetNum1(void)  
{
	uint8_t KeyNum=0;
	if(DL_GPIO_readPins(GPIOA, DL_GPIO_PIN_31) == 0)
	{
		delay_ms(10);
		while(DL_GPIO_readPins(GPIOA, DL_GPIO_PIN_31) == 0);//直到松手
		delay_ms(10);
		KeyNum = 1;//返回值为1
	}

	return KeyNum;
}

uint8_t Key_GetNum10(void)
{
	uint8_t KeyNum=0;
	if(DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_10) == 0)
	{
		delay_ms(10);
		while(DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_10) == 0);//直到松手
		delay_ms(10);
		KeyNum = 1;//返回值为1
	}
	return KeyNum;
}

/* 备用按键 PB17：结构与 PA30 相同，当前未被主循环使用 */
uint8_t Key_GetNum17(void)
{
	uint8_t KeyNum=0;

	if(DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_17) == 0)
	{
		delay_ms(10);
		while(DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_17) == 0);//直到松手
		delay_ms(10);
		KeyNum = 1;//返回值为1
	}
	return KeyNum;
}
/* 备用按键 PB18：模式 1 里用作“基础速度 +10” */
uint8_t Key_GetNum18(void)
{
	uint8_t KeyNum=0;
	if(DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_18) == 0)
	{
		delay_ms(10);
		while(DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_18) == 0);//直到松手
		delay_ms(10);
		KeyNum = 1;//返回值为
	}

	return KeyNum;
}
/* 备用按键 PB19：模式 1 里用作“基础速度 -10” */
uint8_t Key_GetNum19(void)
{
	uint8_t KeyNum=0;
	if(DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_19) == 0)
	{
		delay_ms(10);
		while(DL_GPIO_readPins(GPIOB, DL_GPIO_PIN_19) == 0);//直到松手
		delay_ms(10);
		KeyNum = 1;//返回值为1
	}
	return KeyNum;
}
////uint16_t sys_KEY_scan(void)
////{
////  if(DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY0_PIN) == 0 ||DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY1_PIN) == 0 ||DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY2_PIN) == 0 ) {
////    delay_ms(20);
////    if(DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY0_PIN) == 0) {
////      while(DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY0_PIN) == 0) {}
////      return 1;
////    }
////    if(DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY1_PIN) == 0) {
////      while(DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY1_PIN) == 0) {}
////      return 2;
////    }
////    if(DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY2_PIN) == 0) {
////      while(DL_GPIO_readPins(GPIO_KEY_PORT, GPIO_KEY_KEY2_PIN) == 0) {}
////      return 3;
////    }
////  }
////  return 0;
////}
//==================================按键变量定义区===================================================
uint8 key1_state = 1;                                // 按键动作状态
uint8 key2_state = 1;                                // 按键动作状态
//uint8 key3_state = 1;                                // 按键动作状态
//uint8 key4_state = 1;                                // 按键动作状态

uint8 key1_state_last = 0;                           // 上一次按键动作状态
uint8 key2_state_last = 0;                           // 上一次按键动作状态
//uint8 key3_state_last = 0;                           // 上一次按键动作状态
//uint8 key4_state_last = 0;                           // 上一次按键动作状态

//按键开关标志
uint8 key1_flag;
uint8 key2_flag;
//uint8 key3_flag;
//uint8 key4_flag;

/**
 * @brief  非阻塞按键扫描：检测“松手”上升沿并置标志
 * @note   1. 需按固定周期调用（建议 10 ~ 20 ms），调用太慢会漏掉短按；
 *         2. key1_state/key2_state 保存“上一次采到的原始电平”，
 *            用 (当前==1 && 上次==0) 判松手，天然带一次消抖效果；
 *         3. 标志置 1 后由调用者清零，key_scan 自身不清，避免漏事件。
 */
void key_scan(void)//按键扫描
{
    //保存按键状态
    key1_state_last = key1_state;
    key2_state_last = key2_state;
//    key3_state_last = key3_state;
//    key4_state_last = key4_state;

    //读取当前按键状态
    key1_state = DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_30);
    key2_state = DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_31);
//    key3_state = DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_1);
//    key4_state = DL_GPIO_readPins(GPIOA,DL_GPIO_PIN_0);

    //检测到按键按下之后  并放开置位标志位
    if(key1_state && !key1_state_last)   {key1_flag = 1;}//按键按下完成对应标志位会置1
    if(key2_state && !key2_state_last)   {key2_flag = 1;}
//    if(key3_state && !key3_state_last)   {key3_flag = 1;}
//    if(key4_state && !key4_state_last)   {key4_flag = 1;}

}
/**
 * @brief  蜂鸣器响 / 停（PA2，高电平有效）
 * @note   若蜂鸣器模块是低电平触发的无源模块，把 set/clear 对调即可。
 */
void Buzzer_ON(void)
{
	DL_GPIO_setPins(GPIOA,DL_GPIO_PIN_2);//蜂鸣器开
}
void Buzzer_OFF(void)
{
	DL_GPIO_clearPins(GPIOA, DL_GPIO_PIN_2);	//蜂鸣器关
}


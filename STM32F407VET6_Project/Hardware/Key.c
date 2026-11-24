/**
 ******************************************************************************
 * @file    Key.c
 * @brief   三键输入：20 ms 消抖 + 松手事件
 * @details
 *   按键接法：PD1 / PD0 / PD12，按下读 0（低电平有效）。
 *   两种读取方式：
 *     Key_Tick()   在 1 ms 定时中断里累加计数，每 20 ms 采样一次，
 *                  用“上次有键按下、这次全部松开”判出松手事件，写入 Key_num；
 *     Key_GetNum() 取走事件并清零，供主循环使用（非阻塞）。
 *   事件编号：1 = PD0 键、2 = PD1 键、3 = PD12 键（对应三个实体按键）。
 * @note
 *   1. 消抖在时间维度上完成（20 ms 才采一次），不需要额外延时，中断里很安全；
 *   2. Key_GetState() 会被 Key_Tick 调用，不宜在其中加 HAL_Delay；
 *   3. 文件里保留的阻塞式 Key_GetNum 旧实现已注释，可用于对比两种写法。
 ******************************************************************************
 */

#include "Key.h"

#define  	GPIOX	GPIOD
#define  	GPIOY	GPIOD

#define		PIN1	GPIO_PIN_0	//407上板子对应按键1
#define		PIN2	GPIO_PIN_1	//407上板子对应按键2


uint8_t Key_num;

/**
 * @brief  取出并清零按键事件（非阻塞）
 * @retval 0=无按键，1/2/3=对应按键刚松开
 * @note   事件是“一次性”的，取走后 Key_num 立即清零，避免一次按键触发多次动作。
 */
uint8_t Key_GetNum(void)
{
	uint8_t Temp;
	Temp = Key_num;
	Key_num = 0;
	return Temp;
}
//uint8_t Key_GetNum(void)
//{
//	uint8_t KeyNum=0;
//	if(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0) == 0)
//	{
//		HAL_Delay(10);
//		while(HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_0) == 0);//直到松手
//		HAL_Delay(10);
//		KeyNum = 1;//返回值为1
//	}
//	return KeyNum;
//}
/**
 * @brief  读取当前按键电平组合
 * @retval 0=全松开，1/2/3=对应按键正被按住
 * @note   只看电平不看边沿，供 Key_Tick 做“按住->松开”的判断。
 */
uint8_t Key_GetState(void)
{
	uint8_t KeyNum = 0;
	if(HAL_GPIO_ReadPin(GPIOX,PIN2) == 0)//读取输入(如果为0)
	{
		KeyNum = 1;	//返回值为1
	}
	if(HAL_GPIO_ReadPin(GPIOY,PIN1) == 0)
	{
		KeyNum = 2;	//返回值为2
	}
	if(HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_12) == 0)
	{
		KeyNum = 3;	//返回值为3
	}
	return KeyNum;
}


/**
 * @brief  按键扫描（放在 1 ms 定时中断里，内部 20 ms 分频）
 * @note   CurrState/PrevState 用 static 保存上一次采样，天然实现去抖；
 *         若发现按键偶尔不响应，先把 20 调小（如 10）再试。
 */
void Key_Tick(void)	//定时器扫描
{
	static uint8_t Count;
	static uint8_t CurrState, PrevState;
	
	Count ++;
	if (Count >= 20)	//设置20ms进一次中断
	{
		Count = 0;
		
		PrevState = CurrState;	//使用按键状态传递
		CurrState = Key_GetState();
		
		if (CurrState == 0 && PrevState != 0)	//判断上一次按键按下且这次按键松开
		{
			Key_num = PrevState;
		}
	}
}

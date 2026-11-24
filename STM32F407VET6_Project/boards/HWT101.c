/**
 ******************************************************************************
 * @file    HWT101.c
 * @brief   维特智能 HWT101 单轴陀螺仪驱动：逐字节喂入 + 状态机解帧
 * @details
 *   协议：11 字节一帧，帧头 0x55 + 类型字节，0x53 表示“角度输出”。
 *   帧结构 (0x53)：[0]=0x55 [1]=0x53 [2..3]=Roll [4..5]=Pitch [6..7]=Yaw
 *                 [8..9]=版本 [10]=校验和（前 10 字节累加的低 8 位）。
 *   角度换算：raw / 32768 * 180（单位：度）。
 *   接收方式：USART6 每收到 1 字节就调用 HWT101_ReceiveData，
 *   由 RxState 状态机完成“找包头 -> 确认类型 -> 收满 11 字节 -> 校验 -> 解析”。
 * @warning
 *   main.c 中启动接收的语句被注释，g_usart6_receivedata 恒为 0，
 *   所以 Roll/Pitch/Yaw 一直为 0，chassis_behaviour 的车头锁定因此无效。
 *   要用陀螺仪闭环，必须打开那一行 HAL_UART_Receive_IT(&huart6,...)。
 ******************************************************************************
 */

#include "HWT101.h"

static uint8_t RxBuffer[11];/*接收数据数组*/
static volatile uint8_t RxState = 0;/*接收状态标志位*/
static uint8_t RxIndex = 0;/*接受数组索引*/

float Roll,Pitch,Yaw;/*角度信息，如果只需要整数可以改为整数类型*/

/**
 * @brief       数据包处理函数
 * @param       串口接收的数据RxData
 * @retval      无
 */
 
/**
 * @brief  单字节解帧状态机（在串口接收中断/回调里逐字节调用）
 * @param  RxData 刚收到的 1 个字节
 * @note   RxState: 0=等 0x55、1=等类型字节、2=收剩余 9 字节。
 *         校验失败不会报错，只是丢弃本帧并从状态 0 重新找包头。
 */
void HWT101_ReceiveData(uint8_t RxData)
{
	uint8_t i,sum=0;
	if (RxState == 0)	//等待包头
	{
		if (RxData == 0x55)	//收到包头
		{
			RxBuffer[RxIndex] = RxData;
			RxState = 1;
			RxIndex = 1;	//进入下一状态
		}
	}
	else if (RxState == 1)
	{
		if (RxData == 0x53)	/*判断数据内容，修改这里可以改变要读的数据内容，0x53为角度输出*/
		{
			RxBuffer[RxIndex] = RxData;
			RxState = 2;
			RxIndex = 2;	//进入下一状态
		}
	}
	else if (RxState == 2)	//接收数据
	{
		RxBuffer[RxIndex++] = RxData;
		if(RxIndex == 11)	//接收完成
		{
			for(i=0;i<10;i++)
			{
				sum = sum + RxBuffer[i];	//计算校验和
			}
			if(sum == RxBuffer[10])		//校验成功
			{
				/*计算数据，根据数据内容选择对应的计算公式*/
				Roll = ((int16_t) ((int16_t) RxBuffer[3] << 8 | (int16_t) RxBuffer[2])) / 32768.0f * 180.0f;
				Pitch = ((int16_t) ((int16_t) RxBuffer[5] << 8 | (int16_t) RxBuffer[4])) / 32768.0f * 180.0f;
				Yaw = ((int16_t) ((int16_t) RxBuffer[7] << 8 | (int16_t) RxBuffer[6])) / 32768.0f * 180.0f;
			}
			RxState = 0;
			RxIndex = 0;	//读取完成，回到最初状态，等待包头
		}
	}
}

/**
 * @brief  发送零位/校准指令帧（FF AA 52），把当前姿态设为航向零点
 * @note   调用后车头方向即“0 度”，发完无需应答；指令含义见维特智能手册。
 */
void HWT101_zero(void)
{
	 uint8_t data[3] = {0xFF, 0xAA,0x52};
    HAL_StatusTypeDef status = HAL_UART_Transmit(&huart6, data, 3, 0xFFF);
}

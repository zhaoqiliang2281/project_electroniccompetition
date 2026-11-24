/**
 ******************************************************************************
 * @file    JY901S.h
 * @brief   JY901S 姿态传感器接口声明
 * @note    文件末尾保留了 SysConfig 中该串口的配置片段（引脚/波特率/DMA 长度），
 *          便于换工程时对照，不影响编译。
 ******************************************************************************
 */

#ifndef __JY901S_H
#define __JY901S_H

#include "ti_msp_dl_config.h"
#include <stdint.h>
#include <stdbool.h>

// 接收数组最大长度
#define RX_LEN_MAX  (33*2)  // 一个数据包33字节，长度66刚好接收两个包

// 外部变量
extern volatile uint8_t rxData[RX_LEN_MAX];

// 初始化 DMA 接收 JY901S 数据
void JY901S_Init(void);

// 更新并获取角度值
bool JY901S_Update(void);

// 获取角度值（单位：°）
float JY901S_GetRoll(void);
float  JY901S_GetPitch(void);
float  JY901S_GetYaw(void);

void JY901S_Zero(void);



#endif

//UART4.targetBaudRate                       = 115200;
//UART4.$name                                = "UART_JY901S";
//UART4.enabledDMARXTriggers                 = "DL_UART_DMA_INTERRUPT_RX";
//UART4.peripheral.$assign                   = "UART3";
//UART4.peripheral.rxPin.$assign             = "PA13";
//UART4.peripheral.txPin.$assign             = "PA14";
//UART4.txPinConfig.direction                = scripting.forceWrite("OUTPUT");
//UART4.txPinConfig.hideOutputInversion      = scripting.forceWrite(false);
//UART4.txPinConfig.onlyInternalResistor     = scripting.forceWrite(false);
//UART4.txPinConfig.passedPeripheralType     = scripting.forceWrite("Digital");
//UART4.txPinConfig.$name                    = "ti_driverlib_gpio_GPIOPinGeneric8";
//UART4.rxPinConfig.hideOutputInversion      = scripting.forceWrite(false);
//UART4.rxPinConfig.onlyInternalResistor     = scripting.forceWrite(false);
//UART4.rxPinConfig.passedPeripheralType     = scripting.forceWrite("Digital");
//UART4.rxPinConfig.$name                    = "ti_driverlib_gpio_GPIOPinGeneric9";
//UART4.DMA_CHANNEL_RX.srcLength             = "BYTE";
//UART4.DMA_CHANNEL_RX.dstLength             = "BYTE";
//UART4.DMA_CHANNEL_RX.$name                 = "DMA_UART_JY901S";
//UART4.DMA_CHANNEL_RX.configureTransferSize = true;
//UART4.DMA_CHANNEL_RX.transferSize          = 66;
//UART4.DMA_CHANNEL_RX.transferMode          = "FULL_CH_REPEAT_SINGLE";
//UART4.DMA_CHANNEL_RX.destIncrement         = "INCREMENT";
//UART4.DMA_CHANNEL_RX.peripheral.$assign    = "DMA_CH0";

		//陀螺仪读取欧拉角测试
//		if (JY901S_Update()) //陀螺仪数据读取
//		{
//			int roll  = (int)(JY901S_GetRoll());   
//			int pitch = (int)(JY901S_GetPitch());
//			int yaw   = (int)(JY901S_GetYaw());
//			OLED_ShowSignedNum(1, 1, roll,3);
//			OLED_ShowSignedNum(1, 6, pitch, 3);
//			OLED_ShowSignedNum(1, 11, yaw, 3);
////			printf("%d,%d,%d\n\r",roll,pitch,yaw);//上位机看值
////			delay_ms(500);
//			
////			rpy[0] = roll;
////			rpy[1] = pitch;
////			rpy[2] = yaw;
////			Serial_SendArray(rpy,3);
//		}

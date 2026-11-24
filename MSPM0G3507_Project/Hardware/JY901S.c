/**
 ******************************************************************************
 * @file    JY901S.c
 * @brief   JY901S（维特智能）九轴姿态传感器驱动：UART3 + DMA 收帧 + 欧拉角解析
 * @details
 *   数据链路：UART3(PA13/PA14, 115200) -> DMA 通道 0 -> rxData[66]（两帧缓冲）。
 *   协议要点：
 *     - 每帧 11 字节，帧头 0x55 + 类型字节；角度帧类型为 0x51；
 *     - 角度数据为 int16，满量程 32768 对应 180 度，故 角度 = raw * 180 / 32768；
 *     - 一帧内：Roll=X(偏移2/3)、Pitch=Y(偏移4/5)、Yaw=Z(偏移6/7)，低字节在前。
 *   解析策略：先在两帧缓冲里找 0x55 0x51 定出帧头位置，之后按 11 字节步进取
 *   第 3 个角度帧，避免每帧都重新搜索；一旦首字节不再是 0x55 就丢帧重同步。
 * @note
 *   1. DMA 配置在 SysConfig（UART_JY901S / DMA_CH0，FULL_CH_REPEAT_SINGLE，长度 66）；
 *   2. 本版 main.c 未调用 JY901S_Update()，姿态数据只做预留（车体转向未用陀螺仪闭环）；
 *   3. rxData 为 volatile，读取顺序不要编译器重排，必要时加临界区保护。
 ******************************************************************************
 */

#include "JY901S.h"
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "Serial.h"

volatile uint8_t rxData[RX_LEN_MAX];

static bool hasFoundFrameHead = false;
static uint8_t FrameHeadIndex[3] = {0};

static float angleX_float = 0;
static float angleY_float = 0;
static float angleZ_float = 0;

/**
 * @brief  初始化 DMA 接收：源=UART3 RXDATA、目的=rxData[0]，长度为 SysConfig 配置的 66
 * @note   必须先启动 DMA 再让传感器持续发数据，否则会漏掉首帧导致同步偏移。
 */
void JY901S_Init(void)
{
    // 配置DMA接收数据
    DL_DMA_setSrcAddr(DMA, DMA_UART_JY901S_CHAN_ID, (uint32_t)(&UART_JY901S_INST->RXDATA));
    DL_DMA_setDestAddr(DMA, DMA_UART_JY901S_CHAN_ID, (uint32_t)&rxData[0]);
    DL_DMA_enableChannel(DMA, DMA_UART_JY901S_CHAN_ID);
    while (DL_DMA_isChannelEnabled(DMA, DMA_UART_JY901S_CHAN_ID) == false);
}

/**
 * @brief  更新并解析一帧角度数据
 * @retval true=本次成功刷新 Roll/Pitch/Yaw；false=帧未就绪或丢帧，需调用者丢弃
 * @note   返回 false 时调用方不要直接使用 GetRoll() 等值（那是上一次的旧数据）。
 */
bool JY901S_Update(void)
{
    if (!hasFoundFrameHead) {
        for (int i = 0; i < RX_LEN_MAX / 2; i++) {
            if (rxData[i] == 0x55 && rxData[i + 1] == 0x51) {
                FrameHeadIndex[0] = i + 11 * 0;
                FrameHeadIndex[1] = i + 11 * 1;
                FrameHeadIndex[2] = i + 11 * 2;
                hasFoundFrameHead = true;
                break;
            }
        }
    } else {
        if (rxData[FrameHeadIndex[0]] != 0x55) {
            hasFoundFrameHead = false;
            return false;
        }

        // 解析角度数据
        int16_t angleX_int16 = rxData[FrameHeadIndex[2]+2] | (rxData[FrameHeadIndex[2]+3] << 8);
        int16_t angleY_int16 = rxData[FrameHeadIndex[2]+4] | (rxData[FrameHeadIndex[2]+5] << 8);
        int16_t angleZ_int16 = rxData[FrameHeadIndex[2]+6] | (rxData[FrameHeadIndex[2]+7] << 8);

        angleX_float = (float)angleX_int16 * 180.0f / 32768.0f;
        angleY_float = (float)angleY_int16 * 180.0f / 32768.0f;
        angleZ_float = (float)angleZ_int16 * 180.0f / 32768.0f;

        return true;
    }

    return false;
}

/* 取最近一次解析出的欧拉角（单位：度） */
float JY901S_GetRoll(void)
{
    return angleX_float;
}

/* Pitch：俯仰角，单位度 */
float JY901S_GetPitch(void)
{
    return angleY_float;
}

/* Yaw：航向角，单位度（-180 ~ +180） */
float JY901S_GetYaw(void)
{
    return angleZ_float;
}

void JY901S_Serial_SendByte(uint8_t Byte)
{
	DL_UART_transmitDataBlocking(UART3, Byte);//将字节数据写入数据寄存器，写入后USART自动生成时序波形
}
void JY901S_Serial_SendArray(uint8_t *Array, uint16_t Length)	//16位
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		JY901S_Serial_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}
/**
 * @brief  向传感器发送零位/校准类指令帧（FF AA 52）
 * @note   指令具体含义见维特智能手册；发送后需等待传感器回零完成再继续采数。
 */
void JY901S_Zero(void)
{
	 uint8_t data[3] = {0xFF, 0xAA,0x52};
	 JY901S_Serial_SendArray(data,3);
}

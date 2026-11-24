/**
 ******************************************************************************
 * @file    Serial2.c
 * @brief   MaixCAM 视觉坐标解析（ASCII 协议，DMA + IDLE 不定长接收）
 * @details
 *   协议：一帧形如 "x170y115"（也可能是 "x=170,y=115" 之类的变体，本解析只认字母 x/y），
 *   解析规则：找到 'x' 后 atoi 得 x 值，再在其后找 'y' 取 y 值；
 *   x=640 且 y=480 是双方约定的“丢失目标”哨兵。
 *   缓冲区 buffer3[256] 由 DMA 常驻填充，IDLE 中断算出本帧长度。
 * @warning
 *   1. 解析失败（如丢 'y' 或首字符异常）时函数直接 return，
 *      跳过了末尾的 memset + HAL_UART_Receive_DMA —— DMA 不会重启，
 *      视觉数据流会就此“停摆”。稳妥做法是把清除与重启放到函数末尾统一执行；
 *   2. 缓冲是 256 字节但 IDLE 中断里重启长度是 16（见 stm32f4xx_it.c），
 *      长帧会被截断，两处必须一起改；
 *   3. 解析依赖 atoi，遇到 "x12y34abc" 这类脏数据会静默取到 12/34，调试时注意看原始帧。
 ******************************************************************************
 */

#include "Serial2.h"

/*
串口分配：
串口一:K230 扫码
串口二:K230 识别颜色小旗
串口三:maxcan  识别
串口四:串口屏
串口六:陀螺仪
*/
 
// 接收数据的缓冲区
#define RECEIVE_BUFFER_SIZE 255
char RECS[RECEIVE_BUFFER_SIZE];
unsigned char i = 0;


//串口三
uint8_t USAR3_ReceiveData_len=0;
int x3 = 0;
int y3 = 0;

uint8_t buffer3[256];
/**************************MaixCAM****************************/

/**
 * @brief  解析 DMA 缓冲中的视觉坐标并重新启动接收（主循环每圈调用一次）
 * @note   解析成功则更新全局 x3/y3，供 Cloud_platform_PID_Scan 使用；
 *         调用频率高（每圈一次）但耗时很短，属于“轮询式收帧”。
 */
void MaixCAM_data_manage(void) 
{
		const char *str_buffer = (const char *)buffer3;
		char *x_start = strchr(str_buffer, 'x');
		if (x_start != NULL) 
		{
			x_start++; // 跳过 'x'
			int x_value = atoi(x_start);
			// 简单的错误处理：如果转换结果为 0 且原字符串不是 "0"，可能转换失败
			if (x_value == 0 && strcmp(x_start, "0") != 0) 
			{
				return;
			}

			// 查找 y 的位置
			char *y_start = strchr(x_start, 'y');
			if (y_start != NULL) 
			{
				y_start++; // 跳过 'y'
				int y_value = atoi(y_start);
				// 简单的错误处理：如果转换结果为 0 且原字符串不是 "0"，可能转换失败
				if (y_value == 0 && strcmp(y_start, "0") != 0) 
				{
					return;
				}

				// 处理解析到的数据，这里可以根据需求修改
				x3 = x_value;
				y3 = y_value;
			}
		}
		memset(buffer3, 0, sizeof(buffer3));
		HAL_UART_Receive_DMA(&huart3, buffer3, 256);
}


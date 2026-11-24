/**
 ******************************************************************************
 * @file    Serial.c
 * @brief   串口收发封装（UART0 ~ UART3，115200-8-N-1）
 * @details
 *   - UART0：调试口（PA10/PA11），printf 重定向到这里；
 *   - UART1/2：通用串口，预留与外部模块通信；
 *   - UART3：JY901S 陀螺仪，实际数据由 DMA 搬运（本文件的收/发缓冲只做备用）。
 *   本文件里的 uart_rece/uart_send 缓冲是 50 字节的“发送/接收草稿区”，
 *   真正的协议解析请另建模块，不要把业务解析写在这层。
 * @note
 *   printf 重定向依赖微库（Keil 里勾选 Use MicroLIB），否则 fputc 不会被调用，
 *   现象是“printf 没输出但程序也不报错”。
 ******************************************************************************
 */

#include "All_include.h"

/* 每个串口一对收发缓冲：接收用于中断存字节，发送用于组包后一次性输出 */
char UART_0_uart_rece[50];
char UART_0_uart_send[50];

char UART_1_uart_rece[50];
char UART_1_uart_send[50];

char UART_2_uart_rece[50];
char UART_2_uart_send[50];

char UART_3_uart_rece[50];
char UART_3_uart_send[50];
/**
 * @brief       用于指定串口发送指定的字符串内容
 * 
 * @param[i]    uart    指定的串口寄存器基地址（串口号），类型为 UART_Regs*
 * @param[ii]   str     要发送的字符串内容，以空字符 '\0' 结尾的字符数组指针
 * 
 * @retval      无返回值
 */
/**
 * @brief  指定串口发送字符串（阻塞式，逐字节等待发送完成）
 * @param  uart 串口寄存器基址，如 UART0 / UART1 / UART2 / UART3
 * @param  str  以 '\0' 结尾的字符串
 * @note   逐字节阻塞发送，长字符串会明显占用 CPU；大批量数据建议开 DMA。
 */
void uart_sendString(UART_Regs *uart, char* str) 
{
	while(*str != 0)
	{
		DL_UART_transmitDataBlocking(uart, *str);
		str++;
	}
}
/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
/* 向 UART0 发送 1 字节（阻塞） */
void Serial_SendByte(uint8_t Byte)
{
	DL_UART_transmitDataBlocking(UART0, Byte);//将字节数据写入数据寄存器，写入后USART自动生成时序波形
}

/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
/* 向 UART0 发送字节数组 */
void Serial_SendArray(uint8_t *Array, uint16_t Length)	//16位
{
	uint16_t i;
	for (i = 0; i < Length; i ++)		//遍历数组
	{
		Serial_SendByte(Array[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：串口发送一个字符串
  * 参    数：String 要发送字符串的首地址
  * 返 回 值：无
  */
/* 向 UART0 发送字符串 */
void Serial_SendString(char *String)	//利用指针传递数组首地址
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i ++)//遍历字符数组（字符串），遇到字符串(结束标志位)0后停止
	{
		Serial_SendByte(String[i]);		//依次调用Serial_SendByte发送每个字节数据
	}
}

/**
  * 函    数：次方函数（内部使用）
  * 返 回 值：返回值等于X的Y次方
  */
uint32_t Serial_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;	//设置结果初值为1
	while (Y --)			//执行Y次
	{
		Result *= X;		//将Result乘X的Y次 就是X的Y次方
	}
	return Result;	//将X累乘到结果返回出去
}

/**
  * 函    数：串口发送数字
  * 参    数：Number 要发送的数字，范围：0~4294967295
  * 参    数：Length 要发送数字的长度，范围：0~10
  * 返 回 值：无
  */
/* 向 UART0 发送定长数字（不足 Length 位补前导 0），常用作上位机调试输出 */
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用Serial_SendByte发送每位数字
	}
}


/**
 * @brief  printf 底层重定向：把每个字符送到 UART0
 * @note   需要工程勾选 Use MicroLIB；若改用 DMA 发送，注意不要与其它外设抢通道。
 */
int fputc(int ch, FILE *f)
{
//	Serial_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	DL_UART_transmitDataBlocking(UART0, ch);//发送回来
	return ch;
}

///**
//  * 函    数：自己封装的prinf函数
//  * 参    数：format 格式化字符串
//  * 参    数：... 可变的参数列表
//  * 返 回 值：无
//  */
//void Serial_Printf(char *format, ...)
//{
//	char String[100];				//定义字符数组
//	va_list arg;					//定义可变参数列表数据类型的变量arg
//	va_start(arg, format);			//从format开始，接收参数列表到arg变量
//	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
//	va_end(arg);					//结束变量arg
//	Serial_SendString(String);		//串口发送字符数组（字符串）
//}

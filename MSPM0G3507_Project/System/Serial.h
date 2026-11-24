/**
 ******************************************************************************
 * @file    Serial.h
 * @brief   串口收发接口（UART0 调试口 + 通用发送函数）
 * @note    发送函数默认都走 UART0；要用别的串口请用 uart_sendString(UARTx, str)。
 ******************************************************************************
 */

#ifndef __SERIAL_H__
#define __SERIAL_H__

#include "All_include.h"

extern char UART_0_uart_rece[50];
extern char UART_0_uart_send[50];
	
extern char UART_1_uart_rece[50];
extern char UART_1_uart_send[50];

extern char UART_2_uart_rece[50];
extern char UART_2_uart_send[50];

extern char UART_3_uart_rece[50];
extern char UART_3_uart_send[50];

void uart_sendString(UART_Regs *uart, char* str);
	
void Serial_SendByte(uint8_t Byte);	
void Serial_SendString(char *String);	
void Serial_SendNumber(uint32_t Number, uint8_t Length);	
void Serial_SendArray(uint8_t *Array, uint16_t Length);

#endif

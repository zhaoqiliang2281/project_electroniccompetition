#include "Serial.h"

char Serial_RxPacket[100];				//定义接收数据包数组，数据包格式"@MSG\r\n"
uint8_t Serial_RxFlag;					//定义接收数据包标志位
//清零数组函数memset(RxData,0,sizeof(RxData));
//	snprintf(send_buf, sizeof(send_buf), "RedflagX");//转进数组里
/**
  * 函    数：串口发送一个字节
  * 参    数：Byte 要发送的一个字节
  * 返 回 值：无
  */
void Serial_SendByte(uint8_t Byte)
{
	 HAL_UART_Transmit(&huart1, &Byte, 1, HAL_MAX_DELAY);
}
/**
  * 函    数：串口发送一个数组
  * 参    数：Array 要发送数组的首地址
  * 参    数：Length 要发送数组的长度
  * 返 回 值：无
  */
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
void Serial_SendNumber(uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i ++)		//根据数字长度遍历数字的每一位
	{
		Serial_SendByte(Number / Serial_Pow(10, Length - i - 1) % 10 + '0');	//依次调用Serial_SendByte发送每位数字
	}	//
}

/**
  * 函    数：使用printf需要重定向的底层函数
  * 参    数：保持原始格式即可，无需变动
  * 返 回 值：保持原始格式即可，无需变动
  */
int fputc(int ch, FILE *f)
{
	Serial_SendByte(ch);			//将printf的底层重定向到自己的发送字节函数
	return ch;
}

/**
  * 函    数：自己封装的prinf函数
  * 参    数：format 格式化字符串
  * 参    数：... 可变的参数列表
  * 返 回 值：无
  */
void Serial_Printf(char *format, ...)
{
	char String[100];				//定义字符数组
	va_list arg;					//定义可变参数列表数据类型的变量arg
	va_start(arg, format);			//从format开始，接收参数列表到arg变量
	vsprintf(String, format, arg);	//使用vsprintf打印格式化字符串和参数列表到字符数组中
	va_end(arg);					//结束变量arg
	Serial_SendString(String);		//串口发送字符数组（字符串）
}

/*
//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) //接收中断函数
//{
//	if (huart == &huart1)
//	{
//		Serial_RxFlag=1;//已接收标志位，说明已经接收完一次
//		HAL_UART_Receive_IT(&huart1, &Serial_RxData, 1);//接收了一次后需要再次打开接收中断为下次中断接收做准备
//	}
// }



//		KeyNum=Key_GetNum();
//		if(KeyNum==1)
//		{
//			flag=!flag; 
//			snprintf(usart2_send, sizeof(usart2_send), "RedflagX");//转进数组里
//			K230_SendBuf(usart2_send,RedflagX);
//			OpenMV_Receive_Data++;
//			SendToScreen(OpenMV_Receive_Data);
////			HAL_UART_Transmit(&huart2, usart2_send,  8, HAL_MAX_DELAY);//发送数组里的字符串
//		}
//		if(KeyNum==2)
//		{
//			flag=!flag;
//			snprintf(usart2_send, sizeof(usart2_send), "Redflag");//转进数组里
//			HAL_UART_Transmit(&huart2, usart2_send,  7, HAL_MAX_DELAY);//发送数组里的字符串
//		}
		
//		HAL_UART_Transmit(&huart3, usart2_rece,  4, HAL_MAX_DELAY);//发送数组里的字符串
		
		//串口接收数据显示
//		OLED_ShowNum(3,1,usart2_rece[0],3);
//		OLED_ShowNum(3,5,usart2_rece[1],3);
//		OLED_ShowNum(3,9,usart2_rece[2],3);
//		OLED_ShowNum(3,13,usart2_rece[3],3);
		
		
//		if(interr_flag==1)
//		{
//			interr_flag=0;
//			snprintf(usart3_send, sizeof(usart3_send), "RedflagX");//转进数组里
//			HAL_UART_Transmit(&huart3, usart3_send,  sizeof(usart3_send), HAL_MAX_DELAY);//发送数组里的字符串
//		}
//		if (strcmp(usart3_rece, "LEDON") == 0)			//如果收到LED_ON指令
//		{
//			flag=0;
//		}
//		if (strcmp(usart3_rece, "LEDOF") == 0)			//如果收到LED_ON指令
//		{
//			flag=1;
//		}

//    if(NRF24L01_Get_Value_Flag() == 0)//接收
//    {
//        NRF24L01_GetRxBuf(RxBuf);
//    }
//	OLED_ShowNum(2,1,RxBuf[1],2);
//	OLED_ShowNum(2,3,RxBuf[1],2);
//	OLED_ShowNum(2,6,RxBuf[1],2);
//	OLED_ShowNum(2,9,RxBuf[1],2);
*/
extern uint8_t flag;
uint8_t flag8,flag9;
/**
 * @brief  蓝牙遥控指令解析（当前未接入主循环，作为调试手柄备用）
 * @details
 *   '1'~'8' 八方向平移、'9' 停车；'a'/'b'/'c' 机械臂升降停；
 *   'd'/'e' 舵机转向切换、'f'/'g' 夹爪松紧。
 *   每条指令后跟 HAL_Delay(1)，是为了让 CAN 帧之间有间隔，避免发送邮箱拥塞。
 * @note   本文件里 lanya_Receive() 先后写过三个版本：前两段已被注释（数字 case
 *         的早期协议），真正参与编译的是最后一段字符 case 版本。改协议时注意
 *         别改到注释段落，也别忘了 Serial.h 里还有一份同名声明。
 */
/*
void lanya_Receive(uint8_t Data)
{
	static uint8_t jiaodu;
	static uint8_t jiaodu1;
	
	switch (Data)
	{
		case '0':
		{
//			Location_algorithm(0,0,0);
//			Emm_V5_Vel_Control(5, 0, 0, 0, 0);	
		}
		break;
		case '1':
		{
			Location_algorithm(0,0.2,0);
			HAL_Delay(1);
		
		}
		break;
		case'2':
		{
			Location_algorithm(0,-0.2,0);
			HAL_Delay(1);
		}
		break;
		case '3':
		{
			Location_algorithm(-0.2,0,0);
			HAL_Delay(1);
		}
		break;
		case'4':
		{
			Location_algorithm(0.2,0,0);
			HAL_Delay(1);
		}
		break;
		
		case '5':
		{
			Emm_V5_Vel_Control(5, 1, 30, 0, 0);	
			HAL_Delay(1);
		}
		break;
		case'6':
		{
			Emm_V5_Vel_Control(5, 0, 30, 0, 0);	
			HAL_Delay(1);
		}
		break;
		case '7':
		{
			if(flag8==0)
			{
				jiaodu=0;
				flag8=1;
			}
		}
		break;
		case'8':
		{
			if(flag8==1)
			{
				flag8=0;
				jiaodu=180;
			}
			
		}
		break;
		case '9':
		{
			Location_algorithm(0,0,0);
			HAL_Delay(1);
		}
		break;
		case'a':
		{
			if(flag9==0)
			{
				jiaodu1=0;
				flag9=1;
			}
		}
		break;
		case'c':
		{
			if(flag9==1)
			{
				flag9=0;
				jiaodu1=70;
			}
		}
		break;
		case'b':
		{
			Emm_V5_Vel_Control(5, 0, 0, 0, 0);	
			HAL_Delay(1);
		}
		break;
		default:
			break;
	}
	douji_SetAngle1(jiaodu);
	douji_SetAngle2(jiaodu1);
}
*/

/**/
 void lanya_Receive(uint8_t Data)
{
	static uint8_t jiaodu;
	static uint8_t jiaodu1;
	
	switch (Data)
	{
		case '0':
		{

		}
		break;
		case '1'://前进
		{
			Location_algorithm(0,0.2,0);
			HAL_Delay(1);
		
		}
		break;
		case'2'://后退
		{
			Location_algorithm(0,-0.2,0);
			HAL_Delay(1);
		}
		break;
		case '3'://左移
		{
			Location_algorithm(-0.2,0,0);
			HAL_Delay(1);
		}
		break;
		case'4'://右移
		{
			Location_algorithm(0.2,0,0);
			HAL_Delay(1);
		}
		break;
		
		case '5'://左上
		{
			Location_algorithm(-0.2,0.2,0);
			HAL_Delay(1);
		}
		break;
		case'6'://右上
		{
			Location_algorithm(0.2,0.2,0);
			HAL_Delay(1);
		}
		break;
		case '7'://左下
		{
			Location_algorithm(-0.2,-0.2,0);
			HAL_Delay(1);
		}
		break;
		case'8'://右下
		{
			Location_algorithm(0.2,-0.2,0);
			HAL_Delay(1);
		}
		break;
		case '9'://停车
		{
			Location_algorithm(0,0,0);
			HAL_Delay(1);
		}
		break;
		case'a'://机械臂上移
		{
			Emm_V5_Vel_Control(5, 1, 100, 0, 0);	
			HAL_Delay(1);
		}
		break; 
		case'b'://机械臂下
		{
			Emm_V5_Vel_Control(5, 0, 100, 0, 0);	
			HAL_Delay(1);
		}
		break;		
		case'c'://机械臂停
		{
			Emm_V5_Vel_Control(5, 0, 0, 0, 0);	
			HAL_Delay(1);
		}
		break;	
		case'd'://舵机旋转
		{
			if(flag8==0)
			{
				jiaodu=0;
				flag8=1;
			}
		}
		break;	
		case'e'://舵机旋转
		{
			if(flag8==1)
			{
				flag8=0;
				jiaodu=180;
			}
		}
		break;	
		case'f'://抓取
		{
			if(flag9==0)
			{
				flag9=1;
				jiaodu1=0;
			}
		}
		break;	
		
		case'g'://松开
		{
			if(flag9==1)
			{
				flag9=0;
				jiaodu1=110;
			}
		}
		break;
		default:
			break;
	}
	douji_SetAngle1(jiaodu);//舵机赋值
	douji_SetAngle2(jiaodu1);
}



/*
 void lanya_Receive(uint8_t Data)
{
	static uint8_t jiaodu;
	static uint8_t jiaodu1;
	
	switch (Data)
	{
		case '0':
		{

		}
		break;
		case 1://前进
		{
			Location_algorithm(0,0.2,0);
//			HAL_Delay(1);
		
		}
		break;
		case 2://后退
		{
			Location_algorithm(0,-0.2,0);
//			HAL_Delay(1);
		}
		break;
		case 3://左移
		{
			Location_algorithm(-0.2,0,0);
//			HAL_Delay(1);
		}
		break;
		case 4://右移
		{
			Location_algorithm(0.2,0,0);
//			HAL_Delay(1);
		}
		break;
		
		case 5://左上
		{
			Location_algorithm(-0.2,0.2,0);
//			HAL_Delay(1);
		}
		break;
		case 6://右上
		{
			Location_algorithm(0.2,0.2,0);
			HAL_Delay(1);
		}
		break;
		case 7://左下
		{
			Location_algorithm(-0.2,-0.2,0);
			HAL_Delay(1);
		}
		break;
		case 8://右下
		{
			Location_algorithm(0.2,-0.2,0);
			HAL_Delay(1);
		}
		break;
		case 9://停车
		{
			Location_algorithm(0,0,0);
			HAL_Delay(1);
		}
		break;
		case 10://机械臂上移
		{
			Emm_V5_Vel_Control(5, 1, 30, 0, 0);	
			HAL_Delay(1);
		}
		break;
		case 11://机械臂下
		{
			Emm_V5_Vel_Control(5, 0, 30, 0, 0);	
			HAL_Delay(1);
		}
		break;		
		case 12://机械臂停
		{
			Emm_V5_Vel_Control(5, 0, 0, 0, 0);	
			HAL_Delay(1);
		}
		break;	
		case 13://舵机旋转
		{
			if(flag8==0)
			{
				jiaodu=0;
				flag8=1;
			}
		}
		break;	
		case 14://舵机旋转
		{
			if(flag8==1)
			{
				flag8=0;
				jiaodu=180;
			}
		}
		break;	
		case 15://抓取
		{
			if(flag9==0)
			{
				flag9=1;
				jiaodu1=0;
			}
		}
		break;	
		
		case 16://松开
		{
			if(flag9==1)
			{
				flag9=0;
				jiaodu1=70;
			}
		}
		break;
		case'h'://
		{

		}
		break;
		case'i'://
		{

		}
		break;
		case'k'://
		{

		}
		break;
		case'y'://
		{

		}
		break;
		case'p'://
		{

		}
		break;
		case'u'://
		{

		}
		break;
		case't'://
		{

		}
		break;
		case'r'://
		{

		}
		break;


//		case 1://前进
//		{
//			Location_algorithm(0,0.2,0);
//			HAL_Delay(1);
//		
//		}
//		break;
//		case 2://后退
//		{
//			Location_algorithm(0,-0.2,0);
//			HAL_Delay(1);
//		}
//		break;

		
		default:
			break;
	}
	douji_SetAngle1(jiaodu);//舵机赋值
	douji_SetAngle2(jiaodu1);
}

*/



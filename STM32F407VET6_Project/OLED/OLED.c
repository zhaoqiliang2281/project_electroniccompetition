/**
 ******************************************************************************
 * @file    OLED.c
 * @brief   0.96 寸 SSD1306 OLED 驱动（GPIO 模拟 I2C，SCL=PB8 / SDA=PB9）
 * @details
 *   时序全部软件模拟：拉高拉低 + 微秒延时（delaynus），没有 ACK 校验。
 *   显示坐标：Line 1~4（页），Column 1~16（字符列）。
 *   本工程用它显示：心跳计数、当前科目、视觉 x/y 像素、命中标志。
 * @note
 *   1. 软件 I2C 对延时敏感，delaynus 依赖 TIM14 为 1 MHz，改主频要重新确认波形；
 *   2. 屏不亮优先量 PB8/PB9 波形，再看 OLED_Init 的初始化序列是否被裁剪。
 ******************************************************************************
 */

#include "main.h"
#include "OLED_Font.h"
#include <Stdio.h>      //使用Sprintf时要包含
#include "delay.h"


void HAL_Delay_us(uint32_t us, uint16_t CLK_MHz )
{
	uint32_t Delay = us * CLK_MHz / 12;
	do
	{
	__NOP();
	} while (Delay --);
}

/*引脚配置*/
void OLED_W_SCL(uint8_t x)
{
	if(x)HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_8, GPIO_PIN_RESET);
//	HAL_Delay_us(1,168);
	delaynus(1);
}
void OLED_W_SDA(uint8_t x)
{
	if(x)HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_SET);
	else HAL_GPIO_WritePin(GPIOB, GPIO_PIN_9, GPIO_PIN_RESET);
//	HAL_Delay_us(1,168);
	delaynus(1);
}	


/*引脚初始化*/
void OLED_I2C_Init(void)
{
	/*释放SCL和SDA*/
	OLED_W_SCL(1);
	OLED_W_SDA(1);
}

/**
  * @brief  I2C开始
  * @param  无
  * @retval 无
  */
void OLED_I2C_Start(void)
{
	OLED_W_SDA(1);	//释放SDA，确保SDA为高电平
	OLED_W_SCL(1);	//释放SCL，确保SCL为高电平
	OLED_W_SDA(0);	//在SCL高电平期间，拉低SDA，产生起始信号
	OLED_W_SCL(0);	//起始后把SCL也拉低，即为了占用总线，也为了方便总线时序的拼接
}

/**
  * @brief  I2C停止
  * @param  无
  * @retval 无
  */
void OLED_I2C_Stop(void)
{
	OLED_W_SDA(0);	//拉低SDA，确保SDA为低电平
	OLED_W_SCL(1);	//释放SCL，使SCL呈现高电平
	OLED_W_SDA(1);	//在SCL高电平期间，释放SDA，产生终止信号
}

/**
  * @brief  I2C发送一个字节
  * @param  Byte 要发送的一个字节
  * @retval 无
  */
/**
 * @brief  软件 I2C 发送 1 字节（高位先出，不发 ACK 时钟）
 * @note   无应答检测，接线松动时表现为“写成功但屏幕不刷新”。
 */
void OLED_I2C_SendByte(uint8_t Byte)
{
	uint8_t i;
	/*循环8次，主机依次发送数据的每一位*/
	for (i = 0; i < 8; i++)
	{
		OLED_W_SDA(Byte & (0x80 >> i));
		OLED_W_SCL(1);	//释放SCL，从机在SCL高电平期间读取SDA
		OLED_W_SCL(0);	//拉低SCL，主机开始发送下一位数据
	}
	OLED_W_SCL(1);	//额外的一个时钟，不处理应答信号
	OLED_W_SCL(0);	//拉低SCL，主机开始发送下一位数据
}

/**
  * @brief  OLED写命令
  * @param  Command 要写入的命令
  * @retval 无
  */
void OLED_WriteCommand(uint8_t Command)
{
	OLED_I2C_Start();				//I2C起始
	OLED_I2C_SendByte(0x78);		//发送OLED的从机地址
	OLED_I2C_SendByte(0x00);		//控制字节，给0x00，表示即将写命令
	OLED_I2C_SendByte(Command); 	//写命令
	OLED_I2C_Stop();				//I2C终止
}

/**
  * @brief  OLED写数据
  * @param  Data 要写入的数据
  * @retval 无
  */
void OLED_WriteData(uint8_t Data)
{
	OLED_I2C_Start();
	OLED_I2C_SendByte(0x78);		//从机地址
	OLED_I2C_SendByte(0x40);		//写数据
	OLED_I2C_SendByte(Data);
	OLED_I2C_Stop();
}

/**
  * @brief  OLED设置光标位置
  * @param  Y 以左上角为原点，向下方向的坐标，范围：0~7
  * @param  X 以左上角为原点，向右方向的坐标，范围：0~127
  * @retval 无
  */
/* 设置显存页地址(0~7)与列地址(0~127)，1 基的 Line/Column 在此换算 */
void OLED_SetCursor(uint8_t Y, uint8_t X)
{
	OLED_WriteCommand(0xB0 | Y);					//设置Y位置
	OLED_WriteCommand(0x10 | ((X & 0xF0) >> 4));	//设置X位置高4位
	OLED_WriteCommand(0x00 | (X & 0x0F));			//设置X位置低4位
}

/**
  * @brief  OLED清屏
  * @param  无
  * @retval 无
  */
void OLED_Clear(void)
{  
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		OLED_SetCursor(j, 0);
		for(i = 0; i < 128; i++)
		{
			OLED_WriteData(0x00);
		}
	}
}

/**
  * @brief  OLED显示一个字符
  * @param  Line 行位置，范围：1~4
  * @param  Column 列位置，范围：1~16
  * @param  Char 要显示的一个字符，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowChar(uint8_t Line, uint8_t Column, char Char)
{      	
	uint8_t i;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 8);		//设置光标位置在上半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line - 1) * 2 + 1, (Column - 1) * 8);	//设置光标位置在下半部分
	for (i = 0; i < 8; i++)
	{
		OLED_WriteData(OLED_F8x16[Char - ' '][i + 8]);		//显示下半部分内容
	}
}

/**
  * @brief  OLED显示字符串
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  String 要显示的字符串，范围：ASCII可见字符
  * @retval 无
  */
void OLED_ShowString(uint8_t Line, uint8_t Column, char *String)
{
	uint8_t i;
	for (i = 0; String[i] != '\0'; i++)
	{
		OLED_ShowChar(Line, Column + i, String[i]);
	}
}

/**
  * @brief  OLED次方函数
  * @retval 返回值等于X的Y次方
  */
uint32_t OLED_Pow(uint32_t X, uint32_t Y)
{
	uint32_t Result = 1;
	while (Y--)
	{
		Result *= X;
	}
	return Result;
}

/**
  * @brief  OLED显示数字（十进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~4294967295
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十进制，带符号数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：-2147483648~2147483647
  * @param  Length 要显示数字的长度，范围：1~10
  * @retval 无
  */
void OLED_ShowSignedNum(uint8_t Line, uint8_t Column, int32_t Number, uint8_t Length)
{
	uint8_t i;
	uint32_t Number1;
	if (Number >= 0)
	{
		OLED_ShowChar(Line, Column, '+');
		Number1 = Number;
	}
	else
	{
		OLED_ShowChar(Line, Column, '-');
		Number1 = -Number;
	}
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i + 1, Number1 / OLED_Pow(10, Length - i - 1) % 10 + '0');
	}
}

/**
  * @brief  OLED显示数字（十六进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~0xFFFFFFFF
  * @param  Length 要显示数字的长度，范围：1~8
  * @retval 无
  */
void OLED_ShowHexNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i, SingleNumber;
	for (i = 0; i < Length; i++)							
	{
		SingleNumber = Number / OLED_Pow(16, Length - i - 1) % 16;
		if (SingleNumber < 10)
		{
			OLED_ShowChar(Line, Column + i, SingleNumber + '0');
		}
		else
		{
			OLED_ShowChar(Line, Column + i, SingleNumber - 10 + 'A');
		}
	}
}

/**
  * @brief  OLED显示数字（二进制，正数）
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~16
  * @param  Number 要显示的数字，范围：0~1111 1111 1111 1111
  * @param  Length 要显示数字的长度，范围：1~16
  * @retval 无
  */
void OLED_ShowBinNum(uint8_t Line, uint8_t Column, uint32_t Number, uint8_t Length)
{
	uint8_t i;
	for (i = 0; i < Length; i++)							
	{
		OLED_ShowChar(Line, Column + i, Number / OLED_Pow(2, Length - i - 1) % 2 + '0');
	}
}


/**
  * @brief  OLED显示波形函数
	* @param  Wave_sum波形值，范围：0~15
  * @retval 无
  */
void OLED_Wave(uint8_t Wave_Sum)//波形刷新
{
		static uint8_t i;		  
		OLED_WriteCommand (0xb6);    						//设置页地址
		OLED_WriteCommand((i & 0x0f));      		//设置显示位置—列低地址
		OLED_WriteCommand(((i & 0xf0) >> 4) |0x10);      	//设置显示位置—列高地址   
		OLED_WriteData(PPG_Wave_Fig[Wave_Sum*2]); 
	
		OLED_WriteCommand (0xb7);    						//设置页地址
		OLED_WriteCommand ((i & 0x0f));      				//设置显示位置—列低地址
		OLED_WriteCommand (((i & 0xf0) >> 4) |0x10);      	//设置显示位置—列高地址 
		OLED_WriteData(PPG_Wave_Fig[Wave_Sum*2+1]);		//显示数据,PPG_wave_fig[n]的值跟OLED寄存器配置的扫描方式相关
		i++;
		if(i>127) i=0;
}
/**
  * @brief  OLED显示中文
  * @param  Line 起始行位置，范围：1~4
  * @param  Column 起始列位置，范围：1~8
* @param  ChineseLoca 中文字符位置,范围:自定义
  * @retval 无
  */
void OLED_ShowChinese(uint8_t Line,uint8_t Column,uint8_t ChineseLoca)
{
	uint8_t i = 0;
	OLED_SetCursor((Line - 1) * 2, (Column - 1) * 16);		//设置光标位置在上半部分
	for (i = 0; i < 16; i++)
	{
		OLED_WriteData(Hzk[2*ChineseLoca][i]);			//显示上半部分内容
	}
	OLED_SetCursor((Line-1) * 2+1, ((Column - 1) * 16));	//设置光标位置在下半部分
	for (i = 0; i < 16; i++)
	{
		OLED_WriteData(Hzk[2*ChineseLoca+1][i]);		//显示下半部分内容
	}
}

void OLED_ShowFloat(uint8_t Line, uint8_t Column,float Parameter)    //把可变参数Parameter以小数类型显示
{
	  char buf[20];
	  sprintf(buf,"%.3f",Parameter);    //保留两位小数,并以逐个单字符存入数组buf中,包括小数点   Parameter=buf
	  OLED_ShowString(Line, Column,buf); //在line行Column列输出小数Parameter
}

/**
  * @brief  OLED初始化
  * @param  无
  * @retval 无
  */
/* SSD1306 上电初始化序列：复位 -> 关闭显示 -> 时钟/复用率/对比度/电荷泵 -> 开显示 */
void OLED_Init(void)
{
	uint32_t i, j;
	
	for (i = 0; i < 1000; i++)			//上电延时
	{
		for (j = 0; j < 1000; j++);
	}
	
	OLED_I2C_Init();			//端口初始化
	
	OLED_WriteCommand(0xAE);	//关闭显示
	
	OLED_WriteCommand(0xD5);	//设置显示时钟分频比/振荡器频率
	OLED_WriteCommand(0x80);
	
	OLED_WriteCommand(0xA8);	//设置多路复用率
	OLED_WriteCommand(0x3F);
	
	OLED_WriteCommand(0xD3);	//设置显示偏移
	OLED_WriteCommand(0x00);
	
	OLED_WriteCommand(0x40);	//设置显示开始行
	
	OLED_WriteCommand(0xA1);	//设置左右方向，0xA1正常 0xA0左右反置
	
	OLED_WriteCommand(0xC8);	//设置上下方向，0xC8正常 0xC0上下反置

	OLED_WriteCommand(0xDA);	//设置COM引脚硬件配置
	OLED_WriteCommand(0x12);
	
	OLED_WriteCommand(0x81);	//设置对比度控制
	OLED_WriteCommand(0xCF);

	OLED_WriteCommand(0xD9);	//设置预充电周期
	OLED_WriteCommand(0xF1);

	OLED_WriteCommand(0xDB);	//设置VCOMH取消选择级别
	OLED_WriteCommand(0x30);

	OLED_WriteCommand(0xA4);	//设置整个显示打开/关闭

	OLED_WriteCommand(0xA6);	//设置正常/倒转显示

	OLED_WriteCommand(0x8D);	//设置充电泵
	OLED_WriteCommand(0x14);

	OLED_WriteCommand(0xAF);	//开启显示
		
	OLED_Clear();				//OLED清屏
}


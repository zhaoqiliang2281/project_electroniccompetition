/**
 ******************************************************************************
 * @file    main.c
 * @brief   2025 电赛 · 灰度循迹小车主程序（MSPM0G3507 / 80 MHz / 无 RTOS）
 * @details
 *   【系统构成】
 *     - 车体：四路直流减速电机（TIMA0 四通道 PWM + GPIO 方向脚，5 kHz PWM）；
 *     - 感知：8 路灰度传感器（PB0/PB27/PB26/PA26/PA25/PA24/PA12/PA7，低电平=压黑线）；
 *     - 姿态：JY901S 陀螺仪（UART3 + DMA，PA13/PA14，115200），当前仅驱动就绪；
 *     - 显示：0.96" OLED（软件 I2C，PA16/PA15），用于实时显示传感器状态与调试量；
 *     - 输入：6 个独立按键（PA30/PA31/PB10/PB17/PB18/PB19，低电平有效）；
 *     - 编码器：E3/E4 两路正交编码器走 GPIOB 中断计数（本版未参与闭环）。
 *   【控制思路】
 *     灰度权值法算偏移 -> 位置式 PID 出“差速修正量 Error_Speed” ->
 *     左轮速度 = 基础速度 - 修正，右轮速度 = 基础速度 + 修正 ->
 *     由电机驱动层输出 PWM + 方向。整条链路跑在 TIMG0 定时中断里。
 *   【三种运行模式】（Key_GetNum10 按键循环切换，OLED 第 1 行第 13 列显示模式号）
 *     模式 0：定速 240 循迹；识别到横向黑线（location_flag==1）时原地右转后继续；
 *     模式 1：基础速度 beseSpeed 可用按键 18/19 每次 +/-10 调整（默认 200）；
 *             遇线做 13 次小角度顿挫转向，用于细调转弯半径；
 *     模式 2：定速 200 循迹；遇线用 130 速度做 90 度快速掉头。
 *     三种模式共用 Key_flag1 总开关（Key_GetNum1 翻转）与计圈逻辑：
 *     circle 累加已过线次数，达到 quanshu（按键3每次 +4，上限 24）后停轮。
 * @note
 *   1. 定时中断周期在 SysConfig 中配置为 5 ms（10 MHz 计数 + 50000 周期），
 *      即循迹 PID 运行频率约 200 Hz；源码内“每 10 ms”的旧注释按当时配置理解即可。
 *   2. SysConfig 工程文件为 empty.syscfg，改引脚/时钟请改它，勿手改
 *      ti_msp_dl_config.c/h（重新生成会覆盖）。
 *   3. 编译环境：Keil MDK + MSPM0 SDK 2.01.00.03（工程内包含路径写死了 D:\ti\...，
 *      换机器需要改 keil/empty_LP_MSPM0G3507_nortos_keil.uvprojx 的 IncludePath）。
 ******************************************************************************
 */

#include "All_include.h"
#include "ti_msp_dl_config.h"
#include "delay.h"
#include "OLED.h"
//#include "RGB.h"
#include "PWM.h"
#include "PID.h"
#include "Serial.h"
#include "Servo.h"
#include "Motor.h"
#include "grayscale_sensor.h"
#include "JY901S.h"

//#include "Serial.h"
//#include <stdio.h>

/*-------------------------------------------------------------
 * 全局变量区
 *   Error_Speed : 循迹 PID 输出，左右轮差速修正量（中断内更新）
 *   num         : 主循环计数器，看 OLED 变化可判断程序是否卡死
 *   beseSpeed   : 模式 1 的基础速度（按键 18/19 加减 10）
 *   zhuanSpeed  : 预留的转向速度，当前未参与控制
 *   circle      : 已通过的横线/圈数；circle_on 保证每轮只复位一次
 *   quanshu     : 目标圈数（按键 3 每次 +4，实际显示 quanshu/4）
 *   key_mode    : 运行模式索引 0/1/2
 *   Get_Encoder_countC/D : 编码器脉冲计数（GROUP1 中断累加，本版仅调试用）
 *-----------------------------------------------------------*/
float Error_Speed;

int num=0,num1;
uint8_t GetNum;
uint8_t led_flag;

int i;
int j;

int beseSpeed = 200;
int zhuanSpeed = 150;

uint32_t gpio_interrup;
uint32_t gpio_interrup1;
int32_t Get_Encoder_countA,encoderA_cnt,Get_Encoder_countB,encoderB_cnt;
int32_t Get_Encoder_countC,encoderC_cnt,Get_Encoder_countD,encoderD_cnt;

//uint8_t rpy[3]={0};

int16_t speedL,speedR;
int Key_flag,Key_flag1,Key_flag2;

int circle=0;
uint8_t circle_on;

int quanshu;
int csnum;

uint8_t key_mode,key_mode1;

/**
 * @brief  程序入口：外设初始化 + 三模式主循环
 * @retval 无（正常情况下不会返回）
 * @note   初始化顺序不要随意调换：
 *         SysConfig 系统时钟/引脚 -> OLED -> 电机 PWM -> 循迹 PID ->
 *         使能 TIMG0 定时中断并启动计数器。中断一旦开启，PID 就在后台持续跑。
 */
int main(void)
{
	SYSCFG_DL_init(); //芯片资源初始化,由SysConfig配置软件自动生成

	OLED_Init();	//OLED初始化

//	All_PID_Init();	//PID初始化
	
	Motor_Init();	//电机初始化
	
//	Servo_Init();	//舵机初始化

	automatic_Init();	//寻迹PID初始化
	
//	OLED_ShowString(1, 1, "Hello whord!:");		//显示静态字符串

	/* 上电等待传感器、OLED 与电机驱动稳定，避免首帧读到无效电平 */
	delay_ms(100);
	//开启中断
//	NVIC_EnableIRQ(UART0_INT_IRQn);	//使能串口0中断
//	NVIC_EnableIRQ(UART1_INT_IRQn);	//使能串口1中断
//	NVIC_EnableIRQ(UART2_INT_IRQn);	//使能串口2中断
//	NVIC_EnableIRQ(UART3_INT_IRQn);	//使能串口3中断
	

	/* 控制节拍：使能 TIMG0 周期中断并启动计数，PID 与灰度采样都在其中执行。
	 * 串口 0-3 的中断、GPIOB 编码器中断在本版均未开启（见下方注释）。 */
	NVIC_EnableIRQ(TIMG0_INT_IRQn);	//使能定时器中断
	DL_TimerG_startCounter(TIMG0);	//开启定时器

//	NVIC_EnableIRQ(GPIOB_INT_IRQn);//开启按键引脚的GPIOB端口中断
	
	
	/*--------------------- 主循环：显示 + 人机交互 ---------------------
	 * 主循环只做“不要求实时”的事（OLED 刷新、按键扫描、模式切换），
	 * 真正的控制计算全部放在 TIMG0 中断里，保证节拍稳定。 */
	while(1)
	{
		/* 第 2 行实时显示 8 路灰度电平，用于对线调试（1=白/未压线，0=黑/压线） */
		OLED_SENSOR_Show();//显示灰度值
		
		//主循环测试
		/* 循环计数：OLED 第 1 行第 1 列，数值不动说明主循环卡死 */
		num++;
		OLED_ShowNum(1,1,num,5);

		/* 按键 PB10：切换运行模式（0->1->2->0） */
		if(Key_GetNum10()==1)
		{
			key_mode++;
			key_mode=key_mode%3;
		}
		
		/* 按键 PA31：总开关，翻转 Key_flag1（置 1 才开始跑车） */
		if(Key_GetNum1()==1)
		{
			Key_flag1 = !Key_flag1;
		}
		/* 按键 PA30：目标圈数 +4，超过 24 归零（OLED 上按 quanshu/4 显示圈数） */
		if(Key_GetNum()==1)
		{
			quanshu+=4;
			quanshu=quanshu%24;
		}
		OLED_ShowNum(1,13,key_mode,1);
		/*---------------------------------------------------------------
		 * 模式选择：三种模式共用下面这套“读线 -> 差速 -> 输出”的框架，
		 * 差异只在基础速度、以及“识别到横线时”的动作序列。
		 * 判线依据 location_flag（见 grayscale_sensor.c 的采样检测法）：
		 *   location_flag == 1 且总开关已开 -> 认为压到横向黑线，执行一次转向
		 *   否则按 PID 差速正常循迹
		 *----------------------------------------------------------------- */
		switch (key_mode)
        {
        	case 0:
			{
				//车控制
				if(location_flag==1 && Key_flag1 == 1)//全亮与按键开启时才触发转向
				{			
					circle++;
					location_flag=0;
					
					Motor_LDN(90);
					Motor_RDN(90);
					delay_ms(550);//

					Motor_LDN(180);
					Motor_RDN(-180);
					delay_ms(580);
					
					Motor_LDN(0); 
					Motor_RDN(0);
					delay_ms(100);
				}
				else
				{
					speedL = 240-Error_Speed;//zuo
					speedR = 240+Error_Speed;//you
				}
				if(Key_flag1)
				{
					Motor_LDN(speedL);
					Motor_RDN(speedR);
				}
				else
				{
					Motor_LDN(0);
					Motor_RDN(0);
				}
				if(circle==quanshu)
				{
					Motor_LDN(0);
					Motor_RDN(0);	
				}
			   }
        		break;
        	case 1:
			{
				if(Key_GetNum19()==1)
				{
					beseSpeed += 10;
				}
				if(Key_GetNum18()==1)
				{
					beseSpeed -= 10;
				}	
				OLED_ShowNum(1,8,beseSpeed,4);
				
				//车控制
				if(location_flag==1 && Key_flag1 == 1)//全亮与按键开启时才触发转向 
				{					
					circle++;
					if(circle_on==0)
					{
						circle_on=1;
						circle=0;
					}	
					location_flag=0;
					
					Motor_LDN(90);
					Motor_RDN(90);
					delay_ms(300);
					
					for(j=0;j<13;j++)
					{
						Motor_LDN(150);
						Motor_RDN(-150);
						delay_ms(100);
						Motor_LDN(0); 
						Motor_RDN(0);
						delay_ms(800);					
					}
					Motor_LDN(0); 
					Motor_RDN(0);
				}
				else
				{
					speedL = beseSpeed-Error_Speed;//zuo
					speedR = beseSpeed+Error_Speed;//you
				}
				if(Key_flag1)
				{
					Motor_LDN(speedL);
					Motor_RDN(speedR);
				}
				else
				{
					Motor_LDN(0);
					Motor_RDN(0);
				}
				if(circle==quanshu)
				{
					Motor_LDN(0);
					Motor_RDN(0);	
				}
				
			}
        		break;
			case 2:
			{

				if(location_flag==1 && Key_flag1 == 1)
				{	
					circle++;
					if(circle_on==0)
					{
						circle_on=1;
						circle=0;
					}
					location_flag=0;
					
					Motor_LDN(90);
					Motor_RDN(90);
					delay_ms(550);//

					Motor_LDN(130);
					Motor_RDN(-130);
					delay_ms(1090);
					
					Motor_LDN(0); 
					Motor_RDN(0);
					delay_ms(100);
				}
				else
				{
					speedL = 200-Error_Speed;//zuo
					speedR = 200+Error_Speed;//you
				}
				if(Key_flag1)
				{
					Motor_LDN(speedL);
					Motor_RDN(speedR);
				}
				else
				{
					Motor_LDN(0);
					Motor_RDN(0);
				}
				if(circle==quanshu)
				{
					Motor_LDN(0);
					Motor_RDN(0);	
				}
			}
			break;
        }
			
//			OLED_ShowSignedNum(3,1,Error_Speed,3);	
//			OLED_ShowSignedNum(3,8,Calculate_Error(),3);
//			OLED_ShowSignedNum(4,1,speedL,5);
//			OLED_ShowSignedNum(4,8,speedR,5);
			/* 调试显示区（第 1 行 15 列起）：目标圈数 | 总开关 | 已计圈数 | 原始圈数计数 */
			OLED_ShowNum(1,15,quanshu/4%6,2);
			OLED_ShowNum(2,15,Key_flag1,2);
			OLED_ShowNum(3,15,circle,2);
			OLED_ShowNum(4,15,quanshu,2);		
		
	}
}

/************************************************
				if(Key_GetNum19()==1)	//加减速度
				{
					beseSpeed += 10;
				}
				if(Key_GetNum18()==1)
				{
					beseSpeed -= 10;
				}	
				OLED_ShowNum(1,8,beseSpeed,4);
				
				//车控制
				if(location_flag==1 && Key_flag1 == 1)//全亮与按键开启时才触发转向
				{
					Key_flag2=1;					
//					circle++;
					Motor_LDN(-200);
					Motor_RDN(200);
					if(location_flag==2)
					{
						location_flag=0;
					}
				}	
				else
				{
					speedL = beseSpeed-Error_Speed;//zuo
					speedR = beseSpeed+Error_Speed;//you
				}
				//
				if(Key_flag1)
				{
					Motor_LDN(speedL);
					Motor_RDN(speedR);
				}
				else
				{
					Motor_LDN(0);
					Motor_RDN(0);
				}
				if(circle==quanshu)
				{
					Motor_LDN(0);
					Motor_RDN(0);	
				}
// *  函数名称: 串口接收中断回调函数：UARTX_IRQHandler(void)//X:对应串口
// *  功能说明：
// *  参数说明：
// *  参   数： 无
// *  函数返回：无
// *  修改时间：2025年7月15日
// *  备   注：
// ************************************************/
/**
 * @brief  串口 0 接收中断（调试串口，PA10/PA11，115200）
 * @note   当前仅做“收到即回显”，用于验证链路是否通；正式功能可在此解析上位机指令。
 */
void UART0_IRQHandler(void) 
{
	if(DL_UART_getPendingInterrupt(UART0) == DL_UART_IIDX_RX) 
	{
//		UART_flag=0;
		UART_0_uart_rece[0] = DL_UART_receiveDataBlocking(UART0);
		DL_UART_transmitDataBlocking(UART0, UART_0_uart_rece[0]);//发送回来
	}
}
/**
 * @brief  串口 1 接收中断（PA8/PA9，115200）：回显模式，预留外设通信
 */
void UART1_IRQHandler(void) 
{
	if(DL_UART_getPendingInterrupt(UART1) == DL_UART_IIDX_RX) 
	{
//		UART_flag=1;
		UART_1_uart_rece[0] = DL_UART_receiveDataBlocking(UART1);
		DL_UART_transmitDataBlocking(UART1, UART_1_uart_rece[0]);//发送回来
	}
}
/**
 * @brief  串口 2 接收中断（PB15/PB16，115200）：回显模式，预留外设通信
 */
void UART2_IRQHandler(void) 
{
	if(DL_UART_getPendingInterrupt(UART2) == DL_UART_IIDX_RX) 
	{
//		UART_flag=2;
		UART_2_uart_rece[0] = DL_UART_receiveDataBlocking(UART2);
		DL_UART_transmitDataBlocking(UART2, UART_2_uart_rece[0]);//发送回来
	}
}
/**
 * @brief  串口 3 接收中断（JY901S 陀螺仪，PA13/PA14，115200）
 * @note   JY901S 的数据实际由 DMA 搬到 rxData[] 缓冲区解析（见 JY901S.c），
 *         此中断只做回显，正常运行时不应使能（否则会与 DMA 抢同一字节）。
 */
void UART3_IRQHandler(void) 
{
	if(DL_UART_getPendingInterrupt(UART3) == DL_UART_IIDX_RX) 
	{
//		UART_flag=3;
		UART_3_uart_rece[0] = DL_UART_receiveDataBlocking(UART3);
		DL_UART_transmitDataBlocking(UART3, UART_3_uart_rece[0]);//发送回来
	}
}
/************************************************
 *  函数名称: TIMG0中断回调函数：UART_Debug_INST_IRQHandler(void)
 *  功能说明：
 *  参数说明：
 *  参   数： 无
 *  函数返回：无
 *  修改时间：2025年7月15日
 *  备   注：
 ************************************************/
/**
 * @brief  定时器 G0（TIMG0）周期中断：循迹控制环
 * @note   中断里不要加延时或串口打印，否则会拉长节拍导致循迹抖动。
 */
void TIMG0_IRQHandler(void) 
{
//	static uint16_t T0Count = 0;
//	static uint16_t T1Count = 0;
	/* TIMG0 周期中断（约 5 ms 一次）：整个控制环的节拍源。
	 * 每次中断做两件事：读 8 路灰度、算一次循迹 PID。 */
	if(DL_Timer_getPendingInterrupt(TIMG0) == DL_TIMER_IIDX_ZERO) 
	{
//		T0Count++;
//		T1Count++;
//		if(T1Count == 10) 
//		{
//			T1Count = 0 ;

			//读取灰度值
			/* 采样 8 路灰度并更新 location_flag（是否压到横向黑线） */
			Read_Sensors();
		
			//差速转向，寻迹PID计算出左右误差赋给电机
			/* 权值法偏移量作为反馈，目标值为 0（把车拉回黑线中心），
			 * 输出即差速修正量，正负决定左右轮速度差 */
			Error_Speed=PID_calc(&PID_xunji,Calculate_Error(),0); //每10ms计算一次寻迹PID
			
//		}
//		if(T0Count == 20) 
//		{
//			T0Count = 0 ;
//			led_flag = !led_flag;
			
//			encoderC_cnt =  Get_Encoder_countC;//两个电机安装相反，所以编码器值也要相反
//			encoderD_cnt =  Get_Encoder_countD;
//			Get_Encoder_countC = 0;//编码器计数值清零
//			Get_Encoder_countD = 0;

//		}

  }
}

/*******************************************************
函数功能：外部中断模拟编码器信号
入口函数：无
返回  值：无
***********************************************************/
/**
 * @brief  GPIOB 组中断：正交编码器 4 倍频计数（E3/E4 两路）
 * @details
 *   每路编码器 A/B 两相各接一个中断源，任一跳变都判一次方向：
 *   - 若 A 相触发：B 为低则计数 -1，否则 +1；
 *   - 若 B 相触发：A 为低则计数 +1，否则 -1。
 *   这就是常见的“4 倍频”软解码，分辨率最高、但 CPU 开销也最大。
 * @note   E1/E2 两路的解码代码仍保留在上方注释块中，本车只用 C/D 两路；
 *         中断里必须清标志，否则会反复进中断。
 */
void GROUP1_IRQHandler(void)
{
/*	
//	//获取中断信号
//	gpio_interrup = DL_GPIO_getEnabledInterruptStatus(GPIOB,ENCODER_E1A_PIN|ENCODER_E1B_PIN|ENCODER_E2A_PIN|ENCODER_E2B_PIN);
//	//encoderA
//	if((gpio_interrup & ENCODER_E1A_PIN)==ENCODER_E1A_PIN)
//	{
//		if(!DL_GPIO_readPins(GPIOB,ENCODER_E1B_PIN))
//		{
//			Get_Encoder_countA--;
//		}
//		else
//		{
//			Get_Encoder_countA++;
//		}
//	}
//	else if((gpio_interrup & ENCODER_E1B_PIN)==ENCODER_E1B_PIN)
//	{
//		if(!DL_GPIO_readPins(GPIOB,ENCODER_E1A_PIN))
//		{
//			Get_Encoder_countA++;
//		}
//		else
//		{
//			Get_Encoder_countA--;
//		}
//	}
//	//encoderB
//	if((gpio_interrup & ENCODER_E2A_PIN)==ENCODER_E2A_PIN)
//	{
//		if(!DL_GPIO_readPins(GPIOB,ENCODER_E2B_PIN))
//		{
//			Get_Encoder_countB--;
//		}
//		else
//		{
//			Get_Encoder_countB++;
//		}
//	}
//	else if((gpio_interrup & ENCODER_E2B_PIN)==ENCODER_E2B_PIN)
//	{
//		if(!DL_GPIO_readPins(GPIOB,ENCODER_E2A_PIN))
//		{
//			Get_Encoder_countB++;
//		}
//		else
//		{
//			Get_Encoder_countB--;
//		}
//	}
//	DL_GPIO_clearInterruptStatus(GPIOB,ENCODER_E1A_PIN|ENCODER_E1B_PIN|ENCODER_E2A_PIN|ENCODER_E2B_PIN);
*/	
/*
	gpio_interrup = DL_GPIO_getEnabledInterruptStatus(GPIOB,ENCODER_E1A_PIN|ENCODER_E1B_PIN|ENCODER_E2A_PIN|ENCODER_E2B_PIN);
	// 假设只配置 E1A 和 E2A 为中断触发
	if (gpio_interrup & ENCODER_E1A_PIN) 
	{
		if (DL_GPIO_readPins(GPIOB, ENCODER_E1B_PIN)) 
		{
			Get_Encoder_countA++;
		} 
		else 
		{
			Get_Encoder_countA--;
		}
	}

	if (gpio_interrup & ENCODER_E2A_PIN) 
	{
		if (DL_GPIO_readPins(GPIOB, ENCODER_E2B_PIN)) 
		{
			Get_Encoder_countB++;
		} 
		else 
		{
			Get_Encoder_countB--;
		}
	}
	DL_GPIO_clearInterruptStatus(GPIOB,ENCODER_E1A_PIN|ENCODER_E1B_PIN|ENCODER_E2A_PIN|ENCODER_E2B_PIN);
*/

/*
//	//获取中断信号
//	gpio_interrup = DL_GPIO_getEnabledInterruptStatus(GPIOB,ENCODER_E1A_PIN | ENCODER_E1B_PIN | ENCODER_E2A_PIN | ENCODER_E2B_PIN);
//	//encoderA
//	if((gpio_interrup & ENCODER_E1A_PIN)==ENCODER_E1A_PIN)
//	{
//		if(!DL_GPIO_readPins(GPIOB,ENCODER_E1B_PIN))
//		{
//			Get_Encoder_countA--;
//		}
//		else
//		{
//			Get_Encoder_countA++;
//		}
//	}
//	else if((gpio_interrup & ENCODER_E1B_PIN)==ENCODER_E1B_PIN)
//	{
//		if(!DL_GPIO_readPins(GPIOB,ENCODER_E1A_PIN))
//		{
//			Get_Encoder_countA++;
//		}
//		else
//		{
//			Get_Encoder_countA--;
//		}
//	}
//	//encoderB
//	if((gpio_interrup & ENCODER_E2A_PIN)==ENCODER_E2A_PIN)
//	{
//		if(!DL_GPIO_readPins(GPIOB,ENCODER_E2B_PIN))
//		{
//			Get_Encoder_countB--;
//		}
//		else
//		{
//			Get_Encoder_countB++;
//		}
//	}
//	else if((gpio_interrup & ENCODER_E2B_PIN)==ENCODER_E2B_PIN)
//	{
//		if(!DL_GPIO_readPins(GPIOB,ENCODER_E2A_PIN))
//		{
//			Get_Encoder_countB++;
//		}
//		else
//		{
//			Get_Encoder_countB--;
//		}
//	}
//	DL_GPIO_clearInterruptStatus(GPIOB,ENCODER_E1A_PIN|ENCODER_E1B_PIN|ENCODER_E2A_PIN|ENCODER_E2B_PIN);
*/	
	gpio_interrup1 = DL_GPIO_getEnabledInterruptStatus(GPIOB,ENCODER_E3A_PIN | ENCODER_E3B_PIN | ENCODER_E4A_PIN | ENCODER_E4B_PIN);
	//encoderC
	if((gpio_interrup1 & ENCODER_E3A_PIN)==ENCODER_E3A_PIN)
	{
		if(!DL_GPIO_readPins(GPIOB,ENCODER_E3B_PIN))
		{
			Get_Encoder_countC--;
		}
		else
		{
			Get_Encoder_countC++;
		}
	}
	else if((gpio_interrup1 & ENCODER_E3B_PIN)==ENCODER_E3B_PIN)
	{
		if(!DL_GPIO_readPins(GPIOB,ENCODER_E3A_PIN))
		{
			Get_Encoder_countC++;
		}
		else
		{
			Get_Encoder_countC--;
		}
	}
	//encoderD
	if((gpio_interrup1 & ENCODER_E4A_PIN)==ENCODER_E4A_PIN)
	{
		if(!DL_GPIO_readPins(GPIOB,ENCODER_E4B_PIN))
		{
			Get_Encoder_countD--;
		}
		else
		{
			Get_Encoder_countD++;
		}
	}
	else if((gpio_interrup1 & ENCODER_E4B_PIN)==ENCODER_E4B_PIN)
	{
		if(!DL_GPIO_readPins(GPIOB,ENCODER_E4A_PIN))
		{
			Get_Encoder_countD++;
		}
		else
		{
			Get_Encoder_countD--;
		}
	}
	
	DL_GPIO_clearInterruptStatus(GPIOB,ENCODER_E3A_PIN|ENCODER_E3B_PIN|ENCODER_E4A_PIN|ENCODER_E4B_PIN);

}


/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : 2025 电赛 · 视觉追踪激光云台主程序（STM32F407VET6 / 168 MHz）
  ******************************************************************************
  * @details
  *   【工程定位】系统入口与调度核心：视觉坐标 -> PID -> CAN 闭环步进电机 -> 云台指向。
  *
  *   【外设分工】
  *     - USART3 (PB10/PB11, 115200) + DMA1_Stream1 + IDLE 中断：接收 MaixCAM 的
  *       目标像素坐标，ASCII 协议 "x<数字>y<数字>"，如 "x170y115"；
  *       x=640 & y=480 是约定好的“丢失目标”哨兵值。
  *     - CAN1 (PA11/PA12, 500 kbps, 扩展帧)：挂 Emm_V5（张大头）闭环步进驱动器。
  *       ID1 = 云台水平轴、ID2 = 云台俯仰轴；ID3/ID4 底盘、ID5 机械臂为预留。
  *     - USART1 (K230 扫码) / USART2 (K230 色旗识别) / USART4 (串口屏)：预留未启用。
  *     - USART6 (HWT101 陀螺仪)：驱动与解析已写好，但本版未开启接收中断，Yaw 恒为 0。
  *     - TIM6：1 ms 节拍。每 10 次（10 ms）跑一次云台 PID，同时驱动
  *       LED_Tick / Buzzer_Tick / Key_Tick / Buzzer_scan。
  *     - TIM7 / TIM14：微秒级延时（delaynus，OLED 软件 I2C 用）。
  *
  *   【主循环】
  *     清计数 -> MaixCAM_data_manage()（解析 DMA 缓冲并重启 DMA）
  *     -> Key_GetNum() 取按键 -> 按键 3 在 KM2/KM3/KM4 三种策略间轮换
  *     -> 执行对应策略 -> OLED 显示坐标。
  *
  *   【三种追踪策略】（见 System/Subject.c）
  *     KM2：只追 x 轴（目标 170），x 落在 165~175 时开激光；
  *     KM3：x、y 双轴（目标 170 / 118），并支持“先回零再左/右转”扫图；
  *     KM4：x、y 双轴（目标 170 / 115），纯追踪不扫图。
  *
  *   【控制链】
  *     Cloud_platform_PID_Scan()（10 ms 一次）读取 X3Temp/Y3Temp，
  *     与目标像素做增量式 PID，输出速度 -> cloud_platformSpeed()
  *     -> cloud_platformSpeed() 用 Emm_V5_Vel_Control 经 CAN 下发，并阻塞等待回帧。
  *
  * @warning
  *   1. USART3 的 IDLE 中断里重启 DMA 用的是 16 字节（见 stm32f4xx_it.c），
  *      而 main / Serial2 用的是 256 字节；一帧超过 16 字节时会被截断，
  *      改协议长度时两处必须同时改。
  *   2. Emm_V5 的速度控制函数里有 while(can.rxFrameFlag == false) 忙等，
  *      主频/总线异常时会死等；调试时若“停在这里”，先查 CAN 终端电阻与波特率。
  *   3. 本文件由 STM32CubeMX 生成，手写代码请夹在成对的 USER CODE 标记区之间，
  *      否则重新生成 .ioc 时会被覆盖。
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "can.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* CubeMX 生成的外设头文件（可由 .ioc 重新生成） */
#include "can.h"
#include "HWT101.h"
#include "Emm_V5.h"
#include "motor.h"
#include "chassis_behaviour.h"
#include "stm32f4xx_it.h"
/* 手写模块：显示 / 人机 / 通信 / 云台 / 科目策略 */
#include "OLED.h"
#include "Key.h"
#include "LED.h"
#include "BUZZER.h"
#include "Serial.h"
#include "Serial2.h"
#include "HWT101.h"
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Dloud_deck.h"
#include "Subject.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

#define usart2_rece_sizeof	5		/* 旧版 USART2 单帧长度（当前未使用，保留以兼容早期代码） */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

float num;							/* 通用浮点调试量（当前未被使用） */
uint8_t flag;						/* 1 Hz 翻转标志，用于观察主循环/中断是否存活 */


/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* 串口接收标志：1 = 对应的 RxCplt 回调刚取到一帧（当前未被消费，保留） */
uint8_t interr_flag = 0 ;

/*----------------------------------------------------------------------------
 * 串口收发缓冲区
 *   每个串口一对：rece 供中断/DMA 写入，send 供组包后一次性发出。
 *   128 字节是“够用且不浪费 RAM”的经验值；MaixCAM 帧很短，实际只用到前几字节。
 *   usart4 对应串口屏（本版未接），usart6 为 HWT101 单字节接收。
 *----------------------------------------------------------------------------*/
uint8_t usart1_rece[128] = {0};
char usart1_send[128] = {0};
uint8_t usart2_rece[128] = {0};
char usart2_send[128] = {0};
uint8_t usart3_rece[128] = {0};		/* MaixCAM 备用中断接收区（主链路走 DMA） */
char usart3_send[128] = {0};
uint8_t usart4_rece[128] = {0};
char usart4_send[128] = {0};
uint8_t usart6_rece[128] = {0};
//char usart6_send[128] = {0};
uint8_t g_usart6_receivedata = 0;	/* HWT101 逐字节接收缓存（接收中断未开启，故恒为 0） */

uint8_t RxData[20];					/* 备用原始帧缓存 */

uint8_t num1,num2,num3,num4;		/* 各链路的“收到过多少帧”计数，num2 显示在 OLED 第 1 行 */

uint8_t usart1_receivedata;			/* 旧版 K230 单字节接收变量 */
uint8_t usart2_receivedata;

/* 当前科目策略索引：0=KM2、1=KM3、2=KM4（按键 3 循环切换） */
uint8_t Key_mode;

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  /* 注意：MX_DMA_Init 必须在各 UART/CAN 初始化之前调用，
   * 否则 DMA 时钟未开、句柄未挂接，后续 Receive_DMA 会返回 HAL_ERROR */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_CAN1_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART6_UART_Init();
  MX_TIM14_Init();
  MX_TIM7_Init();
  MX_TIM6_Init();
  /* USER CODE BEGIN 2 */
	/* 应用层初始化：先“能显示、能响、能算”，再打通通信链路 */
	LED_Init();				/* 三路 LED + 激光使能脚全部拉到熄灭状态 */
	OLED_Init();			/* OLED 上电初始化，用于后续所有调试显示 */
	Buzzer_Init();

	Cloud_platform_PID_Init();	/* 云台 x/y 三套 PID 参数初始化（增量式） */

//	OLED_ShowString(1,1,"Hello World!");


	/* CAN 链路：滤波器 -> 启动 -> 打开 RX0 中断通知 */
	USER_CAN1_Filter_Init();	/* 32 位掩码全 0，即“接收所有扩展帧” */
	if(HAL_CAN_Start(&hcan1) != HAL_OK) { Error_Handler(); }	/* 启动 CAN 控制器 */
	if(HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) { Error_Handler(); }	/* 使能 FIFO0 非空中断 */

	/* 视觉链路：DMA + IDLE 中断“不定长接收”
	 * 思路：DMA 一直往 buffer3 里填，总线空闲（IDLE）时用剩余计数算出本帧长度。
	 * 长度 256 必须与 Serial2.c 中的重启长度一致。 */
//	HAL_UART_Receive_IT(&huart1,usart1_rece,4);//旧版一击扫码，四字节固定帧

//	HAL_UART_Receive_DMA(&huart2,buffer,16);
//	__HAL_UART_ENABLE_IT(&huart2,UART_IT_IDLE);//旧版第二路 K230 识别小旗

	HAL_UART_Receive_DMA(&huart3,buffer3,256);
	__HAL_UART_ENABLE_IT(&huart3,UART_IT_IDLE);//MaixCAM 识别主链路

	/* 控制节拍：TIM6 每 1 ms 进一次中断，回调里再分频到 10 ms 跑 PID */
	HAL_TIM_Base_Start_IT(&htim6);	/* 启动定时器 6 并开中断 */
	Jiguang_OFF();					/* 上电确保激光关闭（安全要求） */

//	Emm_V5_Origin_Modify_Params(2, true, 0, 0, 80, 200, 100, 100, 100, false);

//	cloud_platform(3200,3200);

//	Emm_V5_Origin_Set_O(1, 1);//设置单圈回零的零点位置
//	HAL_Delay(15);
//	Emm_V5_Origin_Trigger_Return(2,0,0);
//	HAL_Delay(15);
//	Emm_V5_Origin_Set_O(2, 1);//设置单圈回零的零点位置
//	HAL_Delay(15);
//	Emm_V5_Origin_Trigger_Return(2,0,0);
//	HAL_Delay(15);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  /* 主循环心跳：num2 递增显示在 OLED 第 1 行第 1 列，数值不动就说明主循环卡死 */
		num2 ++;
		OLED_ShowNum(1,1,num2,5);

		MaixCAM_data_manage();	/* 解析 DMA 缓冲里的 "x..y.."，解析完清零并重启 DMA */

		KeyNum=Key_GetNum();	/* 读并清除按键事件（由 1 ms 中断里的 Key_Tick 产生） */
		if(KeyNum == 3)			/* 按键 3 = 切换科目策略 */
		{
			KeyNum = 0;
			Key_mode++;
			Key_mode%=3;
		}
		switch (Key_mode)
		{
			case 0:
			{
				OLED_ShowString(1,8,"KM2");	/* 第 1 行第 8 列显示当前策略 */
				KM2();
			}
			break;
			case 1:
			{
				OLED_ShowString(1,8,"KM3");
				KM3();
			}
			break;
			case 2:
			{
				OLED_ShowString(1,8,"KM4");
				KM4();
			}
		break;
		}

		/* 调试显示：第 2 行 = 目标 x 像素，第 3 行 = 目标 y 像素（640/480 表示丢目标） */
		OLED_ShowNum(2,1,X3Temp,5);
		OLED_ShowNum(3,1,Y3Temp,5);

//		OLED_ShowSignedNum(4,1,T_Cloud_platform_Speedresultx,3);
//		OLED_ShowSignedNum(4,8,T_Cloud_platform_Speedresulty,3);

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
/**
  * @brief  系统时钟配置：HSE 8 MHz -> PLL -> SYSCLK 168 MHz
  * @note   PLL: 8 MHz / M(4) = 2 MHz -> * N(168) = 336 MHz -> / P(2) = 168 MHz；
  *         APB1 = 42 MHz（CAN/串口 2~6 挂这里，CAN 的 500 kbps 就是按 42 MHz 算的），
  *         APB2 = 84 MHz（串口 1/6、SPI 等）。
  *         改了主频，必须先重算 CAN 分频与各串口的 BRR，否则通信会静默失效。
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/**
  * @brief  TIM6 周期中断回调（1 ms 一次）：软件定时任务分发中心
  * @note   这里的 10 / 1000 分频都是“按 1 ms 节拍数”实现的软件定时器：
  *         - 每 10 ms：一次云台 PID；
  *         - 每 1000 ms：flag 翻转（心跳/调试用）；
  *         - 每 1 ms：LED 呼吸/闪烁、蜂鸣器定时、按键 20 ms 消抖扫描。
  *         中断里不要放 HAL_Delay 或串口打印，否则会把 1 ms 节拍拉长。
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)	
{	
	static uint16_t T0time = 0,T1time = 0;
	if(htim==&htim6)
	{
		T0time++;
		if(T0time>=10)			/* 10 ms：控制周期 */
		{
			T0time = 0;
			Cloud_platform_PID_Scan();
		}
		T1time++;
		if(T1time>=1000)		/* 1 s：心跳标志翻转 */
		{
			T1time = 0;
			flag=!flag;
		}
		
		LED_Tick();
		Buzzer_Tick();
		Key_Tick();
		Buzzer_scan();
	}
 }

/**
  * @brief  串口接收完成回调（仅中断式接收会进入，DMA 接收走 IDLE 中断）
  * @note   huart1 是 K230 扫码（4 字节定长帧），huart3 是 MaixCAM 的备用中断通道；
  *         huart6 为 HWT101 逐字节接收，但启动接收的语句被注释，
  *         所以当前 Yaw 恒为 0，云台也就没有“绝对角度”闭环。
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if( huart == &huart1)/* 串口 1 中断源：K230 扫码，当前接收未开启 */
    {
//		K230_QuickMark_Receive(usart1_rece[0],0);	//解析 K230 的维码识别结果
//		K230_QuickMark_Receive(usart1_rece[1],1);	//解析 K230 的维码识别结果
		HAL_UART_Receive_IT(&huart1,usart1_rece,4); /* 重新打开接收中断 */
    }
	if( huart == &huart2)/* 串口 2 中断源：K230 识别色旗，当前接收未开启 */
    {
//		HAL_UART_Receive_DMA(&huart1,buffer,16);
    }
	if( huart == &huart3)/* 串口 3 中断源：MaixCAM（主链路实际走 DMA + IDLE） */
    {
		HAL_UART_Receive_IT(&huart3,usart3_rece,1);/* 重新打开接收中断 */
    }
//	if( huart == &huart4)/* 串口 4 中断源：串口屏，当前未接 */
//    {
//		HAL_UART_Receive_IT(&huart4,usart4_rece,1);/* 重新打开接收中断(目前没有接串口屏) */
//    }
	if( huart == &huart6)/* 串口 6 中断源：HWT101 陀螺仪，逐字节喂给状态机 */
    {
        HWT101_ReceiveData(g_usart6_receivedata);/*把收到的字节交给协议解析 */

//        HAL_UART_Receive_IT(&huart6,&g_usart6_receivedata,1);/* 重新打开接收中断 */
    }
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
/**
  * @brief  错误兜底：关全局中断后死循环
  * @note   现象是“程序全停、调试器仍在线”。定位方法：在此打断点看调用栈，
  *         常见原因是 CAN/USART 初始化失败或时钟配置不对。
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

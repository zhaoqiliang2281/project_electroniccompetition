/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
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

/*==============================================================================
 * stm32f4xx_it.c —— 全部中断服务函数（CubeMX 骨架 + 手写用户代码）
 *
 * 【本工程真正在跑的中断】
 *   TIM6_DAC_IRQHandler   1 ms 节拍：云台 PID（10 ms）、LED/蜂鸣器/按键扫描
 *   CAN1_RX0_IRQHandler   Emm_V5 回帧：把 FIFO0 的数据搬到 can.rxData 并置帧标志
 *   USART3_IRQHandler     MaixCAM 不定长接收：IDLE 中断 + DMA 剩余计数算帧长
 *   DMA1_Stream1/3/5/6    与 USART2/3 收发配对的 DMA 中断
 *   SysTick_Handler       HAL 时基（HAL_GetTick/HAL_Delay 依赖它）
 *
 * 【未启用但保留的】
 *   USART2_IRQHandler 的 IDLE 处理整段被注释（K230 色旗识别未接入）
 *   USART6_IRQHandler  仅转发给 HAL，HWT101 的接收中断从未启动，Yaw 恒为 0
 *
 * @warning
 *   1. USART3 的 IDLE 分支重启 DMA 时长度写的是 16，而 main.c / Serial2.c 用的是
 *      256。一帧 > 16 字节即被截断，且这里是“下次接收长度 16”的源头，改协议时
 *      务必把三处长度一起改（该处的 16 还与 __HAL_DMA_GET_COUNTER 的差值计算绑定）。
 *   2. 下面 extern 声明的 buffer3[15] 与实际定义（Serial2.c 中 256 字节）不一致，
 *      只能当符号引用，不要在此文件用 sizeof(buffer3)。
 *============================================================================*/
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "can.h"
#include "HWT101.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

 /* 外部缓冲与标志：buffer/buffer3 为串口 DMA 目标区，
  * buffer3 的实际长度是 256（定义在 Serial2.c），此处仅作符号引用 */
 extern uint8_t buffer[15];
 extern uint8_t flah;
 extern uint8_t len;
 extern int a;//收到yes为1 
 
 extern uint8_t USAR3_ReceiveData_len;
 extern uint8_t buffer3[15];
 
/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern CAN_HandleTypeDef hcan1;
extern TIM_HandleTypeDef htim6;
extern DMA_HandleTypeDef hdma_usart2_rx;
extern DMA_HandleTypeDef hdma_usart2_tx;
extern DMA_HandleTypeDef hdma_usart3_tx;
extern DMA_HandleTypeDef hdma_usart3_rx;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;
extern UART_HandleTypeDef huart6;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
   while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
/* SysTick：HAL 的 1 ms 时基，HAL_Delay/HAL_GetTick 全靠它，勿在此加耗时操作 */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles DMA1 stream1 global interrupt.
  */
/* DMA 中断与串口配对：Stream1/3 = USART3 收/发，Stream5/6 = USART2 收/发 */
void DMA1_Stream1_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream1_IRQn 0 */

  /* USER CODE END DMA1_Stream1_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_rx);
  /* USER CODE BEGIN DMA1_Stream1_IRQn 1 */

  /* USER CODE END DMA1_Stream1_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream3 global interrupt.
  */
/* USART3 发送方向 DMA（IDLE 回传时用到） */
void DMA1_Stream3_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream3_IRQn 0 */

  /* USER CODE END DMA1_Stream3_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart3_tx);
  /* USER CODE BEGIN DMA1_Stream3_IRQn 1 */

  /* USER CODE END DMA1_Stream3_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream5 global interrupt.
  */
/* USART2 接收方向 DMA */
void DMA1_Stream5_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream5_IRQn 0 */

  /* USER CODE END DMA1_Stream5_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_rx);
  /* USER CODE BEGIN DMA1_Stream5_IRQn 1 */

  /* USER CODE END DMA1_Stream5_IRQn 1 */
}

/**
  * @brief This function handles DMA1 stream6 global interrupt.
  */
/* USART2 发送方向 DMA */
void DMA1_Stream6_IRQHandler(void)
{
  /* USER CODE BEGIN DMA1_Stream6_IRQn 0 */

  /* USER CODE END DMA1_Stream6_IRQn 0 */
  HAL_DMA_IRQHandler(&hdma_usart2_tx);
  /* USER CODE BEGIN DMA1_Stream6_IRQn 1 */

  /* USER CODE END DMA1_Stream6_IRQn 1 */
}

/**
  * @brief This function handles CAN1 RX0 interrupts.
  */
/**
 * @brief  CAN1 接收 FIFO0 非空中断：Emm_V5 回帧的唯一入口
 * @details
 *   流程：HAL_CAN_GetRxMessage 取一帧到 can.rxData -> 未用满的字节补 0
 *   -> can.rxFrameFlag = true。上层（Emm_V5 / motor.c）通过轮询该标志
 *   判断“回帧到了”，这也是 Emm_V5 里 while(can.rxFrameFlag == false) 的原理。
 * @note   帧标志是“一次性”的：上层读到后必须自己清，否则下一次会误判。
 */
void CAN1_RX0_IRQHandler(void)
{
  /* USER CODE BEGIN CAN1_RX0_IRQn 0 */
	
	uint8_t i = 0;

		// 接收一包数据
	if(HAL_CAN_GetRxMessage((&hcan1), CAN_RX_FIFO0, (CAN_RxHeaderTypeDef *)(&can.CAN_RxMsg), (uint8_t *)(&can.rxData)) == HAL_OK)
	{
		// 一帧数据接收完成，置位帧标志位
		for(i=can.CAN_RxMsg.DLC; i < 8; i++) { can.rxData[i] = 0; } can.rxFrameFlag = true;
	}
	
  /* USER CODE END CAN1_RX0_IRQn 0 */
  HAL_CAN_IRQHandler(&hcan1);
  /* USER CODE BEGIN CAN1_RX0_IRQn 1 */

  /* USER CODE END CAN1_RX0_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */

  /* USER CODE END USART1_IRQn 0 */
  HAL_UART_IRQHandler(&huart1);
  /* USER CODE BEGIN USART1_IRQn 1 */

  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART2 global interrupt.
  */
/**
 * @brief  USART2 中断：K230 色旗识别通道（本版未接入）
 * @note   IDLE 分支的处理代码整段被注释，只保留 HAL 默认转发，等于空跑。
 */
void USART2_IRQHandler(void)
{
  /* USER CODE BEGIN USART2_IRQn 0 */

	
	if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_IDLE)!=RESET)
	{
//		__HAL_UART_CLEAR_IDLEFLAG(&huart2);
//		HAL_UART_DMAStop(&huart2);
//		len=16-__HAL_DMA_GET_COUNTER(huart2.hdmarx);
//		
////		HAL_UART_Transmit_DMA(&huart2,buffer,len);//数据回传
//		
//		HAL_UART_Receive_DMA(&huart2,buffer,16);//重新打开＿3是目标最大数捿
		
	}
	

  /* USER CODE END USART2_IRQn 0 */
  HAL_UART_IRQHandler(&huart2);
  /* USER CODE BEGIN USART2_IRQn 1 */

  /* USER CODE END USART2_IRQn 1 */
}

/**
  * @brief This function handles USART3 global interrupt.
  */
/**
 * @brief  USART3 中断：MaixCAM 视觉数据的“不定长帧”接收
 * @details
 *   接收由 DMA 常驻搬运，串口总线空闲 1 个字符时间触发 IDLE：
 *     1) 清 IDLE 标志；
 *     2) 停 DMA，用 16 - 剩余计数 得到本帧字节数；
 *     3) 原样回传给 MaixCAM（调试用，可删，会占用总线时间）；
 *     4) 重新打开 DMA 接收。
 * @warning 这里的 16 与 main.c 的 256 不一致（历史遗留），见文件头说明。
 *          回传与重启 DMA 都在中断里做，帧率很高时会拖慢主循环。
 */
void USART3_IRQHandler(void)
{
  /* USER CODE BEGIN USART3_IRQn 0 */
	
	if(__HAL_UART_GET_FLAG(&huart3,UART_FLAG_IDLE)!=RESET)
	{
		__HAL_UART_CLEAR_IDLEFLAG(&huart3);
		HAL_UART_DMAStop(&huart3);
		USAR3_ReceiveData_len=16-__HAL_DMA_GET_COUNTER(huart3.hdmarx);
		
		HAL_UART_Transmit_DMA(&huart3,buffer3,USAR3_ReceiveData_len);//数据回传
		
		HAL_UART_Receive_DMA(&huart3,buffer3,16);//重新打开＿3是目标最大数捿
		
		
	}
	
  /* USER CODE END USART3_IRQn 0 */
  HAL_UART_IRQHandler(&huart3);
  /* USER CODE BEGIN USART3_IRQn 1 */

  /* USER CODE END USART3_IRQn 1 */
}

/**
  * @brief This function handles TIM6 global interrupt, DAC1 and DAC2 underrun error interrupts.
  */
/**
 * @brief  TIM6 更新中断：整个系统的 1 ms 节拍源
 * @note   具体业务在 main.c 的 HAL_TIM_PeriodElapsedCallback 里，按 1 ms 计数分频。
 */
void TIM6_DAC_IRQHandler(void)
{
  /* USER CODE BEGIN TIM6_DAC_IRQn 0 */

  /* USER CODE END TIM6_DAC_IRQn 0 */
  HAL_TIM_IRQHandler(&htim6);
  /* USER CODE BEGIN TIM6_DAC_IRQn 1 */

  /* USER CODE END TIM6_DAC_IRQn 1 */
}

/**
  * @brief This function handles USART6 global interrupt.
  */
/**
 * @brief  USART6 中断：HWT101 陀螺仪
 * @note   解析函数 HWT101_ReceiveData 由 RxCplt 回调逐字节调用，但接收从未启动，
 *         所以 Roll/Pitch/Yaw 一直是 0，车头锁定功能因此不可用。
 */
void USART6_IRQHandler(void)
{
  /* USER CODE BEGIN USART6_IRQn 0 */

  /* USER CODE END USART6_IRQn 0 */
  HAL_UART_IRQHandler(&huart6);
  /* USER CODE BEGIN USART6_IRQn 1 */

  /* USER CODE END USART6_IRQn 1 */
}

/* USER CODE BEGIN 1 */



/* USER CODE END 1 */

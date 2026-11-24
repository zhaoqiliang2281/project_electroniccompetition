/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.c
  * @brief   This file provides code for the configuration
  *          of the CAN instances.
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
#include "can.h"

/* USER CODE BEGIN 0 */

/* CAN 收发中间结构：txData/rxData 为 8 字节数据域，
 * CAN_TxMsg/CAN_RxMsg 保存帧头（ExtId/DLC/IDE），rxFrameFlag 是回帧标志。 */
__IO CAN_t can = {0};

/* USER CODE END 0 */

CAN_HandleTypeDef hcan1;

/* CAN1 init function */
void MX_CAN1_Init(void)
{

  /* USER CODE BEGIN CAN1_Init 0 */

  /* USER CODE END CAN1_Init 0 */

  /* USER CODE BEGIN CAN1_Init 1 */

  /* USER CODE END CAN1_Init 1 */
  /* 位时序：APB1 = 42 MHz，Prescaler 14 -> 3 MHz 时间片，
   * 1(SJW) + BS1 4 + BS2 1 = 6 tq -> 3 MHz / 6 = 500 kbps。
   * 改主频或改波特率必须同步改这三个值，否则 Emm_V5 完全不应答。
   * AutoRetransmission = DISABLE：丢帧不重发，避免总线异常时卡死发送。 */
  hcan1.Instance = CAN1;
  hcan1.Init.Prescaler = 14;
  hcan1.Init.Mode = CAN_MODE_NORMAL;
  hcan1.Init.SyncJumpWidth = CAN_SJW_1TQ;
  hcan1.Init.TimeSeg1 = CAN_BS1_4TQ;
  hcan1.Init.TimeSeg2 = CAN_BS2_1TQ;
  hcan1.Init.TimeTriggeredMode = DISABLE;
  hcan1.Init.AutoBusOff = DISABLE;
  hcan1.Init.AutoWakeUp = DISABLE;
  hcan1.Init.AutoRetransmission = DISABLE;
  hcan1.Init.ReceiveFifoLocked = DISABLE;
  hcan1.Init.TransmitFifoPriority = DISABLE;
  if (HAL_CAN_Init(&hcan1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN CAN1_Init 2 */

  /* USER CODE END CAN1_Init 2 */

}

/* 引脚复用：PA11 = CAN1_RX、PA12 = CAN1_TX，AF9；外部需 120 Ω 终端电阻 */
void HAL_CAN_MspInit(CAN_HandleTypeDef* canHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspInit 0 */

  /* USER CODE END CAN1_MspInit 0 */
    /* CAN1 clock enable */
    __HAL_RCC_CAN1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_12;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF9_CAN1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* CAN1 interrupt Init */
    HAL_NVIC_SetPriority(CAN1_RX0_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspInit 1 */

  /* USER CODE END CAN1_MspInit 1 */
  }
}

void HAL_CAN_MspDeInit(CAN_HandleTypeDef* canHandle)
{

  if(canHandle->Instance==CAN1)
  {
  /* USER CODE BEGIN CAN1_MspDeInit 0 */

  /* USER CODE END CAN1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_CAN1_CLK_DISABLE();

    /**CAN1 GPIO Configuration
    PA11     ------> CAN1_RX
    PA12     ------> CAN1_TX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_11|GPIO_PIN_12);

    /* CAN1 interrupt Deinit */
    HAL_NVIC_DisableIRQ(CAN1_RX0_IRQn);
  /* USER CODE BEGIN CAN1_MspDeInit 1 */

  /* USER CODE END CAN1_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */

/**
	* @brief   初始化滤波器
	* @param   无
	* @retval  无
	*/
/**
 * @brief  配置 CAN1 接收滤波器：32 位掩码模式，掩码全 0 = 接收全部扩展帧
 * @note   掩码为 0 时“ID 与掩码与运算”恒为 0，等于不过滤；
 *         若现场存在其它 CAN 设备干扰，可在此改成只放行 Emm_V5 的 0x140/0x240 等 ID。
 *         配置失败会卡在 while 里，属于“CAN 外设没起来”的典型表现。
 */
void USER_CAN1_Filter_Init(void)
{
	// 过滤器结构体
	CAN_FilterTypeDef  sFilterConfig;

	// 设置STM32的帧ID - 扩展帧格式 - 不过滤任何数据帧
	__IO uint8_t id_o, im_o; __IO uint16_t id_l, id_h, im_l, im_h;
	id_o = (0x00);
	id_h = (uint16_t)((uint16_t)id_o >> 5);								// 高3位
	id_l = (uint16_t)((uint16_t)id_o << 11) | CAN_ID_EXT; // 低5位
	im_o = (0x00);
	im_h = (uint16_t)((uint16_t)im_o >> 5);
	im_l = (uint16_t)((uint16_t)im_o << 11) | CAN_ID_EXT;

	// 过滤器参数
	sFilterConfig.FilterBank = 0;                      		// 过滤器1
	sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;  		// 掩码模式
	sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT; 		// 32位过滤器位宽
	sFilterConfig.FilterIdHigh = id_h;               			// 过滤器标识符的高16位值
	sFilterConfig.FilterIdLow = id_l;                			// 过滤器标识符的低16位值
	sFilterConfig.FilterMaskIdHigh = im_h;           			// 过滤器屏蔽标识符的高16位值
	sFilterConfig.FilterMaskIdLow = im_l;            			// 过滤器屏蔽标识符的低16位值
	sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0; 		// 指向过滤器的FIFO为0
	sFilterConfig.FilterActivation = ENABLE;           		// 使能过滤器
	sFilterConfig.SlaveStartFilterBank = 0;           		// 从过滤器配置，用来选择从过滤器的寄存器编号
	
	// 配置并自检
	while(HAL_CAN_ConfigFilter(&hcan1, &sFilterConfig) != HAL_OK);
}

/**
	* @brief   CAN发送多个字节
	* @param   无
	* @retval  无
	*/
/**
 * @brief  把 Emm_V5 的“命令串”拆成多帧扩展帧发出（>7 字节时自动分包）
 * @param  cmd 命令缓冲：cmd[0]=目标 ID，cmd[1]=功能码，cmd[2..]=数据
 * @param  len 命令总长度（含 ID 与功能码）
 * @note   分包规则：
 *           - 数据不足 7 字节：一帧发完，DLC = 数据长度 + 1（第 1 字节放功能码）；
 *           - 超过 7 字节：每帧 7 字节数据 + 功能码，DLC = 8；
 *           - ExtId = (目标ID << 8) | 分包序号，接收端按序号拼接。
 *         发送用 while 等邮箱空闲，总线被占满时会阻塞在这。
 */
void can_SendCmd(__IO uint8_t *cmd, uint8_t len)
{
	static uint32_t TxMailbox; __IO uint8_t i = 0, j = 0, k = 0, l = 0, packNum = 0;

	// 除去ID地址和功能码后的数据长度
	j = len - 2;

	// 发送数据
	while(i < j)
	{
		// 数据个数
		k = j - i;

		// 填充缓存
		can.CAN_TxMsg.StdId = 0x00;
		can.CAN_TxMsg.ExtId = ((uint32_t)cmd[0] << 8) | (uint32_t)packNum;
		can.txData[0] = cmd[1];
		can.CAN_TxMsg.IDE = CAN_ID_EXT;
		can.CAN_TxMsg.RTR = CAN_RTR_DATA;

		// 小于8字节命令
		if(k < 8)
		{
			for(l=0; l < k; l++,i++) { can.txData[l + 1] = cmd[i + 2]; } can.CAN_TxMsg.DLC = k + 1;
		}
		// 大于8字节命令，分包发送，每包数据最多发送8个字节
		else
		{
			for(l=0; l < 7; l++,i++) { can.txData[l + 1] = cmd[i + 2]; } can.CAN_TxMsg.DLC = 8;
		}

		// 发送数据
		while(HAL_CAN_AddTxMessage((&hcan1), (CAN_TxHeaderTypeDef *)(&can.CAN_TxMsg), (uint8_t *)(&can.txData), (&TxMailbox)) != HAL_OK);

		// 记录发送的第几包的数据
		++packNum;
	}
}

/* USER CODE END 1 */

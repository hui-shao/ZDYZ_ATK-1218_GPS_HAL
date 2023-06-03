/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.c
  * @brief   This file provides code for the configuration
  *          of the USART instances.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
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
#include "usart.h"

/* USER CODE BEGIN 0 */

#include<stdarg.h>
#include<string.h>
#include<stdio.h>

#include "atk_mo1218_uart.h"

uint8_t USART1_TxBUF[USART1_MAX_SENDLEN];
uint8_t USART1_RxBUF[USART1_MAX_RECVLEN];
uint8_t USART2_TxBUF[USART2_MAX_SENDLEN];
uint8_t USART2_RxBUF[USART2_MAX_RECVLEN];
uint8_t USART3_TxBUF[USART3_MAX_SENDLEN];
uint8_t USART3_RxBUF[USART3_MAX_RECVLEN];
volatile uint16_t USART2_RxLen = 0, USART3_RxLen = 0;
volatile uint8_t USART2_RecvEndFlag = 0, USART3_RecvEndFlag = 0;
volatile uint8_t print_mode = 1;

/* USER CODE END 0 */

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart2;
UART_HandleTypeDef huart3;

/* USART1 init function */

void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}
/* USART2 init function */

void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 38400;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}
/* USART3 init function */

void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

void HAL_UART_MspInit(UART_HandleTypeDef* uartHandle)
{

  GPIO_InitTypeDef GPIO_InitStruct = {0};
  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspInit 0 */

  /* USER CODE END USART1_MspInit 0 */
    /* USART1 clock enable */
    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* USER CODE BEGIN USART1_MspInit 1 */

  /* USER CODE END USART1_MspInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspInit 0 */

  /* USER CODE END USART2_MspInit 0 */
    /* USART2 clock enable */
    __HAL_RCC_USART2_CLK_ENABLE();

    __HAL_RCC_GPIOA_CLK_ENABLE();
    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    /* USART2 interrupt Init */
    HAL_NVIC_SetPriority(USART2_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspInit 1 */

  /* USER CODE END USART2_MspInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspInit 0 */

  /* USER CODE END USART3_MspInit 0 */
    /* USART3 clock enable */
    __HAL_RCC_USART3_CLK_ENABLE();

    __HAL_RCC_GPIOB_CLK_ENABLE();
    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    /* USART3 interrupt Init */
    HAL_NVIC_SetPriority(USART3_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspInit 1 */

  /* USER CODE END USART3_MspInit 1 */
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* uartHandle)
{

  if(uartHandle->Instance==USART1)
  {
  /* USER CODE BEGIN USART1_MspDeInit 0 */

  /* USER CODE END USART1_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART1_CLK_DISABLE();

    /**USART1 GPIO Configuration
    PA9     ------> USART1_TX
    PA10     ------> USART1_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

  /* USER CODE BEGIN USART1_MspDeInit 1 */

  /* USER CODE END USART1_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART2)
  {
  /* USER CODE BEGIN USART2_MspDeInit 0 */

  /* USER CODE END USART2_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART2_CLK_DISABLE();

    /**USART2 GPIO Configuration
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* USART2 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  /* USER CODE BEGIN USART2_MspDeInit 1 */

  /* USER CODE END USART2_MspDeInit 1 */
  }
  else if(uartHandle->Instance==USART3)
  {
  /* USER CODE BEGIN USART3_MspDeInit 0 */

  /* USER CODE END USART3_MspDeInit 0 */
    /* Peripheral clock disable */
    __HAL_RCC_USART3_CLK_DISABLE();

    /**USART3 GPIO Configuration
    PB10     ------> USART3_TX
    PB11     ------> USART3_RX
    */
    HAL_GPIO_DeInit(GPIOB, GPIO_PIN_10|GPIO_PIN_11);

    /* USART3 interrupt Deinit */
    HAL_NVIC_DisableIRQ(USART3_IRQn);
  /* USER CODE BEGIN USART3_MspDeInit 1 */

  /* USER CODE END USART3_MspDeInit 1 */
  }
}

/* USER CODE BEGIN 1 */


/**
 * @description: 串口空闲接受中断函数
 * @param {UART_HandleTypeDef} *huart
 * @param {uint16_t} Size
 * @return {*}
 */
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	if(huart->Instance==USART2)
	{
    u1_printf("(DBG) USART2 IDLE\r\n"); // for test
    g_uart_rx_frame.sta.len = Size; /* 覆盖之前收到的数据 */
    g_uart_rx_frame.sta.finsh = 1;  /* 标记帧接收完成 */
    u2_start_idle_receive();
  }
  else if(huart->Instance==USART3)
	{
    USART3_RxLen = Size;
    USART3_RecvEndFlag = 1;
    // 进入此中断，USART3 接受到的数据存放在 USART3_RxBUF 里面，可以调试以下看看是什么，然后做判断。
    // 判断举例：
    if (USART3_RxBUF[1] == 0x01)
    {
      print_mode = 1; // print mode 用于后续控制打印给串口屏的内容
    }
    else if (USART3_RxBUF[1] == 0x02)
    {
      print_mode = 2;
    }
    
    // 以下 todo: for test only, 实现把接受到的数据从 USART3 重新发送出去。
    HAL_UART_Transmit(&huart3, USART3_RxBUF, 10, 200);
    u3_start_idle_receive();
	}
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if (__HAL_UART_GET_FLAG(huart, UART_FLAG_ORE) != RESET) /* UART接收过载错误中断 */
  {
    __HAL_UART_CLEAR_OREFLAG(huart); /* 清除接收过载错误中断标志 */
    (void)huart->Instance->SR;       /* 先读SR寄存器，再读DR寄存器 */
    (void)huart->Instance->DR;
    huart->RxState = HAL_UART_STATE_READY;
    huart->Lock = HAL_UNLOCKED;
  }
  if (huart->Instance == USART2)
  {
    u1_printf("(DBG) USART2 ORE\r\n");
    u2_start_idle_receive();
  }
  else if (huart->Instance == USART3)
  {
    u1_printf("(DBG) USART3 ORE\r\n");
    u3_start_idle_receive();
  }
  else
  {
    u1_printf("(DBG) ORE ERROR\r\n");
  }
  
  
}

/**
 * @description: 启动串口2接受空闲中断
 * @return {*}
 */
void u2_start_idle_receive(void)
{
  // USART2_RecvEndFlag = 0;
  // memset(USART2_RxBUF, 0, USART2_MAX_RECVLEN);
  // HAL_UARTEx_ReceiveToIdle_IT(&huart2, USART2_RxBUF, USART2_MAX_RECVLEN);
  HAL_UARTEx_ReceiveToIdle_IT(&huart2, g_uart_rx_frame.buf, ATK_MO1218_UART_RX_BUF_SIZE);
}

/**
 * @description: 启动串口2接受空闲中断
 * @return {*}
 */
void u3_start_idle_receive(void)
{
  USART3_RecvEndFlag = 0;
  memset(USART3_RxBUF, 0, USART3_MAX_RECVLEN);
  HAL_UARTEx_ReceiveToIdle_IT(&huart3, USART3_RxBUF, USART3_MAX_RECVLEN);
}

/**
 * @description: 向 USART1 串口发送缓冲区中字符串 注意，\00将被移除
 * @param {char} *fmt 需要发送的字符串
 * @return {*}
 */
void u1_printf(char *fmt, ...)
{
  memset(USART1_TxBUF, 0, USART1_MAX_SENDLEN);
  uint16_t i, j;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char *)USART1_TxBUF, fmt, ap);
  va_end(ap);

  for (i = 0; i < USART1_MAX_SENDLEN; i++)
  {
    j = i + 1;
    if (USART1_TxBUF[i] == '\00')
    {
      for (; j < USART1_MAX_SENDLEN; j++)
      {
        USART1_TxBUF[j - 1] = USART1_TxBUF[j];
      }
    }
  }
  i = strlen((const char *)USART1_TxBUF);
  HAL_UART_Transmit(&huart1, (uint8_t *)USART1_TxBUF, i, 200);
}

/**
 * @description: 向 USART2 串口发送缓冲区中字符串 注意，\00将被移除
 * @param {char} *fmt 需要发送的字符串
 * @return {*}
 */
void u2_printf(char *fmt, ...)
{
  memset(USART2_TxBUF, 0, USART2_MAX_SENDLEN);
  uint16_t i, j;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char *)USART2_TxBUF, fmt, ap);
  va_end(ap);

  for (i = 0; i < USART2_MAX_SENDLEN; i++)
  {
    j = i + 1;
    if (USART2_TxBUF[i] == '\00')
    {
      for (; j < USART2_MAX_SENDLEN; j++)
      {
        USART2_TxBUF[j - 1] = USART2_TxBUF[j];
      }
    }
  }
  i = strlen((const char *)USART2_TxBUF);
  HAL_UART_Transmit(&huart2, (uint8_t *)USART2_TxBUF, i, 200);
}

/**
 * @description: 向 USART3 串口发送缓冲区中字符串 注意，\00将被移除
 * @param {char} *fmt 需要发送的字符串
 * @return {*}
 */
void u3_printf(char *fmt, ...)
{
  memset(USART3_TxBUF, 0, USART3_MAX_SENDLEN);
  uint16_t i, j;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char *)USART3_TxBUF, fmt, ap);
  va_end(ap);

  for (i = 0; i < USART3_MAX_SENDLEN; i++)
  {
    j = i + 1;
    if (USART3_TxBUF[i] == '\00')
    {
      for (; j < USART3_MAX_SENDLEN; j++)
      {
        USART3_TxBUF[j - 1] = USART3_TxBUF[j];
      }
    }
  }
  i = strlen((const char *)USART3_TxBUF);
  HAL_UART_Transmit(&huart3, (uint8_t *)USART3_TxBUF, i, 200);
}

/* USER CODE END 1 */

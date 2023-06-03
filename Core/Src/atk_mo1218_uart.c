/**
 ****************************************************************************************************
 * @file        atk_mo1218_uart.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MO1218模块UART接口驱动代码
 * @license     Copyright (c) 2020-2032, 广州市星翼电子科技有限公司
 ****************************************************************************************************
 * @attention
 *
 * 实验平台:正点原子 MiniSTM32 V4开发板
 * 在线视频:www.yuanzige.com
 * 技术论坛:www.openedv.com
 * 公司网址:www.alientek.com
 * 购买地址:openedv.taobao.com
 *
 ****************************************************************************************************
 */

#include "usart.h"
#include "atk_mo1218_uart.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

static UART_HandleTypeDef g_uart_handle;                   /* ATK-MO1218 UART */
g_uart_rx_frame_t g_uart_rx_frame = {0};                   /* ATK-MO1218 UART接收帧缓冲信息结构体 */
static uint8_t g_uart_tx_buf[ATK_MO1218_UART_TX_BUF_SIZE]; /* ATK-MO1218 UART发送缓冲 */

/**
 * @brief       ATK-MO1218 UART发送数据
 * @param       dat: 待发送的数据
 *              len: 待发送数据的长度
 * @retval      无
 */
void atk_mo1218_uart_send(uint8_t *dat, uint8_t len)
{
    HAL_UART_Transmit(&g_uart_handle, dat, len, HAL_MAX_DELAY);
}

/**
 * @brief       ATK-MO1218 UART printf
 * @param       fmt: 待打印的数据
 * @retval      无
 */
void atk_mo1218_uart_printf(char *fmt, ...)
{
    va_list ap;
    uint16_t len;
    
    va_start(ap, fmt);
    vsprintf((char *)g_uart_tx_buf, fmt, ap);
    va_end(ap);
    
    len = strlen((const char *)g_uart_tx_buf);
    HAL_UART_Transmit(&g_uart_handle, g_uart_tx_buf, len, HAL_MAX_DELAY);
}

/**
 * @brief       ATK-MO1218 UART重新开始接收数据
 * @param       无
 * @retval      无
 */
void atk_mo1218_uart_rx_restart(void)
{
    g_uart_rx_frame.sta.len     = 0;
    g_uart_rx_frame.sta.finsh   = 0;
    u2_start_idle_receive(); // 自行添加的
}

/**
 * @brief       获取ATK-MO1218 UART接收到的一帧数据
 * @param       无
 * @retval      NULL: 未接收到一帧数据
 *              其他: 接收到的一帧数据
 */
uint8_t *atk_mo1218_uart_rx_get_frame(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = '\0';
        return g_uart_rx_frame.buf;
    }
    else
    {
        return NULL;
    }
}

/**
 * @brief       获取ATK-MO1218 UART接收到的一帧数据的长度
 * @param       无
 * @retval      0   : 未接收到一帧数据
 *              其他: 接收到的一帧数据的长度
 */
uint16_t atk_mo1218_uart_rx_get_frame_len(void)
{
    if (g_uart_rx_frame.sta.finsh == 1)
    {
        return g_uart_rx_frame.sta.len;
    }
    else
    {
        return 0;
    }
}

/**
 * @brief       ATK-MO1218 UART初始化
 * @param       baudrate: UART通讯波特率
 * @retval      无
 */
void atk_mo1218_uart_init(uint32_t baudrate)
{
    g_uart_handle.Instance          = ATK_MO1218_UART_INTERFACE;    /* ATK-MO1218 UART */
    g_uart_handle.Init.BaudRate     = baudrate;                     /* 波特率 */
    g_uart_handle.Init.WordLength   = UART_WORDLENGTH_8B;           /* 数据位 */
    g_uart_handle.Init.StopBits     = UART_STOPBITS_1;              /* 停止位 */
    g_uart_handle.Init.Parity       = UART_PARITY_NONE;             /* 校验位 */
    g_uart_handle.Init.Mode         = UART_MODE_TX_RX;              /* 收发模式 */
    g_uart_handle.Init.HwFlowCtl    = UART_HWCONTROL_NONE;          /* 无硬件流控 */
    g_uart_handle.Init.OverSampling = UART_OVERSAMPLING_16;         /* 过采样 */
    if (HAL_UART_Init(&g_uart_handle) != HAL_OK)
    {
        Error_Handler();
    }
    /* 使能ATK-MO1218 UART
     * HAL_UART_Init()会调用函数HAL_UART_MspInit()
     * 该函数定义在文件usart.c中
     */

}

// /**
//  * @brief       ATK-MO1218 UART中断回调函数
//  * @param       无
//  * @retval      无
//  */
// void ATK_MO1218_UART_IRQHandler(void)
// {
//     uint8_t tmp;
    
//     if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_ORE) != RESET)        /* UART接收过载错误中断 */
//     {
//         __HAL_UART_CLEAR_OREFLAG(&g_uart_handle);                           /* 清除接收过载错误中断标志 */
//         (void)g_uart_handle.Instance->SR;                                   /* 先读SR寄存器，再读DR寄存器 */
//         (void)g_uart_handle.Instance->DR;
//     }
    
//     if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_RXNE) != RESET)       /* UART接收中断 */
//     {
//         HAL_UART_Receive(&g_uart_handle, &tmp, 1, HAL_MAX_DELAY);           /* UART接收数据 */
        
//         if (g_uart_rx_frame.sta.len < (ATK_MO1218_UART_RX_BUF_SIZE - 1))    /* 判断UART接收缓冲是否溢出
//                                                                              * 留出一位给结束符'\0'
//                                                                              */
//         {
//             g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp;             /* 将接收到的数据写入缓冲 */
//             g_uart_rx_frame.sta.len++;                                      /* 更新接收到的数据长度 */
//         }
//         else                                                                /* UART接收缓冲溢出 */
//         {
//             g_uart_rx_frame.sta.len = 0;                                    /* 覆盖之前收到的数据 */
//             g_uart_rx_frame.buf[g_uart_rx_frame.sta.len] = tmp;             /* 将接收到的数据写入缓冲 */
//             g_uart_rx_frame.sta.len++;                                      /* 更新接收到的数据长度 */
//         }
//     }
    
//     if (__HAL_UART_GET_FLAG(&g_uart_handle, UART_FLAG_IDLE) != RESET)       /* UART总线空闲中断 */
//     {
//         g_uart_rx_frame.sta.finsh = 1;                                      /* 标记帧接收完成 */
        
//         __HAL_UART_CLEAR_IDLEFLAG(&g_uart_handle);                          /* 清除UART总线空闲中断 */
//     }
// }

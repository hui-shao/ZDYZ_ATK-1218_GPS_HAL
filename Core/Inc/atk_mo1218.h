/**
 ****************************************************************************************************
 * @file        atk_mo1218.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MO1218模块驱动代码
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

#ifndef __ATK_MO1218_H
#define __ATK_MO1218_H

#include "atk_mo1218_bin_msg.h"
#include "atk_mo1218_nmea_msg.h"
#include "atk_mo1218_uart.h"

#include "main.h"

/* ATK-MO1218模块UTC结构体 */
typedef struct
{
    uint16_t year;                                  /* 年 */
    uint8_t month;                                  /* 月 */
    uint8_t day;                                    /* 日 */
    uint8_t hour;                                   /* 时 */
    uint8_t minute;                                 /* 分 */
    uint8_t second;                                 /* 秒 */
    uint16_t millisecond;                           /* 毫秒 */
} atk_mo1218_time_t;

/* ATK-MO1218模块位置结构体 */
typedef struct
{
    atk_mo1218_latitude_t latitude;                 /* 纬度信息 */
    atk_mo1218_longitude_t longitude;               /* 经度信息 */
} atk_mo1218_position_t;

/* ATK-MO1218模块定位信息结构体 */
typedef struct
{
    atk_mo1218_gps_quality_indicator_t quality;     /* 定位质量 */
    uint8_t satellite_num;                          /* 用于定位的卫星数量 */
    atk_mo1218_fix_type_t type;                     /* 定位类型 */
    uint16_t satellite_id[12];                      /* 用于定位的卫星编号 */
    uint16_t pdop;                                  /* 位置精度因子（扩大10倍） */
    uint16_t hdop;                                  /* 水平精度因子（扩大10倍） */
    uint16_t vdop;                                  /* 垂直精度因子（扩大10倍） */
} atk_mo1218_fix_info_t;

/* ATK-MO1218模块可见卫星信息结构体 */
typedef struct
{
    uint8_t satellite_num;                          /* 可见卫星数量 */
    atk_mo1218_satellite_info_t satellite_info[12]; /* 可见卫星信息 */
} atk_mo1218_visible_satellite_info_t;

/* 错误代码 */
#define ATK_MO1218_EOK      0                       /* 没有错误 */
#define ATK_MO1218_ERROR    1                       /* 错误 */
#define ATK_MO1218_ETIMEOUT 2                       /* 超时错误 */
#define ATK_MO1218_EINVAL   3                       /* 参数错误 */

/* 操作函数 */
uint8_t atk_mo1218_init(uint32_t baudrate);                                                                                                                                                                                                                                                     /* ATK-MO1218初始化 */
uint8_t atk_mo1218_update(atk_mo1218_time_t *utc, atk_mo1218_position_t *position, int16_t *altitude, uint16_t *speed, atk_mo1218_fix_info_t *fix_info, atk_mo1218_visible_satellite_info_t *gps_satellite_info, atk_mo1218_visible_satellite_info_t *beidou_satellite_info, uint32_t timeout); /* 获取并更新ATK-MO1218模块数据 */

#endif

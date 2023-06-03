/**
 ****************************************************************************************************
 * @file        atk_mo1218_bin_msg.h
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21
 * @brief       ATK-MO1218模块Binary Message驱动代码
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

#ifndef __ATK_MO1218_BIN_MSG_H
#define __ATK_MO1218_BIN_MSG_H

#include "main.h"

/* 等待ATK-MO1218模块Binary Message响应超时时间 */
#define ATK_MO1218_BIN_MSG_TIMEOUT                      50

/* ATK-MO1218模块Binary Message中Playload的最大大小 */
#define ATK_MO1218_BIN_MSG_PLAYLOAD_MAX_LEN             0x0057

/* ATK-MO1218重新启动枚举 */
typedef enum
{
    ATK_MO1218_RESTART_HOT = 0x00,                      /* 热启动 */
    ATK_MO1218_RESTART_WARM,                            /* 温启动 */
    ATK_MO1218_RESTART_COLD,                            /* 冷启动 */
} atk_mo1218_restart_t;

/* ATK-MO1218模块恢复出厂设置类型枚举 */
typedef enum
{
    ATK_MO1218_FACTORY_RESET_NOREBOOT = 0x00,           /* 恢复出厂设置后不重启 */
    ATK_MO1218_FACTORY_RESET_REBOOT,                    /* 恢复出厂设置后重启 */
} atk_mo1218_factory_reset_type_t;

/* ATK-MO1218模块串口波特率枚举 */
typedef enum
{
    ATK_MO1218_SERIAL_BAUDRATE_4800 = 0x00,             /* 4800bps */
    ATK_MO1218_SERIAL_BAUDRATE_9600,                    /* 9600bps */
    ATK_MO1218_SERIAL_BAUDRATE_19200,                   /* 19200bps */
    ATK_MO1218_SERIAL_BAUDRATE_38400,                   /* 38400bps */
    ATK_MO1218_SERIAL_BAUDRATE_57600,                   /* 57600bps */
    ATK_MO1218_SERIAL_BAUDRATE_115200,                  /* 115200bps */
    ATK_MO1218_SERIAL_BAUDRATE_230400,                  /* 230400bps */
    ATK_MO1218_SERIAL_BAUDRATE_460800,                  /* 460800bps */
    ATK_MO1218_SERIAL_BAUDRATE_921600,                  /* 921600bps */
} atk_mo1218_serial_baudrate_t;

/* ATK-MO1218模块配置保存方式枚举 */
typedef enum
{
    ATK_MO1218_SAVE_SRAM = 0x00,                        /* 保存到SRAM（配置掉电丢失） */
    ATK_MO1218_SAVE_SRAM_FLASH,                         /* 保存到SRAM和Flash（配置掉电不丢失） */
    ATK_MO1218_SAVE_TEMP,                               /* 暂时的 */
} atk_mo1218_save_type_t;

/* ATK-MO1218模块输出类型枚举 */
typedef enum
{
    ATK_MO1218_NOOUTPUT = 0x00,                         /* 无输出 */
    ATK_MO1218_OUTPUT_NMEA,                             /* 输出NMEA信息 */
    ATK_MO1218_OUTPUT_BINARY,                           /* 输出Binary信息 */
} atk_mo1218_output_type_t;

/* ATK-MO1218模块系统电源模式枚举 */
typedef enum
{
    ATK_MO1218_POWER_MODE_NORMAL = 0x00,                /* 正常模式 */
    ATK_MO1218_POWER_MODE_SAVE,                         /* 省电模式 */
} atk_mo1218_power_mode_t;

/* ATK-MO1218模块位置更新频率枚举 */
typedef enum
{
    ATK_MO1218_POSITION_RATE_1HZ = 0x00,                /* 1Hz */
    ATK_MO1218_POSITION_RATE_2HZ,                       /* 2Hz */
    ATK_MO1218_POSITION_RATE_4HZ,                       /* 4Hz，串口波特率需高于38400bps */
    ATK_MO1218_POSITION_RATE_5HZ,                       /* 5Hz，串口波特率需高于38400bps */
    ATK_MO1218_POSITION_RATE_8HZ,                       /* 8Hz，串口波特率需高于38400bps */
    ATK_MO1218_POSITION_RATE_10HZ,                      /* 10Hz，串口波特率需高于38400bps */
    ATK_MO1218_POSITION_RATE_20HZ,                      /* 20Hz，串口波特率需高于115200bps */
    ATK_MO1218_POSITION_RATE_25HZ,                      /* 25Hz，串口波特率需高于115200bps */
    ATK_MO1218_POSITION_RATE_40HZ,                      /* 40Hz，串口波特率需高于921600bps */
    ATK_MO1218_POSITION_RATE_50HZ,                      /* 50Hz，串口波特率需高于921600bps */
} atk_mo1218_position_rate_t;

/* ATK-MO1218模块DOP模式枚举 */
typedef enum
{
    ATK_MO1218_DOP_MODE_DISABLE = 0x00,                 /* 关闭 */
    ATK_MO1218_DOP_MODE_AUTO,                           /* 自动 */
    ATK_MO1218_DOP_MODE_PDOP,                           /* 仅PDOP */
    ATK_MO1218_DOP_MODE_HDOP,                           /* 仅HDOP */
    ATK_MO1218_DOP_MODE_GDOP,                           /* 仅GDOP */
} atk_mo1218_dop_mode_t;

/* ATK-MO1218模块Elevation和CNR模式枚举 */
typedef enum
{
    ATK_MO1218_ELEVATION_CNR_MODE_DISABLE = 0x00,       /* 关闭 */
    ATK_MO1218_ELEVATION_CNR_MODE_BOTH,                 /* Elevation和CNR */
    ATK_MO1218_ELEVATION_CNR_MODE_ELEVATION,            /* 仅Elevation */
    ATK_MO1218_ELEVATION_CNR_MODE_CNR,                  /* 仅CNR */
} atk_mo1218_elevation_cnr_mode_t;

/* ATK-MO1218模块Position Pinning状态枚举 */
typedef enum
{
    ATK_MO1218_POSITION_PINNING_DEFAULT = 0x00,         /* 默认 */
    ATK_MO1218_POSITION_PINNING_ENABLE,                 /* 使能 */
    ATK_MO1218_POSITION_PINNING_DISABLE,                /* 关闭 */
} atk_mo1218_position_pinning_t;

/* ATK-MO1218模块SBAS使能状态枚举 */
typedef enum
{
    ATK_MO1218_SBAS_DISABLE = 0x00,                     /* 关闭 */
    ATK_MO1218_SBAS_ENABLE,                             /* 使能 */
} atk_mo1218_sbas_enable_t;

/* ATK-MO1218模块SBAS用于导航使能状态枚举 */
typedef enum
{
    ATK_MO1218_SBAS_NAV_DISABLE = 0x00,                 /* 关闭 */
    ATK_MO1218_SBAS_NAV_ENABLE,                         /* 使能 */
} atk_mo1218_sbas_nav_enable_t;

/* ATK-MO1218模块SBAS Correction使能状态枚举 */
typedef enum
{
    ATK_MO1218_SBAS_CORRECTION_DISABLE = 0x00,          /* 关闭 */
    ATK_MO1218_SBAS_CORRECTION_ENABLE,                  /* 使能 */
} atk_mo1218_sbas_correction_enable_t;

/* ATK-MO1218模块SBAS WAAS使能状态枚举 */
typedef enum
{
    ATK_MO1218_SBAS_WAAS_DISABLE = 0x00,                /* 关闭 */
    ATK_MO1218_SBAS_WAAS_ENABLE,                        /* 使能 */
} atk_mo1218_sbas_waas_enable_t;

/* ATK-MO1218模块SBAS EGNOS使能状态枚举 */
typedef enum
{
    ATK_MO1218_SBAS_EGNOS_DISABLE = 0x00,               /* 关闭 */
    ATK_MO1218_SBAS_EGNOS_ENABLE,                       /* 使能 */
} atk_mo1218_sbas_egnos_enable_t;

/* ATK-MO1218模块SBAS MSAS使能状态枚举 */
typedef enum
{
    ATK_MO1218_SBAS_MSAS_DISABLE = 0x00,                /* 关闭 */
    ATK_MO1218_SBAS_MSAS_ENABLE,                        /* 使能 */
} atk_mo1218_sbas_msas_enable_t;

/* ATK-MO1218模块QZSS使能状态枚举 */
typedef enum
{
    ATK_MO1218_QZSS_DISABLE = 0x00,                     /* 关闭 */
    ATK_MO1218_QZSS_ENABLE,                             /* 使能 */
} atk_mo1218_qzss_enable_t;

/* ATK-MO1218模块SAEE使能状态枚举 */
typedef enum
{
    ATK_MO1218_SAEE_DEFAULT = 0x00,                     /* 默认 */
    ATK_MO1218_SAEE_ENABLE,                             /* 使能 */
    ATK_MO1218_SAEE_DISABLE,                            /* 关闭 */
} atk_mo1218_saee_enable_t;

/* ATK-MO1218模块启动故障枚举 */
typedef enum
{
    ATK_MO1218_BOOT_FROM_FLASH_OK = 0x00,               /* 从Flash启动成功 */
    ATK_MO1218_BOOT_FROM_ROM,                           /* 从ROM启动，因为从Flash启动失败 */
} atk_mo1218_boot_fail_t;

/* ATK-MO1218模块干扰检测状态枚举 */
typedef enum
{
    ATK_MO1218_INTERENCE_DETECTION_DISABLE = 0x00,      /* 关闭 */
    ATK_MO1218_INTERENCE_DETECTION_ENABLE,              /* 使能 */
} atk_mo1218_interence_detection_enable_t;

/* ATK-MO1218模块干扰状态枚举 */
typedef enum
{
    ATK_MO1218_INTERENCE_UNKNOWN = 0x00,                /* 未知 */
    ATK_MO1218_INTERENCE_NO,                            /* 无干扰 */
    ATK_MO1218_INTERENCE_LITE,                          /* 轻微干扰 */
    ATK_MO1218_INTERENCE_CRITICAL,                      /* 严重干扰 */
} atk_mo1218_interence_status_t;

/* ATK-MO1218模块导航模式枚举 */
typedef enum
{
    ATK_MO1218_NAVIGATION_MODE_AUTO = 0x00,             /* 自动模式 */
    ATK_MO1218_NAVIGATION_MODE_PRDESTRIAN,              /* 行人模式 */
    ATK_MO1218_NAVIGATION_MODE_CAR,                     /* 汽车模式 */
    ATK_MO1218_NAVIGATION_MODE_MARINE,                  /* 航海模式 */
    ATK_MO1218_NAVIGATION_MODE_BALLOON,                 /* 气球模式 */
    ATK_MO1218_NAVIGATION_MODE_AIRBORNE,                /* 飞行模式 */
} atk_mo1218_navigation_mode_t;

/* ATK-MO1218模块GNSS类型枚举 */
typedef enum
{
    ATK_MO1218_GNSS_GPS = 0x00,                         /* GPS */
    ATK_MO1218_GNSS_BEIDOU,                             /* 北斗 */
    ATK_MO1218_GNSS_GPS_BEIDOU,                         /* GPS和北斗 */
} atk_mo1218_gnss_for_navigation_t;

/* ATK-MO1218软件版本结构体定义 */
typedef struct
{
    struct
    {
        uint8_t x1;
        uint8_t y1;
        uint8_t z1;
    } kernel;                                           /* Kernel Version: x1.y1.z1 */
    struct
    {
        uint8_t x1;
        uint8_t y1;
        uint8_t z1;
    } odm;                                              /* ODM Version: x1.y1.z1 */
    struct
    {
        uint8_t yy;
        uint8_t mm;
        uint8_t dd;
    } revision;                                         /* Revision: (2000+yy).mm.dd */
} atk_mo1218_sw_version_t;

/* ATK-MO1218模块GPS星历数据结构体 */
typedef struct
{
    uint16_t sv_id;                                     /* 卫星ID */
    uint8_t subframe0[28];                              /* 星历数据子帧0 */
    uint8_t subframe1[28];                              /* 星历数据子帧1 */
    uint8_t subframe2[28];                              /* 星历数据子帧2 */
} atk_mo1218_gps_ephemeris_data_t;

/* ATK-MO1218模块Position pinning参数结构体 */
typedef struct
{
    uint16_t pinning_speed;                             /* 单位：公里/时 */
    uint16_t pinning_cnt;                               /* 单位：秒 */
    uint16_t unpinning_speed;                           /* 单位：公里/时 */
    uint16_t unpinning_cnt;                             /* 单位：秒 */
    uint16_t unpinning_distance;                        /* 单位：米 */
} atk_mo1218_position_pinning_parameter_t;

/* ATK-MO1218模块Position pinning状态数据结构体 */
typedef struct
{
    atk_mo1218_position_pinning_t status;               /* 使能位 */
    atk_mo1218_position_pinning_parameter_t parameter;  /* 参数 */
} atk_mo1218_position_pinning_status_t;

/* ATK-MO1218模块SBAS参数结构体 */
typedef struct
{
    atk_mo1218_sbas_enable_t enable;                    /* SBAS使能位 */
    atk_mo1218_sbas_nav_enable_t navigation;            /* 用于Navigation使能位 */
    uint8_t ranging_ura_mask;                           /* UAR掩码 */
    atk_mo1218_sbas_correction_enable_t correction;     /* Correction使能位 */
    uint8_t num_tracking_channel;                       /* 通道数量 */
    atk_mo1218_sbas_waas_enable_t waas;                 /* WAAS使能位 */
    atk_mo1218_sbas_egnos_enable_t egnos;               /* EGNOS使能位 */
    atk_mo1218_sbas_msas_enable_t msas;                 /* MSAM使能位 */
} atk_mo1218_sbas_parameter_t;

/* ATK-MO1218模块QZSS参数结构体 */
typedef struct
{
    atk_mo1218_qzss_enable_t enable;                    /* QZSS使能位 */
    uint8_t num_tracking_channel;                       /* 通道数量 */
} atk_mo1218_qzss_parameter_t;

/* ATK-MO1218模块SAEE参数结构体 */
typedef struct
{
    atk_mo1218_saee_enable_t enable;                    /* SAEE使能位 */
} atk_mo1218_saee_parameter_t;

/* ATK-MO1218模块启动状态信息结构体 */
typedef struct
{
    atk_mo1218_boot_fail_t fail_over;                   /* 故障启动 */
} atk_mo1218_boot_status_t;

/* ATK-MO1218模块干扰检测状态结构体 */
typedef struct
{
    atk_mo1218_interence_detection_enable_t enable;     /* 干扰检测使能状态 */
    atk_mo1218_interence_status_t interence_status;     /* 干扰检测结果 */
} atk_mo1218_interence_detection_status_t;

/* 操作函数 */
uint8_t atk_mo1218_send_bin_msg(uint8_t *playload, uint16_t pl, uint16_t timeout);                                                                                                                                                      /* 往ATK-MO1218发送Binary Message */
uint8_t atk_mo1218_restart(atk_mo1218_restart_t restart);                                                                                                                                                                               /* ATK-MO1218模块系统重启 */
uint8_t atk_mo1218_get_sw_version(atk_mo1218_sw_version_t *version);                                                                                                                                                                    /* 获取ATK-MO1218模块软件版本 */
uint8_t atk_mo1218_get_sw_crc(uint16_t *crc);                                                                                                                                                                                           /* 获取ATK-MO1218模块软件CRC值 */
uint8_t atk_mo1218_factory_reset(atk_mo1218_factory_reset_type_t type);                                                                                                                                                                 /* ATK-MO1218模块恢复出厂设置 */
uint8_t atk_mo1218_config_serial(atk_mo1218_serial_baudrate_t baudrate, atk_mo1218_save_type_t save_type);                                                                                                                              /* 配置ATK-MO1218模块串口 */
uint8_t atk_mo1218_config_nmea_msg(uint8_t gga, uint8_t gsa, uint8_t gsv, uint8_t gll, uint8_t rmc, uint8_t vtg, uint8_t zda, atk_mo1218_save_type_t save_type);                                                                        /* 配置ATK-MO1218模块NMEA输出信息间隔 */
uint8_t atk_mo1218_config_output_type(atk_mo1218_output_type_t output_type, atk_mo1218_save_type_t save_type);                                                                                                                          /* 配置ATK-MO1218模块输出消息类型 */
uint8_t atk_mo1218_config_power_mode(atk_mo1218_power_mode_t mode, atk_mo1218_save_type_t save_type);                                                                                                                                   /* 配置ATK-MO1218模块电源模式 */
uint8_t atk_mo1218_config_position_rate(atk_mo1218_position_rate_t rate, atk_mo1218_save_type_t save_type);                                                                                                                             /* 配置ATK-MO1218模块位置更新频率 */
uint8_t atk_mo1218_get_position_rate(atk_mo1218_position_rate_t *rate);                                                                                                                                                                 /* 获取ATK-MO1218模块位置更新频率 */
uint8_t atk_mo1218_config_navigation_interval(uint8_t interval, atk_mo1218_save_type_t save_type);                                                                                                                                      /* 配置ATK-MO1218模块导航数据消息间隔 */
uint8_t atk_mo1218_get_power_mode(atk_mo1218_power_mode_t *mode);                                                                                                                                                                       /* 获取ATK-MO1218模块电源模式 */
uint8_t atk_mo1218_config_dop_mask(atk_mo1218_dop_mode_t mode, uint16_t pdop_val, uint16_t hdop_val, uint16_t gdop_val, atk_mo1218_save_type_t save_type);                                                                              /* 配置ATK-MO1218模块DOP掩码 */
uint8_t atk_mo1218_config_evelation_cnr_mask(atk_mo1218_elevation_cnr_mode_t mode, uint8_t elevation_mask, uint8_t cnr_mask, atk_mo1218_save_type_t save_type);                                                                         /* 配置ATK-MO1218模块Elevation和CNR掩码 */
uint8_t atk_mo1218_get_datum(uint16_t *datum_index);                                                                                                                                                                                    /* 获取ATK-MO1218模块Datum */
uint8_t atk_mo1218_get_dop_mask(atk_mo1218_dop_mode_t *mode, uint16_t *pdop_val, uint16_t *hdop_val, uint16_t *gdop_val);                                                                                                               /* 获取ATK-MO1218模块DOP掩码 */
uint8_t atk_mo1218_get_evelation_cnr_mask(atk_mo1218_elevation_cnr_mode_t *mode, uint8_t *elevation_mask, uint8_t *cnr_mask);                                                                                                           /* 获取ATK-MO1218模块Elevation和CNR掩码 */
uint8_t atk_mo1218_get_gps_ephemeris(uint8_t sv, atk_mo1218_gps_ephemeris_data_t *data);                                                                                                                                                /* 获取ATK-MO1218模块GPS星历数据 */
uint8_t atk_mo1218_config_position_pinning(atk_mo1218_position_pinning_t status, atk_mo1218_save_type_t save_type);                                                                                                                     /* 配置ATK-MO1218模块Position Pinning */
uint8_t atk_mo1218_get_position_pinning_status(atk_mo1218_position_pinning_status_t *status);                                                                                                                                           /* 获取ATK-MO1218模块Position Pinning状态 */
uint8_t atk_mo1218_config_position_pinning_parameters(atk_mo1218_position_pinning_parameter_t *parameter, atk_mo1218_save_type_t save_type);                                                                                            /* 配置ATK-MO1218模块Position Pinning参数 */
uint8_t atk_mo1218_set_gps_ephemeris(atk_mo1218_gps_ephemeris_data_t *data);                                                                                                                                                            /* 设置ATK-MO1218模块GPS星历数据 */
uint8_t atk_mo1218_config_1pps_cable_delay(int32_t cable_delay, atk_mo1218_save_type_t save_type);                                                                                                                                      /* 配置ATK-MO1218模块1PPS的电缆延时 */
uint8_t atk_mo1218_get_1pps_cable_delay(int32_t *cable_delay);                                                                                                                                                                          /* 获取ATK-MO1218模块1PPS的电缆延时 */
uint8_t atk_mo1218_config_sbas(atk_mo1218_sbas_parameter_t *parameter, atk_mo1218_save_type_t save_type);                                                                                                                               /* 配置ATK-MO1218模块SBAS */
uint8_t atk_mo1218_get_sbas_status(atk_mo1218_sbas_parameter_t *parameter);                                                                                                                                                             /* 获取ATK-MO1218模块SBAS配置参数 */
uint8_t atk_mo1218_config_qzss(atk_mo1218_qzss_parameter_t *parameter, atk_mo1218_save_type_t save_type);                                                                                                                               /* 配置ATK-MO1218模块QZSS */
uint8_t atk_mo1218_get_qzss_status(atk_mo1218_qzss_parameter_t *parameter);                                                                                                                                                             /* 获取ATK-MO1218模块QZSS配置参数 */
uint8_t atk_mo1218_config_saee(atk_mo1218_saee_parameter_t *parameter, atk_mo1218_save_type_t save_type);                                                                                                                               /* 配置ATK-MO1218模块SAEE */
uint8_t atk_mo1218_get_saee_status(atk_mo1218_saee_parameter_t *parameter);                                                                                                                                                             /* 获取ATK-MO1218模块SAEE配置参数 */
uint8_t atk_mo1218_get_boot_status(atk_mo1218_boot_status_t *status);                                                                                                                                                                   /* 获取ATK-MO1218模块启动状态信息 */
uint8_t atk_mo1218_config_ext_nmea_msg(uint8_t gga, uint8_t gsa, uint8_t gsv, uint8_t gll, uint8_t rmc, uint8_t vtg, uint8_t zda, uint8_t gns, uint8_t gbs, uint8_t grs, uint8_t dtm, uint8_t gst, atk_mo1218_save_type_t save_type);   /* 配置ATK-MO1218模块扩展NMEA输出信息间隔 */
uint8_t atk_mo1218_get_ext_nmea_msg(uint8_t *gga, uint8_t *gsa, uint8_t *gsv, uint8_t *gll, uint8_t *rmc, uint8_t *vtg, uint8_t *zda, uint8_t *gns, uint8_t *gbs, uint8_t *grs, uint8_t *dtm, uint8_t *gst);                            /* 获取ATK-MO1218模块扩展NMEA输出信息间隔 */
uint8_t atk_mo1218_config_interference_detection(atk_mo1218_interence_detection_enable_t enable, atk_mo1218_save_type_t save_type);                                                                                                     /* 配置ATK-MO1218模块干扰检测 */
uint8_t atk_mo1218_get_interence_detection_status(atk_mo1218_interence_detection_status_t *status);                                                                                                                                     /* 获取ATK-MO1218模块干扰检测状态 */
uint8_t atk_mo1218_config_navigation_mode(atk_mo1218_navigation_mode_t mode, atk_mo1218_save_type_t save_type);                                                                                                                         /* 配置ATK-MO1218模块导航模式 */
uint8_t atk_mo1218_get_navigation_mode(atk_mo1218_navigation_mode_t *mode);                                                                                                                                                             /* 获取ATK-MO1218模块导航模式 */
uint8_t atk_mo1218_config_gnss_for_navigation(atk_mo1218_gnss_for_navigation_t gnss, atk_mo1218_save_type_t save_type);                                                                                                                 /* 配置ATK-MO1218模块用于导航的GNSS */
uint8_t atk_mo1218_get_gnss_for_navigation(atk_mo1218_gnss_for_navigation_t *gnss);                                                                                                                                                     /* 获取ATK-MO1218模块用于导航的GNSS */
uint8_t atk_mo1218_config_1pps_pulse_width(uint32_t pulse_width, atk_mo1218_save_type_t save_type);                                                                                                                                     /* 配置ATK-MO1218模块1PPS的脉冲宽度 */
uint8_t atk_mo1218_get_1pps_pulse_width(uint32_t *pulse_width);                                                                                                                                                                         /* 获取ATK-MO1218模块1PPS的脉冲宽度 */

#endif

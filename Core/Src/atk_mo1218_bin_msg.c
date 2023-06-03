/**
 ****************************************************************************************************
 * @file        atk_mo1218_bin_msg.c
 * @author      正点原子团队(ALIENTEK)
 * @version     V1.0
 * @date        2022-06-21 / 2023-06-03 Modify by Hui-Shao 
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

#include "atk_mo1218_bin_msg.h"
#include "atk_mo1218.h"
#include "delay.h"

/* ATK-MO1218模块Binary Message起始和结束序列 */
#define ATK_MO1218_BIN_MSG_SS       (0xA0A1)    /* Start of Sequence */
#define ATK_MO1218_BIN_MSG_ES       (0x0D0A)    /* End of Sequence */

/* ATK-MO1218模块Binary Message各段的大小 */
#define ATK_MO1218_BIN_MSG_SS_LEN   2           /* Start of Sequence */
#define ATK_MO1218_BIN_MSG_PL_LEN   2           /* Playload Length */
#define ATK_MO1218_BIN_MSG_CS_LEN   1           /* Checksum */
#define ATK_MO1218_BIN_MSG_ES_LEN   2           /* End of Sequence */

/* ATK-MO1218模块Binary Message缓冲的大小 */
#define ATK_MO1218_BIN_MSG_BUF_SIZE (       \
    /* Start of Sequence */                 \
    ATK_MO1218_BIN_MSG_SS_LEN +             \
    /* Playload Length */                   \
    ATK_MO1218_BIN_MSG_PL_LEN +             \
    /* Playload */                          \
    ATK_MO1218_BIN_MSG_PLAYLOAD_MAX_LEN +   \
    /* Checksum */                          \
    ATK_MO1218_BIN_MSG_CS_LEN +             \
    /* End of Sequence */                   \
    ATK_MO1218_BIN_MSG_ES_LEN)

/* ATK-MO1218模块Binary Message ID定义 */
/* Input System Messages */
#define ATK_MO1218_MID_01       (0x01)  /* System Restart */
#define ATK_MO1218_MID_02       (0x02)  /* Query Software version */
#define ATK_MO1218_MID_03       (0x03)  /* QUERY Software CRC */
#define ATK_MO1218_MID_04       (0x04)  /* Set Factory Defaults */
#define ATK_MO1218_MID_05       (0x05)  /* Configure Serial Port */
#define ATK_MO1218_MID_08       (0x08)  /* Configure NMEA */
#define ATK_MO1218_MID_09       (0x09)  /* Configure Message Type */
//#define ATK_MO1218_MID_0B       (0x0B)  /* Software Image Download */
#define ATK_MO1218_MID_0C       (0x0C)  /* Configure Power Mode */
#define ATK_MO1218_MID_0E       (0x0E)  /* Configure position Update Rate */
#define ATK_MO1218_MID_10       (0x10)  /* Query Position Update Rate */
#define ATK_MO1218_MID_11       (0x11)  /* Configure Navigation Data Message Interval */
#define ATK_MO1218_MID_15       (0x15)  /* Query Power Mode */
/* Input GNSS Messages */
//#define ATK_MO1218_MID_29       (0x29)  /* Configure Datum */
#define ATK_MO1218_MID_2A       (0x2A)  /* Configure DOP Mask */
#define ATK_MO1218_MID_2B       (0x2B)  /* Configure Elevation and CNR Mask */
#define ATK_MO1218_MID_2D       (0x2D)  /* Query Datum */
#define ATK_MO1218_MID_2E       (0x2E)  /* Query DOP Mask */
#define ATK_MO1218_MID_2F       (0x2F)  /* Query Elevation and CNR Mask */
#define ATK_MO1218_MID_30       (0x30)  /* Get GPS Ephemeris */
#define ATK_MO1218_MID_39       (0x39)  /* Configure Position Pinning */
#define ATK_MO1218_MID_3A       (0x3A)  /* Query Position Pinning */
#define ATK_MO1218_MID_3B       (0x3B)  /* Configure Position Pinning Parameters */
#define ATK_MO1218_MID_41       (0x41)  /* Set GPS Ephemeris */
#define ATK_MO1218_MID_45       (0x45)  /* Configure 1PPS Cable Delay */
#define ATK_MO1218_MID_46       (0x46)  /* Query 1PPS Cable Delay */
/* Message with Sub-ID */
#define ATK_MO1218_MID_62       (0x62)  /* Message ID: 0x62 */
#define ATK_MO1218_MID_63       (0x63)  /* Message ID: 0x63 */
#define ATK_MO1218_MID_64       (0x64)  /* Message ID: 0x64 */
#define ATK_MO1218_MID_65       (0x65)  /* Message ID: 0x65 */
/* Output System Messages */
#define ATK_MO1218_MID_80       (0x80)  /* Software Version */
#define ATK_MO1218_MID_81       (0x81)  /* Software CRC */
#define ATK_MO1218_MID_83       (0x83)  /* ACK */
#define ATK_MO1218_MID_84       (0x84)  /* NACK */
#define ATK_MO1218_MID_86       (0x86)  /* Position UPdate Rate */
/* Output GNSS Messages */
#define ATK_MO1218_MID_A8       (0xA8)  /* Navigation Data Message */
#define ATK_MO1218_MID_AE       (0xAE)  /* GNSS Datum */
#define ATK_MO1218_MID_AF       (0xAF)  /* GNSS DOP Mask */
#define ATK_MO1218_MID_B0       (0xB0)  /* Elevation and CNR Mask */
#define ATK_MO1218_MID_B1       (0xB1)  /* GPS Ephemeris Data */
#define ATK_MO1218_MID_B4       (0xB4)  /* GNSS Position Pinning Status */
#define ATK_MO1218_MID_B9       (0xB9)  /* GNSS Power Mode Status */
#define ATK_MO1218_MID_BB       (0xBB)  /* GNSS 1PPS Cable Delay */

/* ATK-MO1218模块Binary Message Sub-ID定义 */
/* Message ID: 0x62 */
#define ATK_MO1218_SID_62_01    (0x01)  /* Configure SBAS */
#define ATK_MO1218_SID_62_02    (0x02)  /* Query SBAS Status */
#define ATK_MO1218_SID_62_03    (0x03)  /* Configure QZSS */
#define ATK_MO1218_SID_62_04    (0x04)  /* Query QZSS Status */
#define ATK_MO1218_SID_62_80    (0x80)  /* SBAS Status */
#define ATK_MO1218_SID_62_81    (0x81)  /* QZSS Status */
/* Message ID: 0x63 */
#define ATK_MO1218_SID_63_01    (0x01)  /* Configure SAEE */
#define ATK_MO1218_SID_63_02    (0x02)  /* Query SAEE */
#define ATK_MO1218_SID_63_80    (0x80)  /* SAEE Status */
/* Message ID: 0x64 */
#define ATK_MO1218_SID_64_01    (0x01)  /* Query Boot Status */
#define ATK_MO1218_SID_64_02    (0x02)  /* Configure Extended NEMA Message Interval */
#define ATK_MO1218_SID_64_03    (0x03)  /* Query Extended NMEA Message Interval */
#define ATK_MO1218_SID_64_06    (0x06)  /* Configure Interference Detection */
#define ATK_MO1218_SID_64_07    (0x07)  /* Query Interference Detection Status */
#define ATK_MO1218_SID_64_17    (0x17)  /* Configure GNSS Navigation Mode */
#define ATK_MO1218_SID_64_18    (0x18)  /* Query GNSS Navigation Mode */
#define ATK_MO1218_SID_64_19    (0x19)  /* Configure GNSS constellation type for navigation solution */
#define ATK_MO1218_SID_64_1A    (0x1A)  /* Query GNSS constellation type for navigation solution */
#define ATK_MO1218_SID_64_80    (0x80)  /* GNSS Boot Status */
#define ATK_MO1218_SID_64_81    (0x81)  /* Extened NMEA Message Interval */
#define ATK_MO1218_SID_64_83    (0x83)  /* Interference Detection Status */
#define ATK_MO1218_SID_64_8B    (0x8B)  /* GNSS Navigtion Mode */
#define ATK_MO1218_SID_64_8C    (0x8C)  /* GNSS constellation type for nacigation solution */
/* Message ID: 0x65 */
#define ATK_MO1218_SID_65_01    (0x01)  /* Configure 1PPS Pulse Width */
#define ATK_MO1218_SID_65_02    (0x02)  /* Query 1PPS Pulse Width */
#define ATK_MO1218_SID_65_80    (0x80)  /* 1PPS Pulse Width */

/* Playload结构体（Message ID = 0x01） */
typedef struct
{
    uint8_t mid;
    uint8_t start_mode;
    uint16_t utc_year;
    uint8_t utc_month;
    uint8_t utc_day;
    uint8_t utc_hour;
    uint8_t utc_minute;
    uint8_t utc_second;
    int16_t latitude;
    int16_t longitude;
    int16_t altitude;
} __packed atk_mo1218_mid_01_playload_t;

/* Playload结构体（Message ID = 0x02） */
typedef struct
{
    uint8_t mid;
    uint8_t sw_type;
} __packed atk_mo1218_mid_02_playload_t;

/* Playload结构体（Message ID = 0x03） */
typedef struct
{
    uint8_t mid;
    uint8_t sw_type;
} __packed atk_mo1218_mid_03_playload_t;

/* Playload结构体（Message ID = 0x04） */
typedef struct
{
    uint8_t mid;
    uint8_t type;
} __packed atk_mo1218_mid_04_playload_t;

/* Playload结构体（Message ID = 0x05） */
typedef struct
{
    uint8_t mid;
    uint8_t port;
    uint8_t baudrate;
    uint8_t attributes;
} __packed atk_mo1218_mid_05_playload_t;

/* Playload结构体（Message ID = 0x08） */
typedef struct
{
    uint8_t mid;
    uint8_t gga_interval;
    uint8_t gsa_interval;
    uint8_t gsv_interval;
    uint8_t gll_interval;
    uint8_t rmc_interval;
    uint8_t vtg_interval;
    uint8_t zda_interval;
    uint8_t attributes;
} __packed atk_mo1218_mid_08_playload_t;

/* Playload结构体（Message ID = 0x09） */
typedef struct
{
    uint8_t mid;
    uint8_t type;
    uint8_t attributes;
} __packed atk_mo1218_mid_09_playload_t;

/* Playload结构体（Message ID = 0x0C） */
typedef struct
{
    uint8_t mid;
    uint8_t mode;
    uint8_t attributes;
} __packed atk_mo1218_mid_0c_playload_t;

/* Playload结构体（Message ID = 0x0E） */
typedef struct
{
    uint8_t mid;
    uint8_t rate;
    uint8_t attributes;
} __packed atk_mo1218_mid_0e_playload_t;

/* Playload结构体（Message ID = 0x10） */
typedef struct
{
    uint8_t mid;
} __packed atk_mo1218_mid_10_playload_t;

/* Playload结构体（Message ID = 0x11） */
typedef struct
{
    uint8_t mid;
    uint8_t interval;
    uint8_t attributes;
} __packed atk_mo1218_mid_11_playload_t;

/* Playload结构体（Message ID = 0x15） */
typedef struct
{
    uint8_t mid;
} __packed atk_mo1218_mid_15_playload_t;

/* Playload结构体（Message ID = 0x2A） */
typedef struct
{
    uint8_t mid;
    uint8_t mode;
    uint8_t pdop_val[2];
    uint8_t hdop_val[2];
    uint8_t gdop_val[2];
    uint8_t attributes;
} __packed atk_mo1218_mid_2a_playload_t;

/* Playload结构体（Message ID = 0x2B） */
typedef struct
{
    uint8_t mid;
    uint8_t mode;
    uint8_t elevation_mask;
    uint8_t cnr_mask;
    uint8_t attributes;
} __packed atk_mo1218_mid_2b_playload_t;

/* Playload结构体（Message ID = 0x2D） */
typedef struct
{
    uint8_t mid;
} __packed atk_mo1218_mid_2d_playload_t;

/* Playload结构体（Message ID = 0x2E） */
typedef struct
{
    uint8_t mid;
} __packed atk_mo1218_mid_2e_playload_t;

/* Playload结构体（Message ID = 0x2F） */
typedef struct
{
    uint8_t mid;
} __packed atk_mo1218_mid_2f_playload_t;

/* Playload结构体（Message ID = 0x30） */
typedef struct
{
    uint8_t mid;
    uint8_t sv;
} __packed atk_mo1218_mid_30_playload_t;

/* Playload结构体（Message ID = 0x39） */
typedef struct
{
    uint8_t mid;
    uint8_t status;
    uint8_t attributes;
} __packed atk_mo1218_mid_39_playload_t;

/* Playload结构体（Message ID = 0x3A） */
typedef struct
{
    uint8_t mid;
} __packed atk_mo1218_mid_3a_playload_t;

/* Playload结构体（Message ID = 0x3B） */
typedef struct
{
    uint8_t mid;
    uint8_t pinning_speed[2];
    uint8_t pinning_cnt[2];
    uint8_t unpinning_speed[2];
    uint8_t unpinning_cnt[2];
    uint8_t unpinning_distance[2];
    uint8_t attributes;
} __packed atk_mo1218_mid_3b_playload_t;

/* Playload结构体（Message ID = 0x41） */
typedef struct
{
    uint8_t mid;
    uint8_t sv[2];
    uint8_t sub_frame0[28];
    uint8_t sub_frame1[28];
    uint8_t sub_frame2[28];
} __packed atk_mo1218_mid_41_playload_t;

/* Playload结构体（Message ID = 0x45） */
typedef struct
{
    uint8_t mid;
    uint8_t cable_delay[4];
    uint8_t attributes;
} __packed atk_mo1218_mid_45_playload_t;

/* Playload结构体（Message ID = 0x46） */
typedef struct
{
    uint8_t mid;
} __packed atk_mo1218_mid_46_playload_t;

/* Playload结构体（Message ID = 0x62, Sub-ID = 0x01） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t enable;
    uint8_t ranging;
    uint8_t ranging_ura_mask;
    uint8_t correction;
    uint8_t num_tracking_channel;
    uint8_t subsystem_mask;
    uint8_t attributes;
} __packed atk_mo1218_mid_62_01_playload_t;

/* Playload结构体（Message ID = 0x62, Sub-ID = 0x02） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_62_02_playload_t;

/* Playload结构体（Message ID = 0x62, Sub-ID = 0x03） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t enable;
    uint8_t num_tracking_channel;
    uint8_t attributes;
} __packed atk_mo1218_mid_62_03_playload_t;

/* Playload结构体（Message ID = 0x62, Sub-ID = 0x04） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_62_04_playload_t;

/* Playload结构体（Message ID = 0x62, Sub-ID = 0x80） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t enable;
    uint8_t ranging;
    uint8_t ranging_ura_mask;
    uint8_t correction;
    uint8_t num_tracking_channel;
    uint8_t subsystem_mask;
} __packed atk_mo1218_mid_62_80_playload_t;

/* Playload结构体（Message ID = 0x62, Sub-ID = 0x81） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t enable;
    uint8_t num_tracking_channel;
} __packed atk_mo1218_mid_62_81_playload_t;

/* Playload结构体（Message ID = 0x63, Sub-ID = 0x01） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t enable;
    uint8_t attributes;
} __packed atk_mo1218_mid_63_01_playload_t;

/* Playload结构体（Message ID = 0x63, Sub-ID = 0x02） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_63_02_playload_t;

/* Playload结构体（Message ID = 0x63, Sub-ID = 0x80） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t enable;
} __packed atk_mo1218_mid_63_80_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x01） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_64_01_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x02） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t gga_interval;
    uint8_t gsa_interval;
    uint8_t gsv_interval;
    uint8_t gll_interval;
    uint8_t rmc_interval;
    uint8_t vtg_interval;
    uint8_t zda_interval;
    uint8_t gns_interval;
    uint8_t gbs_interval;
    uint8_t grs_interval;
    uint8_t dtm_interval;
    uint8_t gst_interval;
    uint8_t attributes;
} __packed atk_mo1218_mid_64_02_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x03） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_64_03_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x06） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t enable;
    uint8_t attributes;
} __packed atk_mo1218_mid_64_06_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x07） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_64_07_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x17） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t mode;
    uint8_t attributes;
} __packed atk_mo1218_mid_64_17_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x18） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_64_18_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x19） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t gnss[2];
    uint8_t attributes;
} __packed atk_mo1218_mid_64_19_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x1A） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_64_1a_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x80） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t fail_over;
    uint8_t flash_type;
} __packed atk_mo1218_mid_64_80_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x81） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t gga_interval;
    uint8_t gsa_interval;
    uint8_t gsv_interval;
    uint8_t gll_interval;
    uint8_t rmc_interval;
    uint8_t vtg_interval;
    uint8_t zda_interval;
    uint8_t gns_interval;
    uint8_t gbs_interval;
    uint8_t grs_interval;
    uint8_t dtm_interval;
    uint8_t gst_interval;
} __packed atk_mo1218_mid_64_81_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x83） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t enable;
    uint8_t interence_status;
} __packed atk_mo1218_mid_64_83_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x8B） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t mode;
} __packed atk_mo1218_mid_64_8b_playload_t;

/* Playload结构体（Message ID = 0x64, Sub-ID = 0x8C） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t gnss[2];
} __packed atk_mo1218_mid_64_8c_playload_t;

/* Playload结构体（Message ID = 0x65, Sub-ID = 0x01） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t pulse_width[4];
    uint8_t attributes;
} __packed atk_mo1218_mid_65_01_playload_t;

/* Playload结构体（Message ID = 0x65, Sub-ID = 0x02） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
} __packed atk_mo1218_mid_65_02_playload_t;

/* Playload结构体（Message ID = 0x65, Sub-ID = 0x80） */
typedef struct
{
    uint8_t mid;
    uint8_t sid;
    uint8_t pulse_width[4];
} __packed atk_mo1218_mid_65_80_playload_t;

/* Playload结构体（Message ID = 0x80） */
typedef struct
{
    uint8_t mid;
    uint8_t sw_type;
    uint8_t kernel_ver[4];
    uint8_t odm_ver[4];
    uint8_t rev[4];
} __packed atk_mo1218_mid_80_playload_t;

/* Playload结构体（Message ID = 0x81） */
typedef struct
{
    uint8_t mid;
    uint8_t sw_type;
    uint8_t crc[2];
} __packed atk_mo1218_mid_81_playload_t;

/* Playload结构体（Message ID = 0x83） */
typedef struct
{
    uint8_t mid;
    uint8_t ack_id;
} __packed atk_mo1218_mid_83_playload_t;

/* Playload结构体（Message ID = 0x83，带Sub-ID） */
typedef struct
{
    uint8_t mid;
    uint8_t ack_id[2];
} __packed atk_mo1218_mid_83_sid_playload_t;

/* Playload结构体（Message ID = 0x86） */
typedef struct
{
    uint8_t mid;
    uint8_t rate;
} __packed atk_mo1218_mid_86_playload_t;

/* Playload结构体（Message ID = 0xAE） */
typedef struct
{
    uint8_t mid;
    uint8_t datum_index[2];
} __packed atk_mo1218_mid_ae_playload_t;

/* Playload结构体（Message ID = 0xAF） */
typedef struct
{
    uint8_t mid;
    uint8_t mode;
    uint8_t pdop_val[2];
    uint8_t hdop_val[2];
    uint8_t gdop_val[2];
} __packed atk_mo1218_mid_af_playload_t;

/* Playload结构体（Message ID = 0xB0） */
typedef struct
{
    uint8_t mid;
    uint8_t mode;
    uint8_t elevation_mask;
    uint8_t cnr_mask;
} __packed atk_mo1218_mid_b0_playload_t;

/* Playload结构体（Message ID = 0xB1） */
typedef struct
{
    uint8_t mid;
    uint8_t sv[2];
    uint8_t sub_frame0[28];
    uint8_t sub_frame1[28];
    uint8_t sub_frame2[28];
} __packed atk_mo1218_mid_b1_playload_t;

/* Playload结构体（Message ID = 0xB4） */
typedef struct
{
    uint8_t mid;
    uint8_t status;
    uint8_t pinning_speed[2];
    uint8_t pinning_cnt[2];
    uint8_t unpinning_speed[2];
    uint8_t unpinning_cnt[2];
    uint8_t unpinning_distance[2];
} __packed atk_mo1218_mid_b4_playload_t;

/* Playload结构体（Message ID = 0xB9） */
typedef struct
{
    uint8_t mid;
    uint8_t mode;
} __packed atk_mo1218_mid_b9_playload_t;

/* Playload结构体（Message ID = 0xBB） */
typedef struct
{
    uint8_t mid;
    uint8_t cable_delay[4];
} __packed atk_mo1218_mid_bb_playload_t;

/* ACK Messsage的大小 */
#define ATK_MO1218_BIN_MSG_ACK_LEN (            \
    ATK_MO1218_BIN_MSG_SS_LEN   +               \
    ATK_MO1218_BIN_MSG_PL_LEN   +               \
    sizeof(atk_mo1218_mid_83_playload_t) +      \
    ATK_MO1218_BIN_MSG_CS_LEN   +               \
    ATK_MO1218_BIN_MSG_ES_LEN  )

/* 带Sub-ID的ACK Messsage的大小 */
#define ATK_MO1218_BIN_MSG_ACK_SID_LEN (        \
    ATK_MO1218_BIN_MSG_SS_LEN   +               \
    ATK_MO1218_BIN_MSG_PL_LEN   +               \
    sizeof(atk_mo1218_mid_83_sid_playload_t) +  \
    ATK_MO1218_BIN_MSG_CS_LEN   +               \
    ATK_MO1218_BIN_MSG_ES_LEN  )

/**
 * @brief       解析ATK-MO1218模块Binary Message
 * @param       msg: Binary Message
 *              mid: Binary Message解析出的Message ID
 *              mb : Binary Message解析出的Message Body
 *              len: mb的长度
 * @retval      ATK_MO1218_EOK   : 解析成功
 *              ATK_MO1218_ERROR : msg有误
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
static uint8_t atk_mo1218_decode_bin_msg(uint8_t *msg, uint8_t *mid, uint8_t *mb, uint16_t *len)
{
    uint16_t ss;                /* Start of Sequence */
    uint16_t pl;                /* Playload Length */
    uint8_t cs;                 /* Checksum */
    uint16_t es;                /* End of Sequence */
    uint16_t playload_index;
    uint8_t _cs = 0;
    uint16_t mb_index;
    
    if (msg == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    if ((mid == NULL) && (*mb == NULL) && (len == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    /* Start of Sequence */
    ss = (uint16_t)(msg[0] << 8) | msg[1];
    if (ss != ATK_MO1218_BIN_MSG_SS)
    {
        return ATK_MO1218_ERROR;
    }
    
    /* Playload Length & End of Sequence */
    pl = (uint16_t)(msg[ATK_MO1218_BIN_MSG_SS_LEN + 0] << 8) | msg[ATK_MO1218_BIN_MSG_SS_LEN + 1];
    es = (uint16_t)(msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + pl + ATK_MO1218_BIN_MSG_CS_LEN + 0] << 8) | 
                    msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + pl + ATK_MO1218_BIN_MSG_CS_LEN + 1];
    if (es != ATK_MO1218_BIN_MSG_ES)
    {
        return ATK_MO1218_ERROR;
    }
    
    /* Checksum */
    cs = msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + pl + 0];
    for (playload_index=0; playload_index<pl; playload_index++)
    {
        _cs ^= msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + playload_index];
    }
    if (cs != _cs)
    {
        return ATK_MO1218_ERROR;
    }
    
    /* Playload(Message ID) */
    if (mid != NULL)
    {
        *mid = msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + 0];
    }
    
    /* Playload(Message Body) */
    if (mb != NULL)
    {
        for (mb_index=0; mb_index<(pl-sizeof(*mid)); mb_index++)
        {
            mb[mb_index] = msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + sizeof(*mid) + mb_index];
        }
    }
    
    /* Message Body Length */
    if (len != NULL)
    {
        *len = pl - sizeof(*mid);
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       往ATK-MO1218发送Binary Message
 * @param       playload: Binary Message的Playload
 *              pl      : Binary Message的Playload Length（playload的长度）
 *              timeout : 等待响应超时时间，单位：100毫秒
 * @retval      ATK_MO1218_EOK     : Binary Message发送成功，并得到ACK响应
 *              ATK_MO1218_ERROR   : 得到NACK或其他响应
 *              ATK_MO1218_ETIMEOUT: 等待响应超时
 */
uint8_t atk_mo1218_send_bin_msg(uint8_t *playload, uint16_t pl, uint16_t timeout)
{
    static uint8_t msg[ATK_MO1218_BIN_MSG_BUF_SIZE] = {0};  /* Binaray Message Buffer */
    uint16_t playload_index;
    uint16_t msg_len;
    uint8_t *res;
    uint8_t res_mid;
    
    /* 计算Message的长度 */
    msg_len = ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + pl + ATK_MO1218_BIN_MSG_CS_LEN + ATK_MO1218_BIN_MSG_ES_LEN;
    if (msg_len > ATK_MO1218_BIN_MSG_BUF_SIZE)
    {
        return ATK_MO1218_ERROR;
    }
    
    /* Start of Sequence */
    msg[0] = (uint8_t)(ATK_MO1218_BIN_MSG_SS >> 8) & 0xFF;
    msg[1] = (uint8_t)ATK_MO1218_BIN_MSG_SS & 0xFF;
    /* Playload Length */
    msg[ATK_MO1218_BIN_MSG_SS_LEN + 0] = (uint8_t)(pl >> 8) & 0xFF;
    msg[ATK_MO1218_BIN_MSG_SS_LEN + 1] = (uint8_t)pl & 0xFF;
    /* Playload */
    for (playload_index=0; playload_index<pl; playload_index++)
    {
        msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + playload_index] = playload[playload_index];
    }
    /* Checksum */
    msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + pl + 0] = 0;
    for (playload_index=0; playload_index<pl; playload_index++)
    {
        msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + pl + 0] ^= msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + playload_index];
    }
    /* End of Sequence */
    msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + pl + ATK_MO1218_BIN_MSG_CS_LEN + 0] = (uint8_t)(ATK_MO1218_BIN_MSG_ES >> 8) & 0xFF;
    msg[ATK_MO1218_BIN_MSG_SS_LEN + ATK_MO1218_BIN_MSG_PL_LEN + pl + ATK_MO1218_BIN_MSG_CS_LEN + 1] = (uint8_t)ATK_MO1218_BIN_MSG_ES & 0xFF;
    
    /* ATK-MO1218 UART重新开始接收数据 */
    atk_mo1218_uart_rx_restart();
    
    /* 发送Binary Message */
    atk_mo1218_uart_send(msg, msg_len);
    
    /* 等待响应 */
    if (timeout == 0)
    {
        return ATK_MO1218_EOK;
    }
    else
    {
        while (timeout > 0)
        {
            /* 获取响应数据 */
            res = atk_mo1218_uart_rx_get_frame();
            if (res != NULL)
            {
                /* 解析响应数据 */
                if (atk_mo1218_decode_bin_msg(res, &res_mid, NULL, NULL) == ATK_MO1218_EOK)
                {
                    if (res_mid == ATK_MO1218_MID_83)
                    {
                        /* 得到ACK响应 */
                        return ATK_MO1218_EOK;
                    }
                    else
                    {
                        /* 得到NACK或其他响应 */
                        return ATK_MO1218_ERROR;
                    }
                }
                
                /* 重发Binary Message */
                atk_mo1218_uart_send(msg, msg_len);
                atk_mo1218_uart_rx_restart();
            }
            timeout--;
            delay_ms(100);
        }
        
        return ATK_MO1218_ETIMEOUT;
    }
}

/**
 * @brief       ATK-MO1218模块系统重启
 * @param       restart: ATK_MO1218_RESTART_HOT : 热启动
 *                       ATK_MO1218_RESTART_WARM: 温启动
 *                       ATK_MO1218_RESTART_COLD: 冷启动
 * @retval      ATK_MO1218_EOK  : ATK-MO1218模块系统重启成功
 *              ATK_MO1218_ERROR: ATK-MO1218模块系统重启失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_restart(atk_mo1218_restart_t restart)
{
    uint8_t ret;
    atk_mo1218_mid_01_playload_t playload = {0};
    
    switch (restart)
    {
        case ATK_MO1218_RESTART_HOT:
        {
            playload.start_mode = 1;
            break;
        }
        case ATK_MO1218_RESTART_WARM:
        {
            playload.start_mode = 2;
            break;
        }
        case ATK_MO1218_RESTART_COLD:
        {
            playload.start_mode = 3;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_01;
    playload.utc_year = 0;
    playload.utc_month = 0;
    playload.utc_day = 0;
    playload.utc_hour = 0;
    playload.utc_minute = 0;
    playload.utc_second = 0;
    playload.latitude = 0;
    playload.longitude = 0;
    playload.altitude = 0;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块软件版本
 * @param       version: ATK-MO1218模块软件版本
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块软件版本成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块软件版本失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_sw_version(atk_mo1218_sw_version_t *version)
{
    uint8_t ret;
    atk_mo1218_mid_02_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_80_playload_t playload_res = {0};
    
    if (version == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_02;
    playload.sw_type = 1;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_80)
    {
        return ATK_MO1218_ERROR;
    }
    
    version->kernel.x1 = playload_res.kernel_ver[1];
    version->kernel.y1 = playload_res.kernel_ver[2];
    version->kernel.z1 = playload_res.kernel_ver[3];
    version->odm.x1 = playload_res.odm_ver[1];
    version->odm.y1 = playload_res.odm_ver[2];
    version->odm.z1 = playload_res.odm_ver[3];
    version->revision.yy = playload_res.rev[1];
    version->revision.mm = playload_res.rev[2];
    version->revision.dd = playload_res.rev[3];
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块软件CRC值
 * @param       crc: ATK-MO1218模块软件CRC值
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块软件CRC值成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块软件CRC值失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_sw_crc(uint16_t *crc)
{
    uint8_t ret;
    atk_mo1218_mid_03_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_81_playload_t playload_res = {0};
    
    if (crc == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_03;
    playload.sw_type = 1;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_81)
    {
        return ATK_MO1218_ERROR;
    }
    
    *crc = (uint16_t)(playload_res.crc[0] << 8) | playload_res.crc[1];
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       ATK-MO1218模块恢复出厂设置
 * @param       无
 * @retval      ATK_MO1218_EOK   : ATK-MO1218模块恢复出厂设置成功
 *              ATK_MO1218_ERROR : ATK-MO1218模块恢复出厂设置失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_factory_reset(atk_mo1218_factory_reset_type_t type)
{
    uint8_t ret;
    atk_mo1218_mid_04_playload_t playload = {0};
    
    switch (type)
    {
        case ATK_MO1218_FACTORY_RESET_NOREBOOT:
        {
            playload.type = 0;
            break;
        }
        case ATK_MO1218_FACTORY_RESET_REBOOT:
        {
            playload.type = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_04;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块串口
 * @param       baudrate : ATK_MO1218_SERIAL_BAUDRATE_4800  : 4800bps
 *                         ATK_MO1218_SERIAL_BAUDRATE_9600  : 9600bps
 *                         ATK_MO1218_SERIAL_BAUDRATE_19200 : 19200bps
 *                         ATK_MO1218_SERIAL_BAUDRATE_38400 : 38400bps
 *                         ATK_MO1218_SERIAL_BAUDRATE_57600 : 57600bps
 *                         ATK_MO1218_SERIAL_BAUDRATE_115200: 115200bps
 *                         ATK_MO1218_SERIAL_BAUDRATE_230400: 230400bps
 *                         ATK_MO1218_SERIAL_BAUDRATE_460800: 460800bps
 *                         ATK_MO1218_SERIAL_BAUDRATE_921600: 921600bps
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 *                         ATK_MO1218_SAVE_TEMP      : 暂时的
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块串口成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块串口失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_serial(atk_mo1218_serial_baudrate_t baudrate, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_05_playload_t playload = {0};
    
    switch (baudrate)
    {
        case ATK_MO1218_SERIAL_BAUDRATE_4800:
        {
            playload.baudrate = 0;
            break;
        }
        case ATK_MO1218_SERIAL_BAUDRATE_9600:
        {
            playload.baudrate = 1;
            break;
        }
        case ATK_MO1218_SERIAL_BAUDRATE_19200:
        {
            playload.baudrate = 2;
            break;
        }
        case ATK_MO1218_SERIAL_BAUDRATE_38400:
        {
            playload.baudrate = 3;
            break;
        }
        case ATK_MO1218_SERIAL_BAUDRATE_57600:
        {
            playload.baudrate = 4;
            break;
        }
        case ATK_MO1218_SERIAL_BAUDRATE_115200:
        {
            playload.baudrate = 5;
            break;
        }
        case ATK_MO1218_SERIAL_BAUDRATE_230400:
        {
            playload.baudrate = 6;
            break;
        }
        case ATK_MO1218_SERIAL_BAUDRATE_460800:
        {
            playload.baudrate = 7;
            break;
        }
        case ATK_MO1218_SERIAL_BAUDRATE_921600:
        {
            playload.baudrate = 8;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        case ATK_MO1218_SAVE_TEMP:
        {
            playload.attributes = 2;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_05;
    playload.port = 0;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块NMEA输出信息间隔
 * @param       gga      : GGA信息输出间隔，单位：秒
 *              gsa      : GSA信息输出间隔，单位：秒
 *              gsv      : GSV信息输出间隔，单位：秒
 *              gll      : GLL信息输出间隔，单位：秒
 *              rmc      : RMC信息输出间隔，单位：秒
 *              vtg      : VTG信息输出间隔，单位：秒
 *              zda      : ZDA信息输出间隔，单位：秒
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块NMEA输出信息间隔成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块NMEA输出信息间隔失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_nmea_msg(uint8_t gga, uint8_t gsa, uint8_t gsv, uint8_t gll, uint8_t rmc, uint8_t vtg, uint8_t zda, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_08_playload_t playload = {0};
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_08;
    playload.gga_interval = gga;
    playload.gsa_interval = gsa;
    playload.gsv_interval = gsv;
    playload.gll_interval = gll;
    playload.rmc_interval = rmc;
    playload.vtg_interval = vtg;
    playload.zda_interval = zda;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块输出消息类型
 * @param       output_type: ATK_MO1218_NOOUTPUT     : 无输出
 *                           ATK_MO1218_OUTPUT_NMEA  : 输出NMEA信息
 *                           ATK_MO1218_OUTPUT_BINARY: 输出Binary Message
 *              save_type  : ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                           ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块输出消息类型成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块输出消息类型失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_output_type(atk_mo1218_output_type_t output_type, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_09_playload_t playload = {0};
    
    switch (output_type)
    {
        case ATK_MO1218_NOOUTPUT:
        {
            playload.type = 0;
            break;
        }
        case ATK_MO1218_OUTPUT_NMEA:
        {
            playload.type = 1;
            break;
        }
        case ATK_MO1218_OUTPUT_BINARY:
        {
            playload.type = 2;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_09;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块电源模式
 * @param       mode     : ATK_MO1218_POWER_MODE_NORMAL: 正常模式
 *                         ATK_MO1218_POWER_MODE_SAVE  : 省电模式
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 *                         ATK_MO1218_SAVE_TEMP      : 暂时的
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块电源模式成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块电源模式失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_power_mode(atk_mo1218_power_mode_t mode, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_0c_playload_t playload = {0};
    
    switch (mode)
    {
        case ATK_MO1218_POWER_MODE_NORMAL:
        {
            playload.mode = 0;
            break;
        }
        case ATK_MO1218_POWER_MODE_SAVE:
        {
            playload.mode = 1;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        case ATK_MO1218_SAVE_TEMP:
        {
            playload.attributes = 2;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_0C;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块位置更新频率
 * @param       rate     : ATK_MO1218_POSITION_RATE_1HZ : 1Hz
 *                         ATK_MO1218_POSITION_RATE_2HZ : 2Hz
 *                         ATK_MO1218_POSITION_RATE_4HZ : 4Hz，串口波特率需高于38400bps
 *                         ATK_MO1218_POSITION_RATE_5HZ : 5Hz，串口波特率需高于38400bps
 *                         ATK_MO1218_POSITION_RATE_8HZ : 8Hz，串口波特率需高于38400bps
 *                         ATK_MO1218_POSITION_RATE_10HZ: 10Hz，串口波特率需高于38400bps
 *                         ATK_MO1218_POSITION_RATE_20HZ: 20Hz，串口波特率需高于115200bps
 *                         ATK_MO1218_POSITION_RATE_25HZ: 25Hz，串口波特率需高于115200bps
 *                         ATK_MO1218_POSITION_RATE_40HZ: 40Hz，串口波特率需高于921600bps
 *                         ATK_MO1218_POSITION_RATE_50HZ: 50Hz，串口波特率需高于921600bps
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块位置更新频率成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块位置更新频率失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_position_rate(atk_mo1218_position_rate_t rate, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_0e_playload_t playload = {0};
    
    switch (rate)
    {
        case ATK_MO1218_POSITION_RATE_1HZ:
        {
            playload.rate = 1;
            break;
        }
        case ATK_MO1218_POSITION_RATE_2HZ:
        {
            playload.rate = 2;
            break;
        }
        case ATK_MO1218_POSITION_RATE_4HZ:
        {
            playload.rate = 4;
            break;
        }
        case ATK_MO1218_POSITION_RATE_5HZ:
        {
            playload.rate = 5;
            break;
        }
        case ATK_MO1218_POSITION_RATE_8HZ:
        {
            playload.rate = 8;
            break;
        }
        case ATK_MO1218_POSITION_RATE_10HZ:
        {
            playload.rate = 10;
            break;
        }
        case ATK_MO1218_POSITION_RATE_20HZ:
        {
            playload.rate = 20;
            break;
        }
        case ATK_MO1218_POSITION_RATE_25HZ:
        {
            playload.rate = 25;
            break;
        }
        case ATK_MO1218_POSITION_RATE_40HZ:
        {
            playload.rate = 40;
            break;
        }
        case ATK_MO1218_POSITION_RATE_50HZ:
        {
            playload.rate = 50;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_0E;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块位置更新频率
 * @param       rate: ATK_MO1218_POSITION_RATE_1HZ : 1Hz
 *                    ATK_MO1218_POSITION_RATE_2HZ : 2Hz
 *                    ATK_MO1218_POSITION_RATE_4HZ : 4Hz，串口波特率需高于38400bps
 *                    ATK_MO1218_POSITION_RATE_5HZ : 5Hz，串口波特率需高于38400bps
 *                    ATK_MO1218_POSITION_RATE_8HZ : 8Hz，串口波特率需高于38400bps
 *                    ATK_MO1218_POSITION_RATE_10HZ: 10Hz，串口波特率需高于38400bps
 *                    ATK_MO1218_POSITION_RATE_20HZ: 20Hz，串口波特率需高于115200bps
 *                    ATK_MO1218_POSITION_RATE_25HZ: 25Hz，串口波特率需高于115200bps
 *                    ATK_MO1218_POSITION_RATE_40HZ: 40Hz，串口波特率需高于921600bps
 *                    ATK_MO1218_POSITION_RATE_50HZ: 50Hz，串口波特率需高于921600bps
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块位置更新频率成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块位置更新频率失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_position_rate(atk_mo1218_position_rate_t *rate)
{
    uint8_t ret;
    atk_mo1218_mid_10_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_86_playload_t playload_res = {0};
    
    if (rate == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_10;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_86)
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.rate)
    {
        case 1:
        {
            *rate = ATK_MO1218_POSITION_RATE_1HZ;
            break;
        }
        case 2:
        {
            *rate = ATK_MO1218_POSITION_RATE_2HZ;
            break;
        }
        case 4:
        {
            *rate = ATK_MO1218_POSITION_RATE_4HZ;
            break;
        }
        case 5:
        {
            *rate = ATK_MO1218_POSITION_RATE_5HZ;
            break;
        }
        case 8:
        {
            *rate = ATK_MO1218_POSITION_RATE_8HZ;
            break;
        }
        case 10:
        {
            *rate = ATK_MO1218_POSITION_RATE_10HZ;
            break;
        }
        case 20:
        {
            *rate = ATK_MO1218_POSITION_RATE_20HZ;
            break;
        }
        case 25:
        {
            *rate = ATK_MO1218_POSITION_RATE_25HZ;
            break;
        }
        case 40:
        {
            *rate = ATK_MO1218_POSITION_RATE_40HZ;
            break;
        }
        case 50:
        {
            *rate = ATK_MO1218_POSITION_RATE_50HZ;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块导航数据消息间隔
 * @param       interval : 导航数据消息间隔，单位：秒
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块导航数据消息间隔成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块导航数据消息间隔失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_navigation_interval(uint8_t interval, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_11_playload_t playload = {0};
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_11;
    playload.interval = interval;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块电源模式
 * @param       mode: ATK_MO1218_POWER_MODE_NORMAL: 正常模式
 *                    ATK_MO1218_POWER_MODE_SAVE  : 省电模式
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块电源模式成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块电源模式失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_power_mode(atk_mo1218_power_mode_t *mode)
{
    uint8_t ret;
    atk_mo1218_mid_15_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_b9_playload_t playload_res = {0};
    
    if (mode == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_15;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_B9)
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.mode)
    {
        case 0:
        {
            *mode = ATK_MO1218_POWER_MODE_NORMAL;
            break;
        }
        case 1:
        {
            *mode = ATK_MO1218_POWER_MODE_SAVE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块DOP掩码
 * @param       mode     : ATK_MO1218_DOP_MODE_DISABLE: 关闭
 *                         ATK_MO1218_DOP_MODE_AUTO   : 自动
 *                         ATK_MO1218_DOP_MODE_PDOP   : 仅PDOP
 *                         ATK_MO1218_DOP_MODE_HDOP   : 仅HDOP
 *                         ATK_MO1218_DOP_MODE_GDOP   : 仅GDOP
 *              pdop_val : PDOP值，范围：5~300，单位：0.1
 *              hdop_val : HDOP值，范围：5~300，单位：0.1
 *              gdop_val : GDOP值，范围：5~300，单位：0.1
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块DOP掩码成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块DOP掩码失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_dop_mask(atk_mo1218_dop_mode_t mode, uint16_t pdop_val, uint16_t hdop_val, uint16_t gdop_val, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_2a_playload_t playload = {0};
    
    if ((pdop_val < 5) || (pdop_val > 300))
    {
        return ATK_MO1218_EINVAL;
    }
    
    if ((hdop_val < 5) || (hdop_val > 300))
    {
        return ATK_MO1218_EINVAL;
    }
    
    if ((gdop_val < 5) || (gdop_val > 300))
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (mode)
    {
        case ATK_MO1218_DOP_MODE_DISABLE:
        {
            playload.mode = 0;
            break;
        }
        case ATK_MO1218_DOP_MODE_AUTO:
        {
            playload.mode = 1;
            break;
        }
        case ATK_MO1218_DOP_MODE_PDOP:
        {
            playload.mode = 2;
            break;
        }
        case ATK_MO1218_DOP_MODE_HDOP:
        {
            playload.mode = 3;
            break;
        }
        case ATK_MO1218_DOP_MODE_GDOP:
        {
            playload.mode = 4;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_2A;
    playload.pdop_val[0] = (uint8_t)(pdop_val >> 8) & 0xFF;
    playload.pdop_val[1] = (uint8_t)pdop_val & 0xFF;
    playload.hdop_val[0] = (uint8_t)(hdop_val >> 8) & 0xFF;
    playload.hdop_val[1] = (uint8_t)hdop_val & 0xFF;
    playload.gdop_val[0] = (uint8_t)(gdop_val >> 8) & 0xFF;
    playload.gdop_val[1] = (uint8_t)gdop_val & 0xFF;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块Elevation和CNR掩码
 * @param       mode          : ATK_MO1218_ELEVATION_CNR_MODE_DISABLE  : 关闭
 *                              ATK_MO1218_ELEVATION_CNR_MODE_BOTH     : Elevation和CNR
 *                              ATK_MO1218_ELEVATION_CNR_MODE_ELEVATION: 仅Elevation
 *                              ATK_MO1218_ELEVATION_CNR_MODE_CNR      : 仅CNR
 *              elevation_mask: Elevation掩码，范围：3~85
 *              cnr_mask      : CNR掩码，范围：0~40
 *              save_type     : ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                              ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块Elevation和CNR掩码成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块Elevation和CNR掩码失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_evelation_cnr_mask(atk_mo1218_elevation_cnr_mode_t mode, uint8_t elevation_mask, uint8_t cnr_mask, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_2b_playload_t playload = {0};
    
    if ((elevation_mask < 3) || (elevation_mask > 85))
    {
        return ATK_MO1218_EINVAL;
    }
    
    if ((cnr_mask > 40))
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (mode)
    {
        case ATK_MO1218_ELEVATION_CNR_MODE_DISABLE:
        {
            playload.mode = 0;
            break;
        }
        case ATK_MO1218_ELEVATION_CNR_MODE_BOTH:
        {
            playload.mode = 1;
            break;
        }
        case ATK_MO1218_ELEVATION_CNR_MODE_ELEVATION:
        {
            playload.mode = 2;
            break;
        }
        case ATK_MO1218_ELEVATION_CNR_MODE_CNR:
        {
            playload.mode = 3;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_2B;
    playload.elevation_mask = elevation_mask;
    playload.cnr_mask = cnr_mask;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块Datum
 * @param       datum_index: ATK-MO1218模块Datum索引，查看《Binary Message of SkyTraq Venus 8 GNSS Receiver.pdf》的附录A和附录B
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块Datum成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块Datum失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_datum(uint16_t *datum_index)
{
    uint8_t ret;
    atk_mo1218_mid_2d_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_ae_playload_t playload_res = {0};
    
    if (datum_index == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_2D;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_AE)
    {
        return ATK_MO1218_ERROR;
    }
    
    *datum_index = (uint16_t)(playload_res.datum_index[0] << 8) | playload_res.datum_index[1];
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块DOP掩码
 * @param       mode    : ATK_MO1218_DOP_MODE_DISABLE: 关闭
 *                        ATK_MO1218_DOP_MODE_AUTO   : 自动
 *                        ATK_MO1218_DOP_MODE_PDOP   : 仅PDOP
 *                        ATK_MO1218_DOP_MODE_HDOP   : 仅HDOP
 *                        ATK_MO1218_DOP_MODE_GDOP   : 仅GDOP
 *              pdop_val: PDOP值
 *              hdop_val: HDOP值
 *              gdop_val: GDOP值
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块DOP掩码成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块DOP掩码失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_dop_mask(atk_mo1218_dop_mode_t *mode, uint16_t *pdop_val, uint16_t *hdop_val, uint16_t *gdop_val)
{
    uint8_t ret;
    atk_mo1218_mid_2e_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_af_playload_t playload_res = {0};
    
    if ((mode == NULL) && (pdop_val == NULL) && (hdop_val == NULL) && (gdop_val == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_2E;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_AF)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (mode != NULL)
    {
        switch (playload_res.mode)
        {
            case 0:
            {
                *mode = ATK_MO1218_DOP_MODE_DISABLE;
                break;
            }
            case 1:
            {
                *mode = ATK_MO1218_DOP_MODE_AUTO;
                break;
            }
            case 2:
            {
                *mode = ATK_MO1218_DOP_MODE_GDOP;
                break;
            }
            case 3:
            {
                *mode = ATK_MO1218_DOP_MODE_PDOP;
                break;
            }
            case 4:
            {
                *mode = ATK_MO1218_DOP_MODE_HDOP;
                break;
            }
            default:
            {
                return ATK_MO1218_ERROR;
            }
        }
    }
    
    if (pdop_val != NULL)
    {
        *pdop_val = (uint16_t)(playload_res.pdop_val[0] << 8) | playload_res.pdop_val[1];
    }
    
    if (hdop_val != NULL)
    {
        *hdop_val = (uint16_t)(playload_res.hdop_val[0] << 8) | playload_res.hdop_val[1];
    }
    
    if (gdop_val != NULL)
    {
        *gdop_val = (uint16_t)(playload_res.gdop_val[0] << 8) | playload_res.gdop_val[1];
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块Elevation和CNR掩码
 * @param       mode          : ATK_MO1218_ELEVATION_CNR_MODE_DISABLE  : 关闭
 *                              ATK_MO1218_ELEVATION_CNR_MODE_BOTH     : Elevation和CNR
 *                              ATK_MO1218_ELEVATION_CNR_MODE_ELEVATION: 仅Elevation
 *                              ATK_MO1218_ELEVATION_CNR_MODE_CNR      : 仅CNR
 *              elevation_mask: Elevation掩码
 *              cnr_mask      : CNR掩码
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块Elevation和CNR掩码成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块Elevation和CNR掩码失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_evelation_cnr_mask(atk_mo1218_elevation_cnr_mode_t *mode, uint8_t *elevation_mask, uint8_t *cnr_mask)
{
    uint8_t ret;
    atk_mo1218_mid_2f_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_b0_playload_t playload_res = {0};
    
    if ((mode == NULL) && (elevation_mask == NULL) && (cnr_mask == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_2F;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_B0)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (mode != NULL)
    {
        switch (playload_res.mode)
        {
            case 0:
            {
                *mode = ATK_MO1218_ELEVATION_CNR_MODE_DISABLE;
                break;
            }
            case 1:
            {
                *mode = ATK_MO1218_ELEVATION_CNR_MODE_BOTH;
                break;
            }
            case 2:
            {
                *mode = ATK_MO1218_ELEVATION_CNR_MODE_ELEVATION;
                break;
            }
            case 3:
            {
                *mode = ATK_MO1218_ELEVATION_CNR_MODE_CNR;
                break;
            }
            default:
            {
                return ATK_MO1218_ERROR;
            }
        }
    }
    
    if (elevation_mask != NULL)
    {
        *elevation_mask = playload_res.elevation_mask;
    }
    
    if (cnr_mask != NULL)
    {
        *cnr_mask = playload_res.cnr_mask;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块GPS星历数据
 * @param       sv  : 卫星编号，0：全部卫星；1~32：个别卫星
 *              data: GPS星历数据
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块GPS星历数据成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块GPS星历数据失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_gps_ephemeris(uint8_t sv, atk_mo1218_gps_ephemeris_data_t *data)
{
    uint8_t ret;
    atk_mo1218_mid_30_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_b1_playload_t playload_res = {0};
    uint8_t frame_index;
    
    if (sv > 32)
    {
        return ATK_MO1218_EINVAL;
    }
    
    if (data == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_30;
    playload.sv = sv;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_B1)
    {
        return ATK_MO1218_ERROR;
    }
    
    data->sv_id = (uint16_t)(playload_res.sv[0] << 8) | playload_res.sv[1];
    for (frame_index=0; frame_index<sizeof(playload_res.sub_frame0); frame_index++)
    {
        data->subframe0[frame_index] = playload_res.sub_frame0[frame_index];
    }
    for (frame_index=0; frame_index<sizeof(playload_res.sub_frame1); frame_index++)
    {
        data->subframe1[frame_index] = playload_res.sub_frame1[frame_index];
    }
    for (frame_index=0; frame_index<sizeof(playload_res.sub_frame2); frame_index++)
    {
        data->subframe2[frame_index] = playload_res.sub_frame2[frame_index];
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块Position Pinning
 * @param       status   : ATK_MO1218_POSITION_PINNING_DEFAULT: 默认
 *                         ATK_MO1218_POSITION_PINNING_ENABLE : 使能
 *                         ATK_MO1218_POSITION_PINNING_DISABLE: 关闭
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块Position Pinning成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块Position Pinning失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_position_pinning(atk_mo1218_position_pinning_t status, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_39_playload_t playload = {0};
    
    switch (status)
    {
        case ATK_MO1218_POSITION_PINNING_DEFAULT:
        {
            playload.status = 0;
            break;
        }
        case ATK_MO1218_POSITION_PINNING_ENABLE:
        {
            playload.status = 1;
            break;
        }
        case ATK_MO1218_POSITION_PINNING_DISABLE:
        {
            playload.status = 2;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_39;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块Position Pinning状态
 * @param       status: Position Pinning状态
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块Position Pinning状态成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块Position Pinning状态失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_position_pinning_status(atk_mo1218_position_pinning_status_t *status)
{
    uint8_t ret;
    atk_mo1218_mid_3a_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_b4_playload_t playload_res = {0};
    
    if (status == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_3A;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_B4)
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.status)
    {
        case 0:
        {
            status->status = ATK_MO1218_POSITION_PINNING_DEFAULT;
            break;
        }
        case 1:
        {
            status->status = ATK_MO1218_POSITION_PINNING_ENABLE;
            break;
        }
        case 2:
        {
            status->status = ATK_MO1218_POSITION_PINNING_DISABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    status->parameter.pinning_speed = (uint16_t)(playload_res.pinning_speed[0] << 8) | playload_res.pinning_speed[1];
    status->parameter.pinning_cnt = (uint16_t)(playload_res.pinning_cnt[0] << 8) | playload_res.pinning_cnt[1];
    status->parameter.unpinning_speed = (uint16_t)(playload_res.unpinning_speed[0] << 8) | playload_res.unpinning_speed[1];
    status->parameter.unpinning_cnt = (uint16_t)(playload_res.unpinning_cnt[0] << 8) | playload_res.unpinning_cnt[1];
    status->parameter.unpinning_distance = (uint16_t)(playload_res.unpinning_distance[0] << 8) | playload_res.unpinning_distance[1];
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块Position Pinning参数
 * @param       parameter: Position Pinning参数
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块Position Pinning参数成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块Position Pinning参数失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_position_pinning_parameters(atk_mo1218_position_pinning_parameter_t *parameter, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_3b_playload_t playload = {0};
    
    if (parameter == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_3B;
    playload.pinning_cnt[0] = (uint8_t)(parameter->pinning_cnt >> 8) & 0xFF;
    playload.pinning_cnt[1] = (uint8_t)parameter->pinning_cnt & 0xFF;
    playload.pinning_speed[0] = (uint8_t)(parameter->pinning_speed >> 8) & 0xFF;
    playload.pinning_speed[1] = (uint8_t)parameter->pinning_speed & 0xFF;
    playload.unpinning_cnt[0] = (uint8_t)(parameter->unpinning_cnt >> 8) & 0xFF;
    playload.unpinning_cnt[1] = (uint8_t)parameter->unpinning_cnt & 0xFF;
    playload.unpinning_speed[0] = (uint8_t)(parameter->unpinning_speed >> 8) & 0xFF;
    playload.unpinning_speed[1] = (uint8_t)parameter->unpinning_speed & 0xFF;
    playload.unpinning_distance[0] = (uint8_t)(parameter->unpinning_distance >> 8) & 0xFF;
    playload.unpinning_distance[1] = (uint8_t)parameter->unpinning_distance & 0xFF;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       设置ATK-MO1218模块GPS星历数据
 * @param       data: GPS星历数据
 * @retval      ATK_MO1218_EOK   : 设置ATK-MO1218模块GPS星历数据成功
 *              ATK_MO1218_ERROR : 设置ATK-MO1218模块GPS星历数据失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_set_gps_ephemeris(atk_mo1218_gps_ephemeris_data_t *data)
{
    uint8_t ret;
    atk_mo1218_mid_41_playload_t playload = {0};
    uint8_t frame_index;
    
    if (data == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_41;
    playload.sv[0] = (uint8_t)(data->sv_id >> 8) & 0xFF;
    playload.sv[1] = (uint8_t)data->sv_id & 0xFF;
    for (frame_index=0; frame_index<sizeof(playload.sub_frame0); frame_index++)
    {
        playload.sub_frame0[frame_index] = data->subframe0[frame_index];
    }
    for (frame_index=0; frame_index<sizeof(playload.sub_frame1); frame_index++)
    {
        playload.sub_frame1[frame_index] = data->subframe1[frame_index];
    }
    for (frame_index=0; frame_index<sizeof(playload.sub_frame2); frame_index++)
    {
        playload.sub_frame2[frame_index] = data->subframe2[frame_index];
    }
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块1PPS的电缆延时
 * @param       cable_delay: 1PPS的电缆延时，范围：-500000~500000，单位：0.01纳秒
 *              save_type  : ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                           ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块1PPS的电缆延时成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块1PPS的电缆延时失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_1pps_cable_delay(int32_t cable_delay, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_45_playload_t playload = {0};
    
    if ((cable_delay < -500000) || (cable_delay > 500000))
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_45;
    playload.cable_delay[0] = (uint8_t)(cable_delay >> 24) & 0xFF;
    playload.cable_delay[1] = (uint8_t)(cable_delay >> 16) & 0xFF;
    playload.cable_delay[2] = (uint8_t)(cable_delay >> 8) & 0xFF;
    playload.cable_delay[3] = (uint8_t)cable_delay& 0xFF;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块1PPS的电缆延时
 * @param       cable_delay: 1PPS的电缆延时
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块1PPS的电缆延时成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块1PPS的电缆延时失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_1pps_cable_delay(int32_t *cable_delay)
{
    uint8_t ret;
    atk_mo1218_mid_46_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_bb_playload_t playload_res = {0};
    
    if (cable_delay == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_46;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if (playload_res.mid != ATK_MO1218_MID_BB)
    {
        return ATK_MO1218_ERROR;
    }
    
    *cable_delay = (int32_t)(playload_res.cable_delay[0] << 24) | (playload_res.cable_delay[1] << 16) | (playload_res.cable_delay[2] << 8) | playload_res.cable_delay[3];
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块SBAS
 * @param       parameter: SBAS配置参数
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块SBAS成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块SBAS失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_sbas(atk_mo1218_sbas_parameter_t *parameter, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_62_01_playload_t playload = {0};
    
    if (parameter == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (parameter->enable)
    {
        case ATK_MO1218_SBAS_DISABLE:
        {
            playload.enable = 0;
            break;
        }
        case ATK_MO1218_SBAS_ENABLE:
        {
            playload.enable = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (parameter->navigation)
    {
        case ATK_MO1218_SBAS_NAV_DISABLE:
        {
            playload.ranging = 0;
            break;
        }
        case ATK_MO1218_SBAS_NAV_ENABLE:
        {
            playload.ranging = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    if (parameter->ranging_ura_mask <= 15)
    {
        playload.ranging_ura_mask = parameter->ranging_ura_mask;
    }
    else
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (parameter->correction)
    {
        case ATK_MO1218_SBAS_CORRECTION_DISABLE:
        {
            playload.correction = 0;
            break;
        }
        case ATK_MO1218_SBAS_CORRECTION_ENABLE:
        {
            playload.correction = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    if (parameter->num_tracking_channel <= 3)
    {
        playload.num_tracking_channel = parameter->num_tracking_channel;
    }
    else
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (parameter->waas)
    {
        case ATK_MO1218_SBAS_WAAS_DISABLE:
        {
            playload.subsystem_mask &= ~(1 << 0);
            break;
        }
        case ATK_MO1218_SBAS_WAAS_ENABLE:
        {
            playload.subsystem_mask |= (1 << 0);
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (parameter->egnos)
    {
        case ATK_MO1218_SBAS_EGNOS_DISABLE:
        {
            playload.subsystem_mask &= ~(1 << 1);
            break;
        }
        case ATK_MO1218_SBAS_EGNOS_ENABLE:
        {
            playload.subsystem_mask |= (1 << 1);
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (parameter->msas)
    {
        case ATK_MO1218_SBAS_MSAS_DISABLE:
        {
            playload.subsystem_mask &= ~(1 << 2);
            break;
        }
        case ATK_MO1218_SBAS_MSAS_ENABLE:
        {
            playload.subsystem_mask |= (1 << 2);
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_62;
    playload.sid = ATK_MO1218_SID_62_01;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块SBAS配置参数
 * @param       parameter: SBAS配置参数
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块SBAS配置参数成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块SBAS配置参数失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_sbas_status(atk_mo1218_sbas_parameter_t *parameter)
{
    uint8_t ret;
    atk_mo1218_mid_62_02_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_62_80_playload_t playload_res = {0};
    
    if (parameter == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_62;
    playload.sid = ATK_MO1218_SID_62_02;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_62) || (playload_res.sid != ATK_MO1218_SID_62_80))
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.enable)
    {
        case 0:
        {
            parameter->enable = ATK_MO1218_SBAS_DISABLE;
            break;
        }
        case 1:
        {
            parameter->enable = ATK_MO1218_SBAS_ENABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    switch (playload_res.ranging)
    {
        case 0:
        {
            parameter->navigation = ATK_MO1218_SBAS_NAV_DISABLE;
            break;
        }
        case 1:
        {
            parameter->navigation = ATK_MO1218_SBAS_NAV_ENABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    if (playload_res.ranging_ura_mask <= 15)
    {
        parameter->ranging_ura_mask = playload_res.ranging_ura_mask;
    }
    else
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.correction)
    {
        case 0:
        {
            parameter->correction = ATK_MO1218_SBAS_CORRECTION_DISABLE;
            break;
        }
        case 1:
        {
            parameter->correction = ATK_MO1218_SBAS_CORRECTION_ENABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    if (playload_res.num_tracking_channel <= 3)
    {
        parameter->num_tracking_channel = playload_res.num_tracking_channel;
    }
    else
    {
        return ATK_MO1218_ERROR;
    }
    
    switch ((playload_res.subsystem_mask & (1 << 0)) >> 0)
    {
        case 0:
        {
            parameter->waas = ATK_MO1218_SBAS_WAAS_DISABLE;
            break;
        }
        case 1:
        {
            parameter->waas = ATK_MO1218_SBAS_WAAS_ENABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    switch ((playload_res.subsystem_mask & (1 << 1)) >> 1)
    {
        case 0:
        {
            parameter->egnos = ATK_MO1218_SBAS_EGNOS_DISABLE;
            break;
        }
        case 1:
        {
            parameter->egnos = ATK_MO1218_SBAS_EGNOS_ENABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    switch ((playload_res.subsystem_mask & (1 << 2)) >> 2)
    {
        case 0:
        {
            parameter->msas = ATK_MO1218_SBAS_MSAS_DISABLE;
            break;
        }
        case 1:
        {
            parameter->msas = ATK_MO1218_SBAS_MSAS_ENABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块QZSS
 * @param       parameter: QZSS配置参数
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块QZSS成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块QZSS失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_qzss(atk_mo1218_qzss_parameter_t *parameter, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_62_03_playload_t playload = {0};
    
    if (parameter == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (parameter->enable)
    {
        case ATK_MO1218_SBAS_DISABLE:
        {
            playload.enable = 0;
            break;
        }
        case ATK_MO1218_SBAS_ENABLE:
        {
            playload.enable = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    if ((parameter->num_tracking_channel >= 1) && (parameter->num_tracking_channel <= 3))
    {
        playload.num_tracking_channel = parameter->num_tracking_channel;
    }
    else
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_62;
    playload.sid = ATK_MO1218_SID_62_03;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块QZSS配置参数
 * @param       parameter: QZSS配置参数
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块QZSS配置参数成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块QZSS配置参数失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_qzss_status(atk_mo1218_qzss_parameter_t *parameter)
{
    uint8_t ret;
    atk_mo1218_mid_62_04_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_62_81_playload_t playload_res = {0};
    
    if (parameter == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_62;
    playload.sid = ATK_MO1218_SID_62_04;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_62) || (playload_res.sid != ATK_MO1218_SID_62_81))
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.enable)
    {
        case 0:
        {
            parameter->enable = ATK_MO1218_QZSS_DISABLE;
            break;
        }
        case 1:
        {
            parameter->enable = ATK_MO1218_QZSS_ENABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    if (playload_res.num_tracking_channel <= 3)
    {
        parameter->num_tracking_channel = playload_res.num_tracking_channel;
    }
    else
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块SAEE
 * @param       parameter: SAEE配置参数
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块SAEE成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块SAEE失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_saee(atk_mo1218_saee_parameter_t *parameter, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_63_01_playload_t playload = {0};
    
    if (parameter == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (parameter->enable)
    {
        case ATK_MO1218_SAEE_DEFAULT:
        {
            playload.enable = 0;
            break;
        }
        case ATK_MO1218_SAEE_ENABLE:
        {
            playload.enable = 1;
            break;
        }
        case ATK_MO1218_SAEE_DISABLE:
        {
            playload.enable = 2;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_63;
    playload.sid = ATK_MO1218_SID_63_01;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块SAEE配置参数
 * @param       parameter: SAEE配置参数
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块SAEE配置参数成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块SAEE配置参数失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_saee_status(atk_mo1218_saee_parameter_t *parameter)
{
    uint8_t ret;
    atk_mo1218_mid_63_02_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_63_80_playload_t playload_res = {0};
    
    if (parameter == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_63;
    playload.sid = ATK_MO1218_SID_63_02;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_63) || (playload_res.sid != ATK_MO1218_SID_63_80))
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.enable)
    {
        case 0:
        {
            parameter->enable = ATK_MO1218_SAEE_DEFAULT;
            break;
        }
        case 1:
        {
            parameter->enable = ATK_MO1218_SAEE_ENABLE;
            break;
        }
        case 2:
        {
            parameter->enable = ATK_MO1218_SAEE_DISABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块启动状态信息
 * @param       status: 启动状态信息
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块启动状态信息成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块启动状态信息失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_boot_status(atk_mo1218_boot_status_t *status)
{
    uint8_t ret;
    atk_mo1218_mid_64_01_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_64_80_playload_t playload_res = {0};
    
    if (status == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_01;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_64) || (playload_res.sid != ATK_MO1218_SID_64_80))
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.fail_over)
    {
        case 0:
        {
            status->fail_over = ATK_MO1218_BOOT_FROM_FLASH_OK;
            break;
        }
        case 1:
        {
            status->fail_over = ATK_MO1218_BOOT_FROM_ROM;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块扩展NMEA输出信息间隔
 * @param       gga      : GGA信息输出间隔，单位：秒
 *              gsa      : GSA信息输出间隔，单位：秒
 *              gsv      : GSV信息输出间隔，单位：秒
 *              gll      : GLL信息输出间隔，单位：秒
 *              rmc      : RMC信息输出间隔，单位：秒
 *              vtg      : VTG信息输出间隔，单位：秒
 *              zda      : ZDA信息输出间隔，单位：秒
 *              gns      : GNS信息输出间隔，单位：秒
 *              gbs      : GBS信息输出间隔，单位：秒
 *              grs      : GRS信息输出间隔，单位：秒
 *              dtm      : DTM信息输出间隔，单位：秒
 *              gst      : GST信息输出间隔，单位：秒
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块扩展NMEA输出信息间隔成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块扩展NMEA输出信息间隔失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_ext_nmea_msg(uint8_t gga, uint8_t gsa, uint8_t gsv, uint8_t gll, uint8_t rmc, uint8_t vtg, uint8_t zda, uint8_t gns, uint8_t gbs, uint8_t grs, uint8_t dtm, uint8_t gst, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_64_02_playload_t playload = {0};
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_02;
    playload.gga_interval = gga;
    playload.gsa_interval = gsa;
    playload.gsv_interval = gsv;
    playload.gll_interval = gll;
    playload.rmc_interval = rmc;
    playload.vtg_interval = vtg;
    playload.zda_interval = zda;
    playload.gns_interval = gns;
    playload.gbs_interval = gbs;
    playload.grs_interval = grs;
    playload.dtm_interval = dtm;
    playload.gst_interval = gst;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块扩展NMEA输出信息间隔
 * @param       gga: GGA信息输出间隔，单位：秒
 *              gsa: GSA信息输出间隔，单位：秒
 *              gsv: GSV信息输出间隔，单位：秒
 *              gll: GLL信息输出间隔，单位：秒
 *              rmc: RMC信息输出间隔，单位：秒
 *              vtg: VTG信息输出间隔，单位：秒
 *              zda: ZDA信息输出间隔，单位：秒
 *              gns: GNS信息输出间隔，单位：秒
 *              gbs: GBS信息输出间隔，单位：秒
 *              grs: GRS信息输出间隔，单位：秒
 *              dtm: DTM信息输出间隔，单位：秒
 *              gst: GST信息输出间隔，单位：秒
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块扩展NMEA输出信息间隔成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块扩展NMEA输出信息间隔失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_ext_nmea_msg(uint8_t *gga, uint8_t *gsa, uint8_t *gsv, uint8_t *gll, uint8_t *rmc, uint8_t *vtg, uint8_t *zda, uint8_t *gns, uint8_t *gbs, uint8_t *grs, uint8_t *dtm, uint8_t *gst)
{
    uint8_t ret;
    atk_mo1218_mid_64_03_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_64_81_playload_t playload_res = {0};
    
    if ((gga == NULL) && (gsa == NULL) && (gsv == NULL) && (gll == NULL) && (rmc == NULL) && (vtg == NULL) && (zda == NULL) && (gns == NULL) && (gbs == NULL) && (grs == NULL) && (dtm == NULL) && (gst == NULL))
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_03;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_64) || (playload_res.sid != ATK_MO1218_SID_64_81))
    {
        return ATK_MO1218_ERROR;
    }
    
    if (gga != NULL)
    {
        *gga = playload_res.gga_interval;
    }
    
    if (gsa != NULL)
    {
        *gsa = playload_res.gsa_interval;
    }
    
    if (gsv != NULL)
    {
        *gsv = playload_res.gsv_interval;
    }
    
    if (gll != NULL)
    {
        *gll = playload_res.gll_interval;
    }
    
    if (rmc != NULL)
    {
        *rmc = playload_res.rmc_interval;
    }
    
    if (vtg != NULL)
    {
        *vtg = playload_res.vtg_interval;
    }
    
    if (zda != NULL)
    {
        *zda = playload_res.zda_interval;
    }
    
    if (gns != NULL)
    {
        *gns = playload_res.gns_interval;
    }
    
    if (gbs != NULL)
    {
        *gbs = playload_res.gbs_interval;
    }
    
    if (grs != NULL)
    {
        *grs = playload_res.grs_interval;
    }
    
    if (dtm != NULL)
    {
        *dtm = playload_res.dtm_interval;
    }
    
    if (gst != NULL)
    {
        *gst = playload_res.gst_interval;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块干扰检测
 * @param       enable   : 干扰检测使能状态
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块干扰检测成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块干扰检测失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_interference_detection(atk_mo1218_interence_detection_enable_t enable, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_64_06_playload_t playload = {0};
    
    switch (enable)
    {
        case ATK_MO1218_INTERENCE_DETECTION_DISABLE:
        {
            playload.enable = 0;
            break;
        }
        case ATK_MO1218_INTERENCE_DETECTION_ENABLE:
        {
            playload.enable = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_06;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块干扰检测状态
 * @param       status: 干扰检测状态
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块干扰检测状态成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块干扰检测状态失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_interence_detection_status(atk_mo1218_interence_detection_status_t *status)
{
    uint8_t ret;
    atk_mo1218_mid_64_07_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_64_83_playload_t playload_res = {0};
    
    if (status == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_07;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_64) || (playload_res.sid != ATK_MO1218_SID_64_83))
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.enable)
    {
        case 0:
        {
            status->enable = ATK_MO1218_INTERENCE_DETECTION_DISABLE;
            break;
        }
        case 1:
        {
            status->enable = ATK_MO1218_INTERENCE_DETECTION_ENABLE;
            break;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    switch (playload_res.interence_status)
    {
        case 0:
        {
            status->interence_status = ATK_MO1218_INTERENCE_UNKNOWN;
            break;
        }
        case 1:
        {
            status->interence_status = ATK_MO1218_INTERENCE_NO;
            break;
        }
        case 2:
        {
            status->interence_status = ATK_MO1218_INTERENCE_LITE;
        }
        case 3:
        {
            status->interence_status = ATK_MO1218_INTERENCE_CRITICAL;
        }
        default:
        {
            return ATK_MO1218_ERROR;
        }
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块导航模式
 * @param       mode     : 导航模式
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块导航模式成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块导航模式失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_navigation_mode(atk_mo1218_navigation_mode_t mode, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_64_17_playload_t playload = {0};
    
    switch (mode)
    {
        case ATK_MO1218_NAVIGATION_MODE_AUTO:
        {
            playload.mode = 0;
            break;
        }
        case ATK_MO1218_NAVIGATION_MODE_PRDESTRIAN:
        {
            playload.mode = 1;
            break;
        }
        case ATK_MO1218_NAVIGATION_MODE_CAR:
        {
            playload.mode = 2;
            break;
        }
        case ATK_MO1218_NAVIGATION_MODE_MARINE:
        {
            playload.mode = 3;
            break;
        }
        case ATK_MO1218_NAVIGATION_MODE_BALLOON:
        {
            playload.mode = 4;
            break;
        }
        case ATK_MO1218_NAVIGATION_MODE_AIRBORNE:
        {
            playload.mode = 5;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_17;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块导航模式
 * @param       mode: 导航模式
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块导航模式成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块导航模式失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_navigation_mode(atk_mo1218_navigation_mode_t *mode)
{
    uint8_t ret;
    atk_mo1218_mid_64_18_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_64_8b_playload_t playload_res = {0};
    
    if (mode == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_18;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_64) || (playload_res.sid != ATK_MO1218_SID_64_8B))
    {
        return ATK_MO1218_ERROR;
    }
    
    switch (playload_res.mode)
    {
        case 0:
        {
            *mode = ATK_MO1218_NAVIGATION_MODE_AUTO;
            break;
        }
        case 1:
        {
            *mode = ATK_MO1218_NAVIGATION_MODE_PRDESTRIAN;
            break;
        }
        case 2:
        {
            *mode = ATK_MO1218_NAVIGATION_MODE_CAR;
            break;
        }
        case 3:
        {
            *mode = ATK_MO1218_NAVIGATION_MODE_MARINE;
            break;
        }
        case 4:
        {
            *mode = ATK_MO1218_NAVIGATION_MODE_BALLOON;
            break;
        }
        case 5:
        {
            *mode = ATK_MO1218_NAVIGATION_MODE_AIRBORNE;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块用于导航的GNSS
 * @param       gnss     : 用于导航的GNSS
 *              save_type: ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                         ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块用于导航的GNSS成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块用于导航的GNSS失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_gnss_for_navigation(atk_mo1218_gnss_for_navigation_t gnss, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_64_19_playload_t playload = {0};
    
    switch (gnss)
    {
        case ATK_MO1218_GNSS_GPS:
        {
            playload.gnss[1] = (0x01 << 0);
            break;
        }
        case ATK_MO1218_GNSS_BEIDOU:
        {
            playload.gnss[1] = (0x01 << 3);
            break;
        }
        case ATK_MO1218_GNSS_GPS_BEIDOU:
        {
            playload.gnss[1] = (0x01 << 0) | (0x01 << 3);
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_19;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块用于导航的GNSS
 * @param       gnss: 用于导航的GNSS
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块用于导航的GNSS成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块用于导航的GNSS失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_gnss_for_navigation(atk_mo1218_gnss_for_navigation_t *gnss)
{
    uint8_t ret;
    atk_mo1218_mid_64_1a_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_64_8c_playload_t playload_res = {0};
    
    if (gnss == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_64;
    playload.sid = ATK_MO1218_SID_64_1A;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_64) || (playload_res.sid != ATK_MO1218_SID_64_8C))
    {
        return ATK_MO1218_ERROR;
    }
    
    if (((playload_res.gnss[1] & (0x01 << 0)) == (0x01 << 0)) && ((playload_res.gnss[1] & (0x01 << 3)) == (0x01 << 3)))
    {
        *gnss = ATK_MO1218_GNSS_GPS_BEIDOU;
    }
    else if ((playload_res.gnss[1] & (0x01 << 0)) == (0x01 << 0))
    {
        *gnss = ATK_MO1218_GNSS_GPS;
    }
    else if ((playload_res.gnss[1] & (0x01 << 3)) == (0x01 << 3))
    {
        *gnss = ATK_MO1218_GNSS_BEIDOU;
    }
    else
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       配置ATK-MO1218模块1PPS的脉冲宽度
 * @param       pulse_width: 1PPS的脉冲宽度
 *              save_type  : ATK_MO1218_SAVE_SRAM      : 保存到SRAM（配置掉电丢失）
 *                           ATK_MO1218_SAVE_SRAM_FLASH: 保存到SRAM和Flash（配置掉电不丢失）
 * @retval      ATK_MO1218_EOK   : 配置ATK-MO1218模块1PPS的脉冲宽度成功
 *              ATK_MO1218_ERROR : 配置ATK-MO1218模块1PPS的脉冲宽度失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_config_1pps_pulse_width(uint32_t pulse_width, atk_mo1218_save_type_t save_type)
{
    uint8_t ret;
    atk_mo1218_mid_65_01_playload_t playload = {0};
    
    if ((pulse_width < 1) || (pulse_width > 100000))
    {
        return ATK_MO1218_EINVAL;
    }
    
    switch (save_type)
    {
        case ATK_MO1218_SAVE_SRAM:
        {
            playload.attributes = 0;
            break;
        }
        case ATK_MO1218_SAVE_SRAM_FLASH:
        {
            playload.attributes = 1;
            break;
        }
        default:
        {
            return ATK_MO1218_EINVAL;
        }
    }
    
    playload.mid = ATK_MO1218_MID_65;
    playload.sid = ATK_MO1218_SID_65_01;
    playload.pulse_width[0] = (uint8_t)(pulse_width >> 24) & 0xFF;
    playload.pulse_width[1] = (uint8_t)(pulse_width >> 16) & 0xFF;
    playload.pulse_width[2] = (uint8_t)(pulse_width >> 8) & 0xFF;
    playload.pulse_width[3] = (uint8_t)pulse_width & 0xFF;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    return ATK_MO1218_EOK;
}

/**
 * @brief       获取ATK-MO1218模块1PPS的脉冲宽度
 * @param       pulse_width: 1PPS的脉冲宽度
 * @retval      ATK_MO1218_EOK   : 获取ATK-MO1218模块1PPS的脉冲宽度成功
 *              ATK_MO1218_ERROR : 获取ATK-MO1218模块1PPS的脉冲宽度失败
 *              ATK_MO1218_EINVAL: 函数参数错误
 */
uint8_t atk_mo1218_get_1pps_pulse_width(uint32_t *pulse_width)
{
    uint8_t ret;
    atk_mo1218_mid_65_02_playload_t playload = {0};
    uint8_t *buf;
    atk_mo1218_mid_65_80_playload_t playload_res = {0};
    
    if (pulse_width == NULL)
    {
        return ATK_MO1218_EINVAL;
    }
    
    playload.mid = ATK_MO1218_MID_65;
    playload.sid = ATK_MO1218_SID_65_02;
    
    ret = atk_mo1218_send_bin_msg((uint8_t *)&playload, sizeof(playload), ATK_MO1218_BIN_MSG_TIMEOUT);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    buf = atk_mo1218_uart_rx_get_frame();
    ret = atk_mo1218_decode_bin_msg(&buf[ATK_MO1218_BIN_MSG_ACK_SID_LEN], &playload_res.mid, &playload_res.mid + 1, NULL);
    if (ret != ATK_MO1218_EOK)
    {
        return ATK_MO1218_ERROR;
    }
    
    if ((playload_res.mid != ATK_MO1218_MID_65) || (playload_res.sid != ATK_MO1218_SID_65_80))
    {
        return ATK_MO1218_ERROR;
    }
    
    *pulse_width = (uint32_t)(playload_res.pulse_width[0] << 24) | (playload_res.pulse_width[1] << 16) | (playload_res.pulse_width[2] << 8) | playload_res.pulse_width[3];
    
    return ATK_MO1218_EOK;
}

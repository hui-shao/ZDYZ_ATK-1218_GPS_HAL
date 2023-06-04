/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

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
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

#include<string.h>
#include "atk_mo1218.h"
#include "delay.h"

typedef struct
{
  uint8_t ret;
  atk_mo1218_time_t utc;
  atk_mo1218_position_t position;
  int16_t altitude;
  uint16_t speed;
  atk_mo1218_fix_info_t fix_info;
  atk_mo1218_visible_satellite_info_t gps_satellite_info;
  atk_mo1218_visible_satellite_info_t beidou_satellite_info;
  uint8_t satellite_index;
} user_gps_data_t;

user_gps_data_t gps_data;

uint8_t user_gps_init(void)
{
  uint8_t ret;

  /* 初始化ATK-MO1218模块 */
  ret = atk_mo1218_init(38400);  // 注意 UART 初始化已经由 CubeMX 接管，此处设置的波特率无效。
  // if (ret != 0)
  // {
  //   u1_printf("ATK-MO1218 init failed!\r\n");
  //   return 1;
  // }

  /* 配置ATK-MO1218模块 */
  ret = atk_mo1218_factory_reset(ATK_MO1218_FACTORY_RESET_REBOOT);
  ret += atk_mo1218_config_output_type(ATK_MO1218_OUTPUT_NMEA, ATK_MO1218_SAVE_SRAM);
  ret += atk_mo1218_config_nmea_msg(1, 1, 1, 1, 1, 1, 0, ATK_MO1218_SAVE_SRAM);
  ret += atk_mo1218_config_position_rate(ATK_MO1218_POSITION_RATE_1HZ, ATK_MO1218_SAVE_SRAM);
  ret += atk_mo1218_config_gnss_for_navigation(ATK_MO1218_GNSS_GPS_BEIDOU, ATK_MO1218_SAVE_SRAM);
  if (ret != 0)
  {
    u1_printf("ATK-MO1218 configure failed!\r\n");
    return 2;
  }
  return 0;
}

void user_gps_getdata(void)
{
  /* 结构体清零 */
  memset(&gps_data, 0, sizeof(gps_data));

  // TODO: 把这些下面获取到的信息保存进结构体

  uint8_t ret;
  atk_mo1218_time_t utc;
  atk_mo1218_position_t position;
  int16_t altitude;
  uint16_t speed;
  atk_mo1218_fix_info_t fix_info;
  atk_mo1218_visible_satellite_info_t gps_satellite_info = {0};
  atk_mo1218_visible_satellite_info_t beidou_satellite_info = {0};
  uint8_t satellite_index;

  /* 获取并更新ATK-MO1218模块数据 */
  ret = atk_mo1218_update(&utc, &position, &altitude, &speed, &fix_info, NULL, NULL, 5000);
  if (ret == ATK_MO1218_EOK)
  {
    u1_printf("\r\n");
    
    /* UTC */
    u1_printf("UTC Time: %04d-%02d-%02d %02d:%02d:%02d.%03d\r\n", utc.year, utc.month, utc.day, utc.hour, utc.minute, utc.second, utc.millisecond);

    /* 经纬度（放大了100000倍数） */
    u1_printf("Position: %d.%d'%s %d.%d'%s\r\n", position.longitude.degree / 100000, position.longitude.degree % 100000, (position.longitude.indicator == ATK_MO1218_LONGITUDE_EAST) ? "E" : "W", position.latitude.degree / 100000, position.latitude.degree % 100000, (position.latitude.indicator == ATK_MO1218_LATITUDE_NORTH) ? "N" : "S");

    /* 海拔高度（放大了10倍） */
    u1_printf("Altitude: %d.%dm\r\n", altitude / 10, altitude % 10);

    /* 速度（放大了10倍） */
    u1_printf("Speed: %d.%dKm/H\r\n", speed / 10, speed % 10);

    /* 定位质量 */
    u1_printf("Fix quality: %s\r\n", (fix_info.quality == ATK_MO1218_GPS_UNAVAILABLE) ? "Unavailable" : ((fix_info.quality == ATK_MO1218_GPS_VALID_SPS) ? "SPS mode" : "differential GPS mode"));

    /* 用于定位的卫星数量 */
    u1_printf("Satellites Used: %d\r\n", fix_info.satellite_num);

    /* 定位方式 */
    u1_printf("Fix type: %s\r\n", (fix_info.type == ATK_MO1218_FIX_NOT_AVAILABLE) ? "Unavailable" : ((fix_info.type == ATK_MO1218_FIX_2D) ? "2D" : "3D"));

    /* 用于定位的卫星编号 */
    for (satellite_index = 0; satellite_index < fix_info.satellite_num; satellite_index++)
    {
      if (satellite_index == 0)
      {
        u1_printf("Satellite ID:");
      }
      u1_printf(" %d", fix_info.satellite_id[satellite_index]);
      if (satellite_index == fix_info.satellite_num - 1)
      {
        u1_printf("\r\n");
      }
    }

    /* 位置、水平、垂直精度因子（扩大了10倍） */
    u1_printf("PDOP: %d.%d\r\n", fix_info.pdop / 10, fix_info.pdop % 10);
    u1_printf("HDOP: %d.%d\r\n", fix_info.hdop / 10, fix_info.hdop % 10);
    u1_printf("VDOP: %d.%d\r\n", fix_info.vdop / 10, fix_info.vdop % 10);

    /* 可见的GPS、北斗卫星数量 */
    u1_printf("Number of GPS visible satellite: %d\r\n", gps_satellite_info.satellite_num);
    u1_printf("Number of Beidou visible satellite: %d\r\n", beidou_satellite_info.satellite_num);

    u1_printf("\r\n");
  }
  else
  {
    /* ATK-MO1218模块未定位时，
     * 不输出NMEA协议的GSV语句，
     * 导致因获取不到可见GPS、北斗卫星的信息而超时失败，
     * 此时可将函数atk_mo1218_update()的入参gps_satellite_info和beidou_satellite_info
     * 传入NULL，从而获取未定位时的其他数据
     */
    u1_printf("(DBG) atk_mo1218_update Error\r\n");
  }
}

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
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  delay_init(72);
  u1_printf("(DBG) System Started.\r\n");
  u2_start_idle_receive();
  // user_gps_init(); // 其实不需要
  u3_start_idle_receive();

  
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    // user_gps_getdata();
    delay_ms(200);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
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
     ex: u1_printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

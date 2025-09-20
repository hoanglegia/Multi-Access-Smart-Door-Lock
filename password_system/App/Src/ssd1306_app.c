/**
 * @file       ssd1306_app.c
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-18
 * @author     Tran Van Diep
 *             
 * @brief      SSD1306 Application Layer Implementation
 *             Application level APIs for SSD1306 OLED display
 * @note       Provides simplified APIs for application use
 */

/* Includes ----------------------------------------------------------- */
#include "ssd1306_app.h"
#include <stddef.h>

/* Private defines ---------------------------------------------------- */
#define WHITE_COLOR    (1)     /**< White color value */
#define BLACK_COLOR    (0)     /**< Black color value */

/* Private variables -------------------------------------------------- */
static bsp_ssd1306_config_t g_ssd1306_config;  /*!< BSP configuration */

/* Private function prototypes ---------------------------------------- */
static ssd1306_app_status_t ssd1306_app_bsp_status_convert(bsp_ssd1306_status_t bsp_status);

/* Function definitions ----------------------------------------------- */
ssd1306_app_status_t ssd1306_app_init(void)
{
  bsp_ssd1306_status_t bsp_status;
  
  /* For ssd1306_app, we need to initialize with a dummy system config */
  /* This is a legacy app, should use password_system_app instead */
  bsp_system_config_t dummy_config = {0};
  dummy_config.version = 1;
  
  /* Initialize BSP with new API */
  bsp_status = bsp_ssd1306_init(&g_ssd1306_config, &dummy_config);
  if (bsp_status != BSP_SSD1306_OK)
  {
    return ssd1306_app_bsp_status_convert(bsp_status);
  }
  
  /* Turn on display */
  bsp_status = bsp_ssd1306_display_on(&g_ssd1306_config);
  if (bsp_status != BSP_SSD1306_OK)
  {
    return ssd1306_app_bsp_status_convert(bsp_status);
  }
  
  /* Clear screen */
  bsp_ssd1306_clear();
  
  /* Update display */
  bsp_status = bsp_ssd1306_display(&g_ssd1306_config);
  
  return ssd1306_app_bsp_status_convert(bsp_status);
}

ssd1306_app_status_t ssd1306_app_write_string_default(const char *str)
{
  bsp_ssd1306_status_t bsp_status;
  
  if (str == NULL)
  {
    return SSD1306_APP_ERROR;
  }
  
  /* Write string using default position and font */
  bsp_ssd1306_write_string_at(&g_ssd1306_config, 
                              SSD1306_APP_DEFAULT_X, 
                              SSD1306_APP_DEFAULT_Y, 
                              str);
  
  /* Update display to show the text */
  bsp_status = bsp_ssd1306_display(&g_ssd1306_config);
  
  return ssd1306_app_bsp_status_convert(bsp_status);
}

ssd1306_app_status_t ssd1306_app_display_message(const char *str)
{
  bsp_ssd1306_status_t bsp_status;
  
  if (str == NULL)
  {
    return SSD1306_APP_ERROR;
  }
  
  /* Clear screen first */
  bsp_ssd1306_clear();
  
  /* Write string at default position */
  bsp_ssd1306_write_string_at(&g_ssd1306_config, 
                              SSD1306_APP_DEFAULT_X, 
                              SSD1306_APP_DEFAULT_Y, 
                              str);
  
  /* Update display */
  bsp_status = bsp_ssd1306_display(&g_ssd1306_config);
  
  return ssd1306_app_bsp_status_convert(bsp_status);
}

/* Private definitions ----------------------------------------------- */
static ssd1306_app_status_t ssd1306_app_bsp_status_convert(bsp_ssd1306_status_t bsp_status)
{
  switch (bsp_status)
  {
    case BSP_SSD1306_OK:
      return SSD1306_APP_OK;
    case BSP_SSD1306_TIMEOUT:
      return SSD1306_APP_TIMEOUT;
    case BSP_SSD1306_ERROR:
    default:
      return SSD1306_APP_ERROR;
  }
}

/* End of file -------------------------------------------------------- */

/**
 * @file       ssd1306_app.h
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-18
 * @author     Tran Van Diep
 *             
 * @brief      SSD1306 Application Layer
 *             Application level APIs for SSD1306 OLED display
 * @note       Provides simplified APIs for application use
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef SSD1306_APP_H
#define SSD1306_APP_H

/* Includes ----------------------------------------------------------- */
#include "bsp_ssd1306.h"

/* Public defines ----------------------------------------------------- */
#define SSD1306_APP_DEFAULT_X    (10)   /**< Default X coordinate */
#define SSD1306_APP_DEFAULT_Y    (10)   /**< Default Y coordinate */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief SSD1306 application status enumeration
 */
typedef enum 
{
  SSD1306_APP_OK = 0x00,         /**< Operation completed successfully */
  SSD1306_APP_ERROR,             /**< General error */
  SSD1306_APP_TIMEOUT            /**< Timeout error */
} ssd1306_app_status_t;

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Initialize SSD1306 application
 * 
 * @return ssd1306_app_status_t Operation status
 */
ssd1306_app_status_t ssd1306_app_init(void);

/**
 * @brief Write string at default position (10, 10)
 * 
 * @param[in] str String to write (null-terminated)
 * 
 * @return ssd1306_app_status_t Operation status
 */
ssd1306_app_status_t ssd1306_app_write_string_default(const char *str);

/**
 * @brief Clear screen and write string at default position
 * 
 * @param[in] str String to write (null-terminated)
 * 
 * @return ssd1306_app_status_t Operation status
 */
ssd1306_app_status_t ssd1306_app_display_message(const char *str);

#endif /* SSD1306_APP_H */

/* End of file -------------------------------------------------------- */

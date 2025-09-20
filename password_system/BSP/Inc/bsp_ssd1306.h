/**
 * @file       bsp_ssd1306.h
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    2.0.0
 * @date       2025-09-16
 * @author     Tran Van Diep
 *             
 * @brief      BSP for SSD1306 OLED Display - Version Control & Hardware Integration
 *             Board Support Package providing hardware initialization and configuration
 * @note       Supports multiple hardware versions through conditional compilation
 *             Uses version control system for hardware abstraction
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_SSD1306_H
#define __BSP_SSD1306_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "bsp_system.h"

/* Public defines ----------------------------------------------------- */
/**
 * @brief Hardware version control
 * Define BSP_SSD1306_VERSION in main.h or build system to select hardware configuration
 */
#ifndef BSP_SSD1306_VERSION
  #define BSP_SSD1306_VERSION 1
#endif

/* SSD1306 Configuration */
#define BSP_SSD1306_COMM_TIMEOUT  (100U) /*!< Communication timeout in milliseconds */

/* Hardware Configuration Version 1 - STM32F411CEU6 BlackPill */
#if (BSP_SSD1306_VERSION == 1)
  #define BSP_SSD1306_I2C_INSTANCE     I2C1
  #define BSP_SSD1306_I2C_SCL_PIN      GPIO_PIN_8
  #define BSP_SSD1306_I2C_SCL_PORT     GPIOB
  #define BSP_SSD1306_I2C_SDA_PIN      GPIO_PIN_9
  #define BSP_SSD1306_I2C_SDA_PORT     GPIOB
  #define BSP_SSD1306_I2C_ALT_FUNC     GPIO_AF4_I2C1
  #define BSP_SSD1306_I2C_CLOCK_SPEED  100000U /*!< 100kHz Standard Mode */
#endif

/* Hardware Configuration Version 2 - Future Hardware */
#if (BSP_SSD1306_VERSION == 2)
  #define BSP_SSD1306_I2C_INSTANCE     I2C2
  #define BSP_SSD1306_I2C_SCL_PIN      GPIO_PIN_10
  #define BSP_SSD1306_I2C_SCL_PORT     GPIOB
  #define BSP_SSD1306_I2C_SDA_PIN      GPIO_PIN_11
  #define BSP_SSD1306_I2C_SDA_PORT     GPIOB
  #define BSP_SSD1306_I2C_ALT_FUNC     GPIO_AF4_I2C2
  #define BSP_SSD1306_I2C_CLOCK_SPEED  400000U /*!< 400kHz Fast Mode */
#endif

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief BSP SSD1306 return status enumeration
 */
typedef enum 
{
  BSP_SSD1306_OK,         /**< Operation successful */
  BSP_SSD1306_ERROR,      /**< General error */
  BSP_SSD1306_TIMEOUT,    /**< Communication timeout */
  BSP_SSD1306_HW_ERROR    /**< Hardware initialization error */
}
bsp_ssd1306_status_t;

/**
 * @brief BSP SSD1306 configuration structure
 */
typedef struct
{
  I2C_HandleTypeDef *hi2c;      /**< I2C handle pointer */
  uint8_t            version;   /**< Hardware version */
  uint8_t            initialized; /**< Initialization flag */
}
bsp_ssd1306_config_t;

/**
 * @brief BSP SSD1306 handle structure (alias for config)
 */
typedef bsp_ssd1306_config_t bsp_ssd1306_handle_t;

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize BSP SSD1306 (simplified interface)
 *         This function performs complete initialization including system setup,
 *         hardware configuration, and display initialization
 *
 * @param[out] handle        Pointer to BSP handle structure to initialize
 * @param[in]  system_config Pointer to system configuration
 *
 * @return  
 *  - BSP_SSD1306_OK: Complete initialization successful
 *  - BSP_SSD1306_ERROR: Invalid parameters
 *  - BSP_SSD1306_HW_ERROR: Hardware initialization failed
 * 
 * @note   This is the main initialization function that should be called
 *         Internally calls bsp_ssd1306_init_system, bsp_ssd1306_hw_init, and driver init
 */
bsp_ssd1306_status_t bsp_ssd1306_init(bsp_ssd1306_handle_t *handle,
                                      bsp_system_config_t *system_config);

/**
 * @brief  Initialize BSP SSD1306 system
 *         This function performs version detection and system initialization
 *
 * @param[out] config  Pointer to BSP configuration structure to initialize
 *
 * @return  
 *  - BSP_SSD1306_OK: System initialization successful
 *  - BSP_SSD1306_ERROR: Invalid parameters
 *  - BSP_SSD1306_HW_ERROR: Hardware initialization failed
 * 
 * @note   This function should be called first to detect hardware version
 *         and initialize the BSP configuration structure
 */
bsp_ssd1306_status_t bsp_ssd1306_init_system(bsp_ssd1306_config_t *config);

/**
 * @brief  Initialize SSD1306 hardware (GPIO and I2C)
 *         This function configures GPIO pins and I2C peripheral according to hardware version
 *
 * @param[in]  config  Pointer to BSP configuration structure
 *
 * @return  
 *  - BSP_SSD1306_OK: Hardware initialization successful
 *  - BSP_SSD1306_ERROR: Invalid parameters
 *  - BSP_SSD1306_HW_ERROR: Hardware initialization failed
 *
 * @note   This function is automatically called by bsp_ssd1306_init_system()
 *         Can be called separately for hardware re-initialization
 */
bsp_ssd1306_status_t bsp_ssd1306_hw_init(bsp_ssd1306_config_t *config);

/**
 * @brief  Initialize SSD1306 display driver
 *
 * @param[in]  config  Pointer to BSP configuration structure
 *
 * @return  
 *  - BSP_SSD1306_OK: Display initialization successful
 *  - BSP_SSD1306_ERROR: Invalid parameters or initialization failed
 *  - BSP_SSD1306_TIMEOUT: Communication timeout
 */
bsp_ssd1306_status_t bsp_ssd1306_init_display(bsp_ssd1306_config_t *config);

/**
 * @brief  Turn display ON
 *
 * @param[in]  config  Pointer to BSP configuration structure
 *
 * @return  
 *  - BSP_SSD1306_OK: Success
 *  - BSP_SSD1306_ERROR: Communication error
 *  - BSP_SSD1306_TIMEOUT: Communication timeout
 */
bsp_ssd1306_status_t bsp_ssd1306_display_on(bsp_ssd1306_config_t *config);

/**
 * @brief  Turn display OFF
 *
 * @param[in]  config  Pointer to BSP configuration structure
 *
 * @return  
 *  - BSP_SSD1306_OK: Success
 *  - BSP_SSD1306_ERROR: Communication error
 *  - BSP_SSD1306_TIMEOUT: Communication timeout
 */
bsp_ssd1306_status_t bsp_ssd1306_display_off(bsp_ssd1306_config_t *config);

/**
 * @brief  Clear display buffer
 */
void bsp_ssd1306_clear(void);

/**
 * @brief  Update display with buffer content
 *
 * @param[in]  config  Pointer to BSP configuration structure
 *
 * @return  
 *  - BSP_SSD1306_OK: Success
 *  - BSP_SSD1306_ERROR: Update failed
 *  - BSP_SSD1306_TIMEOUT: Communication timeout
 */
bsp_ssd1306_status_t bsp_ssd1306_display(bsp_ssd1306_config_t *config);

/**
 * @brief  Write string at current cursor position using default font
 *
 * @param[in]  config  Pointer to BSP configuration structure
 * @param[in]  str     String to write
 *
 * @return  Number of pixels advanced horizontally
 *
 * @note   Uses default 6x8 font and white color
 *         Cursor position is automatically advanced
 */
uint16_t bsp_ssd1306_write_string_default(bsp_ssd1306_config_t *config, const char *str);

/**
 * @brief  Write string at specific position
 *
 * @param[in]  config  Pointer to BSP configuration structure  
 * @param[in]  x       X coordinate (0 to 127)
 * @param[in]  y       Y coordinate (0 to 63)
 * @param[in]  str     String to write
 *
 * @return  Number of pixels advanced horizontally
 *
 * @note   Uses default 6x8 font and white color
 *         Cursor position is set to (x,y) before writing
 */
uint16_t bsp_ssd1306_write_string_at(bsp_ssd1306_config_t *config, uint16_t x, uint16_t y, const char *str);

/**
 * @brief  Get BSP version information
 *
 * @param[in]  config  Pointer to BSP configuration structure
 *
 * @return  Hardware version number
 */
uint8_t bsp_ssd1306_get_version(bsp_ssd1306_config_t *config);

/**
 * @brief  Clear screen (clear display buffer)
 *
 * @param[in]  handle  Pointer to BSP handle structure
 *
 * @return  
 *  - BSP_SSD1306_OK: Success
 *  - BSP_SSD1306_ERROR: Invalid parameters
 */
bsp_ssd1306_status_t bsp_ssd1306_clear_screen(bsp_ssd1306_handle_t *handle);

/**
 * @brief  Write string at specific position
 *
 * @param[in]  handle  Pointer to BSP handle structure
 * @param[in]  x       X coordinate (0 to 127)
 * @param[in]  y       Y coordinate (0 to 7, line number)
 * @param[in]  str     String to write
 *
 * @return  
 *  - BSP_SSD1306_OK: Success
 *  - BSP_SSD1306_ERROR: Invalid parameters
 */
bsp_ssd1306_status_t bsp_ssd1306_write_string(bsp_ssd1306_handle_t *handle, 
                                               uint16_t x, uint16_t y, const char *str);

/**
 * @brief  Update screen (send buffer to display)
 *
 * @param[in]  handle  Pointer to BSP handle structure
 *
 * @return  
 *  - BSP_SSD1306_OK: Success
 *  - BSP_SSD1306_ERROR: Update failed
 *  - BSP_SSD1306_TIMEOUT: Communication timeout
 */
bsp_ssd1306_status_t bsp_ssd1306_update_screen(bsp_ssd1306_handle_t *handle);

#endif // __BSP_SSD1306_H

/* End of file -------------------------------------------------------- */

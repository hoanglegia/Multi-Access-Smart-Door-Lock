/**
 * @file       ssd1306.h
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-16
 * @author     Tran Van Diep
 *             
 * @brief      SSD1306 OLED Display Device Driver
 *             Low-level driver for SSD1306 128x64 OLED display using I2C interface
 * @note       This driver provides direct hardware control of SSD1306 controller
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SSD1306_H
#define __SSD1306_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "ssd1306_fonts.h"

/* Public defines ----------------------------------------------------- */
#define SSD1306_I2C_ADDR          (0x3C << 1) /*!< SSD1306 I2C address (7-bit shifted) */
#define SSD1306_WIDTH             (128)       /*!< OLED display width in pixels */
#define SSD1306_HEIGHT            (64)        /*!< OLED display height in pixels */
#define SSD1306_BUFFER_SIZE       (SSD1306_WIDTH * SSD1306_HEIGHT / 8) /*!< Display buffer size */

/* SSD1306 Commands */
#define SSD1306_CMD_DISPLAY_OFF         (0xAE) /*!< Display OFF */
#define SSD1306_CMD_DISPLAY_ON          (0xAF) /*!< Display ON */
#define SSD1306_CMD_SET_MEMORY_MODE     (0x20) /*!< Set Memory Addressing Mode */
#define SSD1306_CMD_SET_COLUMN_ADDR     (0x21) /*!< Set Column Address */
#define SSD1306_CMD_SET_PAGE_ADDR       (0x22) /*!< Set Page Address */
#define SSD1306_CMD_SET_START_LINE      (0x40) /*!< Set Display Start Line */
#define SSD1306_CMD_SET_CONTRAST        (0x81) /*!< Set Contrast Control */
#define SSD1306_CMD_CHARGE_PUMP         (0x8D) /*!< Charge Pump Setting */
#define SSD1306_CMD_SET_SEGMENT_REMAP   (0xA1) /*!< Set Segment Re-map */
#define SSD1306_CMD_ENTIRE_DISPLAY_ON   (0xA4) /*!< Entire Display ON */
#define SSD1306_CMD_SET_NORMAL_DISPLAY  (0xA6) /*!< Set Normal Display */
#define SSD1306_CMD_SET_MULTIPLEX_RATIO (0xA8) /*!< Set Multiplex Ratio */
#define SSD1306_CMD_SET_COM_SCAN_DIR    (0xC8) /*!< Set COM Output Scan Direction */
#define SSD1306_CMD_SET_DISPLAY_OFFSET  (0xD3) /*!< Set Display Offset */
#define SSD1306_CMD_SET_COM_PINS        (0xDA) /*!< Set COM Pins Hardware Configuration */
#define SSD1306_CMD_SET_VCOM_DETECT     (0xDB) /*!< Set VCOMH Deselect Level */
#define SSD1306_CMD_SET_DISPLAY_CLOCK   (0xD5) /*!< Set Display Clock Divide Ratio/Oscillator Frequency */
#define SSD1306_CMD_SET_PRECHARGE       (0xD9) /*!< Set Pre-charge Period */

/* Memory addressing modes */
#define SSD1306_MEMORY_MODE_HORIZONTAL  (0x00) /*!< Horizontal addressing mode */
#define SSD1306_MEMORY_MODE_VERTICAL    (0x01) /*!< Vertical addressing mode */
#define SSD1306_MEMORY_MODE_PAGE        (0x02) /*!< Page addressing mode */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief SSD1306 return status enumeration
 */
typedef enum 
{
  SSD1306_OK,     /**< Operation successful */
  SSD1306_ERROR,  /**< General error */
  SSD1306_TIMEOUT /**< Communication timeout */
}
ssd1306_status_t;

/**
 * @brief SSD1306 pixel color enumeration
 */
typedef enum
{
  SSD1306_COLOR_BLACK = 0, /**< Black pixel (pixel off) */
  SSD1306_COLOR_WHITE = 1  /**< White pixel (pixel on) */
}
ssd1306_color_t;

/**
 * @brief SSD1306 device handle structure
 */
typedef struct 
{
  I2C_HandleTypeDef *hi2c;   /**< I2C handle pointer */
  uint16_t current_x;        /**< Current X coordinate */
  uint16_t current_y;        /**< Current Y coordinate */
  uint8_t  inverted;         /**< Display inverted flag */
  uint8_t  initialized;      /**< Initialization flag */
}
ssd1306_handle_t;

/* Public variables --------------------------------------------------- */
extern uint8_t g_ssd1306_buffer[SSD1306_BUFFER_SIZE]; /**< Display frame buffer */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize SSD1306 OLED display
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 *
 * @return  
 *  - SSD1306_OK: Success
 *  - SSD1306_ERROR: Initialization failed
 *  - SSD1306_TIMEOUT: I2C communication timeout
 */
ssd1306_status_t ssd1306_init(ssd1306_handle_t *handle);

/**
 * @brief  Send command to SSD1306
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 * @param[in]  cmd     Command byte to send
 *
 * @return  
 *  - SSD1306_OK: Success
 *  - SSD1306_ERROR: Communication error
 *  - SSD1306_TIMEOUT: I2C communication timeout
 */
ssd1306_status_t ssd1306_write_command(ssd1306_handle_t *handle, uint8_t cmd);

/**
 * @brief  Send data to SSD1306
 *
 * @param[in]  handle   Pointer to SSD1306 handle structure
 * @param[in]  data     Pointer to data buffer
 * @param[in]  length   Number of bytes to send
 *
 * @return  
 *  - SSD1306_OK: Success  
 *  - SSD1306_ERROR: Communication error
 *  - SSD1306_TIMEOUT: I2C communication timeout
 */
ssd1306_status_t ssd1306_write_data(ssd1306_handle_t *handle, uint8_t *data, uint16_t length);

/**
 * @brief  Update display with buffer content
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 *
 * @return  
 *  - SSD1306_OK: Success
 *  - SSD1306_ERROR: Update failed
 *  - SSD1306_TIMEOUT: I2C communication timeout
 */
ssd1306_status_t ssd1306_update_screen(ssd1306_handle_t *handle);

/**
 * @brief  Clear display buffer
 *
 * @param[in]  color  Fill color (SSD1306_COLOR_BLACK or SSD1306_COLOR_WHITE)
 */
void ssd1306_fill(ssd1306_color_t color);

/**
 * @brief  Set pixel in buffer
 *
 * @param[in]  x      X coordinate (0 to SSD1306_WIDTH-1)
 * @param[in]  y      Y coordinate (0 to SSD1306_HEIGHT-1) 
 * @param[in]  color  Pixel color
 *
 * @attention  This function only updates the buffer. Call ssd1306_update_screen() to display changes.
 */
void ssd1306_draw_pixel(uint16_t x, uint16_t y, ssd1306_color_t color);

/**
 * @brief  Get pixel color from buffer
 *
 * @param[in]  x  X coordinate (0 to SSD1306_WIDTH-1)
 * @param[in]  y  Y coordinate (0 to SSD1306_HEIGHT-1)
 *
 * @return  Pixel color at specified coordinates
 */
ssd1306_color_t ssd1306_get_pixel(uint16_t x, uint16_t y);

/**
 * @brief  Set cursor position
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 * @param[in]  x       X coordinate
 * @param[in]  y       Y coordinate
 */
void ssd1306_goto_xy(ssd1306_handle_t *handle, uint16_t x, uint16_t y);

/**
 * @brief  Turn display ON
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 *
 * @return  
 *  - SSD1306_OK: Success
 *  - SSD1306_ERROR: Communication error
 *  - SSD1306_TIMEOUT: I2C communication timeout
 */
ssd1306_status_t ssd1306_display_on(ssd1306_handle_t *handle);

/**
 * @brief  Turn display OFF
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 *
 * @return  
 *  - SSD1306_OK: Success
 *  - SSD1306_ERROR: Communication error
 *  - SSD1306_TIMEOUT: I2C communication timeout
 */
ssd1306_status_t ssd1306_display_off(ssd1306_handle_t *handle);

/**
 * @brief  Write a character at current cursor position
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 * @param[in]  ch      Character to write
 * @param[in]  font    Pointer to font structure
 * @param[in]  color   Character color
 *
 * @return  Number of pixels advanced horizontally
 */
uint8_t ssd1306_write_char(ssd1306_handle_t *handle, char ch, const font_t *font, ssd1306_color_t color);

/**
 * @brief  Write a string starting from current cursor position
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 * @param[in]  str     Null-terminated string to write
 * @param[in]  font    Pointer to font structure
 * @param[in]  color   String color
 *
 * @return  Number of pixels advanced horizontally
 */
uint16_t ssd1306_write_string(ssd1306_handle_t *handle, const char *str, const font_t *font, ssd1306_color_t color);

/**
 * @brief  Demo function showing text display capabilities
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 *
 * @return  
 *  - SSD1306_OK: Success
 *  - SSD1306_ERROR: Demo failed
 */
ssd1306_status_t ssd1306_text_demo(ssd1306_handle_t *handle);

/**
 * @brief  Write string at specific position (convenience function)
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 * @param[in]  x       X coordinate
 * @param[in]  y       Y coordinate
 * @param[in]  str     String to write
 * @param[in]  font    Font to use
 * @param[in]  color   Text color
 *
 * @return  Number of pixels advanced horizontally
 */
uint16_t ssd1306_write_string_at(ssd1306_handle_t *handle, uint16_t x, uint16_t y, 
                                 const char *str, const font_t *font, ssd1306_color_t color);

#endif // __SSD1306_H

/* End of file -------------------------------------------------------- */

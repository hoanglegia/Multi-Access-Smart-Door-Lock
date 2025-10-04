/**
 * @file       driver_st7735.h
 * @copyright  Copyright (c) 2025
 * @license    
 * @version    1.0.0
 * @date       2025-17-9
 * @author     Hoang Le
 * @brief      Driver for ST7735 1.8" TFT LCD Display
 * @note       This driver supports:
 *             - 128x160 pixel resolution
 *             - 16-bit RGB565 color format
 *             - Hardware and software reset
 *             - Text rendering with custom fonts
 *             - Basic graphics primitives
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __DRIVER_ST7735_H
#define __DRIVER_ST7735_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include <stdint.h>
#include "stm32f1xx_hal.h"
#include <stdio.h>
#include <stdbool.h>
#include "asset_st7735_font.h"
#include "math.h"
#include <string.h>

/* Public defines ----------------------------------------------------------- */
/* Basic colors in RGB565 format */
/* Display specifications */
#define ST7735_WIDTH                          (128)     /**< Display width in pixels */
#define ST7735_HEIGHT                         (160)     /**< Display height in pixels */

#define ST7735_BLACK                          (0x0000)  
#define ST7735_WHITE                          (0xFFFF) 
#define ST7735_BLUE                           (0xFE00) 
#define ST7735_GREEN                          (0x07E0) 
#define ST7735_RED                            (0x001F)  
#define ST7735_YELLOW                         (0xFFE0) 
#define ST7735_MAGENTA                        (0xF81F) 
#define ST7735_CYAN                           (0x07FF) 
/* Public enumerate/structure ----------------------------------------------- */
/**
 * @brief  Main ST7735 display configuration structure
 * @note   Contains all GPIO and SPI configurations needed for display operation
 */
typedef struct
{
  GPIO_TypeDef *port;       /**< GPIO port for all control pins (CS, DC, RST) */
  uint16_t pin_cs;          /**< Chip Select pin number (active low) */
  uint16_t pin_a0;          /**< Data/Command control pin (1=data, 0=command) */
  uint16_t pin_reset;       /**< Hardware Reset pin (active low) */
  SPI_HandleTypeDef *hspi;  /**< SPI interface handler */
}
st7735_t;

/**
 * @brief  Display operation status codes
 * @note   Used to track the success/failure of display operations
 */
typedef enum
{
  ST7735_OK          = 0x00U,  /**< Operation completed successfully */
  ST7735_ERROR       = 0x01U,  /**< Operation failed */
  ST7735_BUSY        = 0x02U,  /**< Display is busy processing previous command */
  ST7735_PARAM_ERROR = 0x03U,  /**< Invalid parameter provided */
  ST7735_SPI_ERROR   = 0x04U,  /**< SPI communication error */
  ST7735_NOT_INIT    = 0x05U   /**< Display not initialized */
}
st7735_status_t;

/**
 * @brief  Display handler structure for tracking display state
 * @note   Maintains initialization status and current operation state
 */
typedef struct
{
  bool st7735_is_init;     /**< Set to true after successful initialization */
  st7735_status_t status;  /**< Current operation status */
  st7735_t *st7735;        /**< Pointer to associated display configuration */
}
st7735_handler_t;

/**
 * @brief  Structure for initialization command sequences
 * @note   Used to send command sequences during display initialization
 *         Each command can have multiple configuration bytes (arguments)
 */
typedef struct
{
  uint8_t cmd;             /**< Command byte to send to display */
  uint8_t num_args;        /**< Number of argument bytes for this command */
  const uint8_t *args;     /**< Array of argument bytes to send after command */
} 
st7735_init_cmd_t;

/* Public macros ------------------------------------------------------------ */
/* Public variables --------------------------------------------------------- */
/* Public APIs -------------------------------------------------------------- */

/**
 * @brief         Write a command byte to the ST7735 display
 * 
 * @param[in]     me    Pointer to ST7735 device struct
 * @param[in]     cmd   Command byte to send
 *
 * @return        ST7735_OK if successful, ST7735_ERROR if parameters invalid or transmission failed
 * @note          Sets A0 pin low to indicate command mode
 */
st7735_status_t st7735_write_cmd(st7735_t *me, uint8_t cmd);

/**
 * @brief         Write a data byte to the ST7735 display
 * 
 * @param[in]     me    Pointer to ST7735 device struct
 * @param[in]     data  Data byte to send
 *
 * @return        ST7735_OK if successful, ST7735_ERROR if parameters invalid or transmission failed
 * @note          Sets A0 pin high to indicate data mode
 */
st7735_status_t st7735_write_data(st7735_t *me, uint8_t data);

/**
 * @brief         Send a list of initialization commands to the display
 * 
 * @param[in]     me         Pointer to ST7735 device struct
 * @param[in]     cmd_list   Pointer to command list array
 *
 * @return        ST7735_OK if all commands sent successfully
 *                ST7735_ERROR if parameters invalid or any command transmission failed
 * @note          Processes commands until ST7735_CMD_END marker is encountered
 */
st7735_status_t st7735_send_cmd_list(st7735_t *me, const st7735_init_cmd_t *cmd_list);

/**
 * @brief         Initialize the ST7735 display
 * 
 * @param[in]     me    Pointer to ST7735 device configuration structure
 * 
 * @return        ST7735_OK if initialization successful
 *                ST7735_ERROR if any step fails or parameters invalid
 * 
 * @note          This function performs the following steps:
 *                1. Hardware reset sequence
 *                2. Software reset command
 *                3. Exit sleep mode
 *                4. Configure display parameters:
 *                   - Frame rate control
 *                   - Power settings
 *                   - Display orientation
 *                   - Gamma settings
 *                   - Memory access control
 *                5. Turn on display
 *
 * @attention     Ensure all GPIO pins and SPI interface are properly configured
 *                before calling this function
 */
st7735_status_t st7735_init(st7735_t *me);

/**
 * @brief         Set the active drawing window on the display
 * 
 * @param[in]     me    Pointer to ST7735 device struct
 * @param[in]     x0    Start column (0 to ST7735_WIDTH-1)
 * @param[in]     y0    Start row (0 to ST7735_HEIGHT-1)
 * @param[in]     x1    End column (x0 to ST7735_WIDTH-1)
 * @param[in]     y1    End row (y0 to ST7735_HEIGHT-1)
 *
 * @return        ST7735_OK if window set successfully
 *                ST7735_ERROR if parameters invalid or command transmission failed
 *                ST7735_PARAM_ERROR if coordinates out of bounds
 * 
 * @note          All subsequent pixel writes will be constrained to this window
 * @attention     x1 must be >= x0 and y1 must be >= y0
 */
st7735_status_t st7735_set_pos(st7735_t *me, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/**
 * @brief         Draw a single pixel on the display
 * 
 * @param[in]     me     Pointer to ST7735 device struct
 * @param[in]     x      Column position (0 to ST7735_WIDTH-1)
 * @param[in]     y      Row position (0 to ST7735_HEIGHT-1)
 * @param[in]     color  16-bit RGB565 color value
 *
 * @return        ST7735_OK if pixel drawn successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if coordinates out of bounds
 * 
 * @note          Color format is RGB565 (5 bits red, 6 bits green, 5 bits blue)
 */
st7735_status_t st7735_draw_pixel(st7735_t *me, uint8_t x, uint8_t y, uint16_t color);

/**
 * @brief         Draw a single character on the display
 * 
 * @param[in]     me      Pointer to ST7735 device struct
 * @param[in]     x       Start column position (0 to ST7735_WIDTH-1)
 * @param[in]     y       Start row position (0 to ST7735_HEIGHT-1)
 * @param[in]     ch      Character to draw (ASCII 32-127)
 * @param[in]     font    Font data structure containing bitmap data
 * @param[in]     color   16-bit RGB565 color for character
 * @param[in]     bgcolor 16-bit RGB565 color for background
 *
 * @return        ST7735_OK if character drawn successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if coordinates invalid or character out of range
 * 
 * @note          Only prints ASCII characters from space (32) to tilde (127)
 * @attention     Ensure x+font.width and y+font.height are within display bounds
 */
st7735_status_t st7735_draw_char(st7735_t *me, uint8_t x, uint8_t y, char ch, font_t font, uint16_t color, uint16_t bgcolor);

/**
 * @brief         Draw a text string on the display
 * 
 * @param[in]     me      Pointer to ST7735 device struct
 * @param[in]     x       Start column position (0 to ST7735_WIDTH-1)
 * @param[in]     y       Start row position (0 to ST7735_HEIGHT-1)
 * @param[in]     str     Null-terminated string to draw
 * @param[in]     font    Font data structure containing bitmap data
 * @param[in]     color   16-bit RGB565 color for text
 * @param[in]     bgcolor 16-bit RGB565 color for background
 *
 * @return        ST7735_OK if string drawn successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if parameters invalid
 * 
 * @note         - String automatically wraps to next line if it reaches display width
 *               - Drawing stops if string reaches bottom of display
 *               - Only prints ASCII characters from space (32) to tilde (127)
 * 
 * @attention     str must be null-terminated
 */
st7735_status_t st7735_draw_string(st7735_t *me, uint8_t x, uint8_t y, char *str, font_t font, uint16_t color, uint16_t bgcolor);

/**
 * @brief         Fill the entire display with a single color
 * 
 * @param[in]     me     Pointer to ST7735 device struct
 * @param[in]     color  16-bit RGB565 color to fill screen with
 *
 * @return        ST7735_OK if screen filled successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_BUSY if display is busy
 * 
 * @note          This function writes to all 20,480 pixels (128x160)
 * @attention     This operation may take several milliseconds to complete
 */
st7735_status_t st7735_full_screen(st7735_t *me, uint16_t color);

/**
 * @brief         Draw a rectangle on the display
 * 
 * @param[in]     me     Pointer to ST7735 device struct
 * @param[in]     x0     Start column (0 to ST7735_WIDTH-1)
 * @param[in]     y0     Start row (0 to ST7735_HEIGHT-1)
 * @param[in]     x1     End column (x0 to ST7735_WIDTH-1)
 * @param[in]     y1     End row (y0 to ST7735_HEIGHT-1)
 * @param[in]     color  16-bit RGB565 color to fill rectangle with
 *
 * @return        ST7735_OK if rectangle drawn successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if coordinates invalid
 * 
 * @note          Draws a rectangle from (x0,y0) to (x1,y1) inclusive
 * @attention     x1 must be >= x0 and y1 must be >= y0; coordinates must be within display bounds
 */
st7735_status_t st7735_draw_rectangle(st7735_t *me, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);

/**
 * @brief         Draw a rhombus (diamond shape) on the display
 * 
 * @param[in]     me     Pointer to ST7735 device struct
 * @param[in]     x      Center column position (0 to ST7735_WIDTH-1)
 * @param[in]     y      Center row position (0 to ST7735_HEIGHT-1)
 * @param[in]     d      Distance from center to each vertex (must be even and >0)
 * @param[in]     color  16-bit RGB565 color to fill rhombus with
 *
 * @return        ST7735_OK if rhombus drawn successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if parameters invalid
 * 
 * @note          Draws a rhombus centered at (x,y) with vertices at:
 *                - Top: (x, y - d/2)
 *                - Bottom: (x, y + d/2)
 *                - Left: (x - d/2, y)
 *                - Right: (x + d/2, y)
 * 
 * @attention     d must be an even number greater than 0; ensure rhombus fits within display bounds
 */
st7735_status_t st7735_draw_rhombus(st7735_t *me, uint8_t x, uint8_t y, uint8_t d, uint16_t color);

/**
 * @brief         Draw a circle on the display
 * 
 * @param[in]     me     Pointer to ST7735 device struct
 * @param[in]     x      Center column position (0 to ST7735_WIDTH-1)
 * @param[in]     y      Center row position (0 to ST7735_HEIGHT-1)
 * @param[in]     r      Radius of the circle (must be >0)
 * @param[in]     color  16-bit RGB565 color to fill circle with
 *
 * @return        ST7735_OK if circle drawn successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if parameters invalid
 * 
 * @note          Draws a circle centered at (x,y) with radius r
 * 
 * @attention     Ensure circle fits within display bounds; large radii may cause performance issues
 */
st7735_status_t st7735_draw_circle(st7735_t *me, uint8_t x, uint8_t y, uint8_t r, uint16_t color);

/**
 * @brief         Fill a rectangle area on the display with a single color
 * 
 * @param[in]     me     Pointer to ST7735 device struct
 * @param[in]     x0     Start column (0 to ST7735_WIDTH-1)
 * @param[in]     y0     Start row (0 to ST7735_HEIGHT-1)
 * @param[in]     x1     End column (x0 to ST7735_WIDTH-1)
 * @param[in]     y1     End row (y0 to ST7735_HEIGHT-1)
 * @param[in]     color  16-bit RGB565 color to fill rectangle with
 *
 * @return        ST7735_OK if area filled successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if coordinates invalid
 * 
 * @note          Fills a rectangle from (x0,y0) to (x1,y1) inclusive with the specified color
 * @attention     x1 must be >= x0 and y1 must be >= y0; coordinates must be within display bounds
 */
st7735_status_t st7735_fill_rectangle(st7735_t *me, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);

/**
 * @brief         Fill a rhombus (diamond shape) area on the display with a single color
 * 
 * @param[in]     me     Pointer to ST7735 device struct
 * @param[in]     x      Center column position (0 to ST7735_WIDTH-1)
 * @param[in]     y      Center row position (0 to ST7735_HEIGHT-1)
 * @param[in]     d      Distance from center to each vertex (must be even and >0)
 * @param[in]     color  16-bit RGB565 color to fill rhombus with
 *
 * @return        ST7735_OK if area filled successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if parameters invalid
 * 
 * @note          Fills a rhombus centered at (x,y) with vertices at:
 *                - Top: (x, y - d/2)
 *                - Bottom: (x, y + d/2)
 *                - Left: (x - d/2, y)
 *                - Right: (x + d/2, y)
 * 
 * @attention     d must be an even number greater than 0; ensure rhombus fits within display bounds
 */
st7735_status_t st7735_fill_rhombus(st7735_t *me, uint8_t x, uint8_t y, uint8_t d, uint16_t color);

/**
 * @brief         Draw a text string centered horizontally on the display
 * 
 * @param[in]     me      Pointer to ST7735 device struct
 * @param[in]     y       Start row position (0 to ST7735_HEIGHT-1)
 * @param[in]     str     Null-terminated string to draw
 * @param[in]     font    Font data structure containing bitmap data
 * @param[in]     color   16-bit RGB565 color for text
 * @param[in]     bgcolor 16-bit RGB565 color for background
 *
 * @return        ST7735_OK if string drawn successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if parameters invalid
 * 
 * @note         - String is centered horizontally based on its pixel width
 *               - Drawing stops if string reaches bottom of display
 *               - Only prints ASCII characters from space (32) to tilde (127)
 * 
 * @attention     str must be null-terminated; ensure y+font.height is within display bounds
 */
st7735_status_t st7735_draw_string_center(st7735_t *me, uint8_t y, char *str, font_t font, uint16_t color, uint16_t bgcolor);

/**
 * @brief         Draw a password input mask (rhombuss) on the display
 * 
 * @param[in]     me             Pointer to ST7735 device struct
 * @param[in]     entered_digits Number of digits entered (filled rhombus)
 * @param[in]     total_digits   Total number of digits in the password
 * @param[in]     y              Row position to draw the mask (0 to ST7735_HEIGHT-1)
 * @param[in]     active_color   16-bit RGB565 color for filled rhombus
 * @param[in]     inactive_color 16-bit RGB565 color for empty rhombus
 *
 * @return        ST7735_OK if mask drawn successfully
 *                ST7735_ERROR if device error occurs
 *                ST7735_PARAM_ERROR if parameters invalid
 * 
 * @note          Draws a horizontal row of rhombus centered on the display:
 *                - Filled rhombus for entered digits
 *                - Empty rhombus for remaining digits
 * 
 * @attention     Ensure total_digits > 0 and entered_digits <= total_digits; ensure y is within display bounds
 */
st7735_status_t st7735_draw_password_mask(st7735_t *me, uint8_t entered_digits, uint8_t total_digits, uint8_t y, uint16_t active_color, uint16_t inactive_color);
/* -------------------------------------------------------------------------- */
#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __DRIVER_ST7735_H */

/* End of file -------------------------------------------------------------- */
/**
 * @file       bsp_st7735.h
 * @copyright  Copyright (c) 2025
 * @license    
 * @version    1.0.0
 * @date       2025-20-9
 * @author     Hoang Le
 * @brief      Board support package for TFT LCD Display (ST7735)
 * @note       This BSP provides an abstraction layer for the ST7735 display driver,
 *             handling hardware-specific initializations (GPIO, SPI) and
 *             exposing simplified display control functions.
 * @example    Refer to application layer for usage examples.
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __BSP_ST7735_H
#define __BSP_ST7735_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "driver_st7735.h" 

#define BSP_VERSION_1

#ifdef BSP_VERSION_1
/* Public defines ----------------------------------------------------------- */
/* GPIO Configuration */
#define ST7735_GPIO_PORT                  GPIOB
#define ST7735_GPIO_PIN_CS                GPIO_PIN_0
#define ST7735_GPIO_PIN_A0                GPIO_PIN_1
#define ST7735_GPIO_PIN_RST               GPIO_PIN_10

/* SPI Configuration */
#define ST7735_SPI_GPIO_PORT              GPIOA
#define ST7735_SPI_SCK_PIN                GPIO_PIN_5
#define ST7735_SPI_MISO_PIN               GPIO_PIN_6
#define ST7735_SPI_MOSI_PIN               GPIO_PIN_7

/* HAL Clock Enable Macros */
#define ST7735_HAL_SPI_PORT_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define ST7735_HAL_GPIO_PORT_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ST7735_HAL_SPI_CLK_ENABLE()       __HAL_RCC_SPI1_CLK_ENABLE()

/* SPI Handle */
#define ST7735_HSPI                       hspi1
#define ST7735_HSPI_INSTANCE              SPI1

#endif /* BSP_VERSION_1 */

#ifdef BSP_VERSION_2
/* Public defines ----------------------------------------------------------- */
/* GPIO Configuration */
#define ST7735_GPIO_PORT                  GPIOB
#define ST7735_GPIO_PIN_CS                GPIO_PIN_0
#define ST7735_GPIO_PIN_A0                GPIO_PIN_1
#define ST7735_GPIO_PIN_RST               GPIO_PIN_10

/* SPI Configuration */
#define ST7735_SPI_GPIO_PORT              GPIOA
#define ST7735_SPI_SCK_PIN                GPIO_PIN_5
#define ST7735_SPI_MISO_PIN               GPIO_PIN_6
#define ST7735_SPI_MOSI_PIN               GPIO_PIN_7

/* HAL Clock Enable Macros */
#define ST7735_HAL_SPI_PORT_CLK_ENABLE()  __HAL_RCC_GPIOA_CLK_ENABLE()
#define ST7735_HAL_GPIO_PORT_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()
#define ST7735_HAL_SPI_CLK_ENABLE()       __HAL_RCC_SPI1_CLK_ENABLE()

/* SPI Handle */
#define ST7735_HSPI                       hspi1
#define ST7735_HSPI_INSTANCE              SPI1

#endif /* BSP_VERSION_2 */

/* Public enumerate/structure ----------------------------------------------- */
/* Public macros ------------------------------------------------------------ */

#define BSP_ST7735_CHECK_HANDLER_INIT(handler) \
    do { \
        if (!(handler)->st7735_is_init || (handler)->status != ST7735_OK) { \
            return ST7735_NOT_INIT; \
        } \
        (handler)->status = ST7735_BUSY; \
    } while(0)

#define BSP_ST7735_CHECK_STATUS(handler, function) \
    do { \
           st7735_status_t status = (function); \
           (handler)->status = status; \
           return (status); \
    } while(0)

#define BSP_ST7735_INIT_CHECK(func_call) \
    do { \
        st7735_status_t _status = (func_call); \
        if (_status != ST7735_OK) { \
            return _status; \
        } \
    } while(0)

/* Public variables --------------------------------------------------------- */
/* Public APIs -------------------------------------------------------------- */

/**
 * @brief         Initializes the necessary GPIO pins for ST7735 control (CS, A0, RST).
 *
 * @param[in]     None
 *
 * @return        st7735_status_t ST7735_OK if GPIOs initialized successfully,
 *                                ST7735_ERROR if any error occurs during HAL GPIO initialization.
 *
 * @note          This function configures the GPIOs for the ST7735 control lines.
 *                It should be called before bsp_st7735_spi_init and bsp_st7735_init.
 * @attention     Specific GPIO ports and pins are defined internally within the BSP.
 *                Ensure HAL_RCC_GPIOx_CLK_ENABLE() calls are handled.
 */
st7735_status_t bsp_st7735_gpio_init(void);

/**
 * @brief         Initializes the SPI1 peripheral for communication with the ST7735 display.
 *
 * @param[in]     None
 *
 * @return        st7735_status_t ST7735_OK if SPI1 initialized successfully,
 *                                ST7735_SPI_ERROR if HAL SPI initialization fails.
 *
 * @note          It should be called after bsp_st7735_gpio_init and before bsp_st7735_init.
 * @attention     Specific SPI parameters (baud rate, pins) are defined internally.
 *                Ensure HAL_RCC_SPIx_CLK_ENABLE() calls are handled.
 */
st7735_status_t bsp_st7735_spi_init(void);

/**
 * @brief         Configures and prepares the ST7735 driver instance within the BSP.
 *                This function maps the hardware resources (GPIO, SPI) to the
 *                ST7735 driver structure.
 *                Performs the full initialization sequence for the ST7735 display.
 *
 * @param[in]     None
 *
 * @return        st7735_status_t ST7735_OK if initialization is successful,
 *                                an error code if any step fails (e.g., SPI error, parameter error).
 *
 * @note          This function relies on the underlying st7735 driver functions.
 */
st7735_status_t bsp_st7735_init(void);

/**
 * @brief         Writes a command byte to the ST7735 display.
 *                This is a low-level access function.
 *
 * @param[in]     cmd   The command byte to send.
 *
 * @return        st7735_status_t ST7735_OK if command sent successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_SPI_ERROR if SPI transmission fails.
 *
 * @note          Sets the A0 pin low before sending the command.
 */
st7735_status_t bsp_st7735_write_cmd(uint8_t cmd);

/**
 * @brief         Writes a data byte to the ST7735 display.
 *                This is a low-level access function.
 *
 * @param[in]     data  The data byte to send.
 *
 * @return        st7735_status_t ST7735_OK if data sent successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_SPI_ERROR if SPI transmission fails.
 *
 * @note          Sets the A0 pin high before sending the data.
 */
st7735_status_t bsp_st7735_write_data(uint8_t data);

/**
 * @brief         Sets the active drawing window (address window) on the display.
 *                All subsequent pixel writes will be constrained to this window.
 *
 * @param[in]     x0    Start column position (0 to ST7735_WIDTH-1).
 * @param[in]     y0    Start row position (0 to ST7735_HEIGHT-1).
 * @param[in]     x1    End column position (x0 to ST7735_WIDTH-1).
 * @param[in]     y1    End row position (y0 to ST7735_HEIGHT-1).
 *
 * @return        st7735_status_t ST7735_OK if window set successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if coordinates are out of bounds or invalid.
 *
 * @attention     x1 must be >= x0 and y1 must be >= y0.
 */
st7735_status_t bsp_st7735_set_pos(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1);

/**
 * @brief         Draws a single pixel on the display at the specified coordinates.
 *
 * @param[in]     x      Column position (0 to ST7735_WIDTH-1).
 * @param[in]     y      Row position (0 to ST7735_HEIGHT-1).
 * @param[in]     color  16-bit RGB565 color value (5 bits Red, 6 bits Green, 5 bits Blue).
 *
 * @return        st7735_status_t ST7735_OK if pixel drawn successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if coordinates are out of bounds.
 *
 * @note          This function sets a single-pixel window and then writes the color data.
 */
st7735_status_t bsp_st7735_draw_pixel(uint8_t x, uint8_t y, uint16_t color);

/**
 * @brief         Draws a single character on the display at the specified start position.
 *
 * @param[in]     x       Start column position (0 to ST7735_WIDTH-1).
 * @param[in]     y       Start row position (0 to ST7735_HEIGHT-1).
 * @param[in]     ch      Character to draw (ASCII 32-127).
 * @param[in]     font    Font data structure containing bitmap data (e.g., width, height, data array).
 * @param[in]     color   16-bit RGB565 color for the character's foreground.
 * @param[in]     bgcolor 16-bit RGB565 color for the character's background.
 *
 * @return        st7735_status_t ST7735_OK if character drawn successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if coordinates are invalid,
 *                                or character is out of supported range (ASCII 32-127).
 *
 * @note          Only prints ASCII characters from space (32) to tilde (127).
 * @attention     Ensure that `x + font.width` and `y + font.height` are within display bounds.
 */
st7735_status_t bsp_st7735_draw_char(uint8_t x, uint8_t y, char ch, font_t font, uint16_t color, uint16_t bgcolor);

/**
 * @brief         Draws a null-terminated text string on the display.
 *
 * @param[in]     x       Start column position for the first character (0 to ST7735_WIDTH-1).
 * @param[in]     y       Start row position for the first character (0 to ST7735_HEIGHT-1).
 * @param[in]     str     Pointer to the null-terminated string to draw.
 * @param[in]     font    Font data structure containing bitmap data.
 * @param[in]     color   16-bit RGB565 color for the text foreground.
 * @param[in]     bgcolor 16-bit RGB565 color for the text background.
 *
 * @return        st7735_status_t ST7735_OK if string drawn successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if `str` is NULL or other parameters are invalid.
 *
 * @note          - The string automatically wraps to the next line if it reaches the display width.
 *                - Drawing stops if the string reaches the bottom of the display.
 *                - Only prints ASCII characters from space (32) to tilde (127).
 * @attention     `str` must be null-terminated.
 */
st7735_status_t bsp_st7735_draw_string(uint8_t x, uint8_t y, char *str, font_t font, uint16_t color, uint16_t bgcolor);

/**
 * @brief         Fills the entire display with a single specified color.
 *
 * @param[in]     color  16-bit RGB565 color value to fill the screen with.
 *
 * @return        st7735_status_t ST7735_OK if the screen is filled successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_SPI_ERROR if SPI communication fails.
 *
 * @note          This function sets the address window to the entire display (128x160 pixels)
 *                and then writes the color data for all pixels.
 * @attention     This operation may take several milliseconds to complete due to the
 *                large number of pixels being written.
 */
st7735_status_t bsp_st7735_full_screen(uint16_t color);

/**
 * @brief         Draws the outline of a rectangle on the display.
 * @param[in]     x0     Start column position (0 to ST7735_WIDTH-1).
 * @param[in]     y0     Start row position (0 to ST7735_HEIGHT-1).
 * @param[in]     x1     End column position (x0 to ST7735_WIDTH-1).
 * @param[in]     y1     End row position (y0 to ST7735_HEIGHT-1).
 * @param[in]     color  16-bit RGB565 color value for the rectangle outline.
 * 
 * @return        st7735_status_t ST7735_OK if rectangle drawn successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if coordinates are out of bounds or invalid.
 * 
 * @note          This function draws the four sides of the rectangle by drawing lines between the specified corners.
 * @attention     x1 must be >= x0 and y1 must be >= y0.
 */
st7735_status_t bsp_st7735_draw_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);

/**
 * @brief         Draws the outline of a rhombus (diamond shape) on the display.
 * @param[in]     x      Center column position (0 to ST7735_WIDTH-1).
 * @param[in]     y      Center row position (0 to ST7735_HEIGHT-1).
 * @param[in]     d      Length of the diagonals (must be even and >0).
 * @param[in]     color  16-bit RGB565 color value for the rhombus outline.
 * 
 * @return        st7735_status_t ST7735_OK if rhombus drawn successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if parameters are invalid or out of bounds.
 * 
 * @note          This function calculates the four vertices of the rhombus based on the center and diagonal length,
 *                then draws lines between these points.
 * @attention     d must be an even number greater than 0. Ensure the rhombus fits within display bounds.
 */
st7735_status_t bsp_st7735_draw_rhombus(uint8_t x, uint8_t y, uint8_t d, uint16_t color);

/**
 * @brief         Draws the outline of a circle on the display using the Midpoint Circle Algorithm.
 * @param[in]     x      Center column position (0 to ST7735_WIDTH-1).
 * @param[in]     y      Center row position (0 to ST7735_HEIGHT-1).
 * @param[in]     r      Radius of the circle (must be >0).
 * @param[in]     color  16-bit RGB565 color value for the circle outline.
 * 
 * @return        st7735_status_t ST7735_OK if circle drawn successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if parameters are invalid or out of bounds.
 * 
 * @note          This function calculates points around the circumference of the circle
 *                and draws pixels at those locations.
 * @attention     Ensure the entire circle fits within display bounds.
 */
st7735_status_t bsp_st7735_draw_circle(uint8_t x, uint8_t y, uint8_t r, uint16_t color);

/**
 * @brief         Fills a rectangle area on the display with a specified color.
 * @param[in]     x0     Start column position (0 to ST7735_WIDTH-1).
 * @param[in]     y0     Start row position (0 to ST7735_HEIGHT-1).
 * @param[in]     x1     End column position (x0 to ST7735_WIDTH-1).
 * @param[in]     y1     End row position (y0 to ST7735_HEIGHT-1).
 * @param[in]     color  16-bit RGB565 color value to fill the rectangle.
 * 
 * @return        st7735_status_t ST7735_OK if rectangle filled successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if coordinates are out of bounds or invalid.
 * 
 * @note          This function sets the address window to the specified rectangle
 *                and then writes the color data for all pixels within that area.
 * @attention     x1 must be >= x0 and y1 must be >= y0.
 */
st7735_status_t bsp_st7735_fill_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color);

/**
 * @brief         Fills a rhombus (diamond shape) area on the display with a specified color.
 * @param[in]     x      Center column position (0 to ST7735_WIDTH-1).
 * @param[in]     y      Center row position (0 to ST7735_HEIGHT-1).
 * @param[in]     d      Length of the diagonals (must be even and >0).
 * @param[in]     color  16-bit RGB565 color value to fill the rhombus.
 * 
 * @return        st7735_status_t ST7735_OK if rhombus filled successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if parameters are invalid or out of bounds.
 * 
 * @note          This function calculates the area of the rhombus based on the center and diagonal length,
 *                then fills it by drawing horizontal lines between the calculated edges.
 * @attention     d must be an even number greater than 0. Ensure the rhombus fits within display bounds.
 */
st7735_status_t bsp_st7735_fill_rhombus(uint8_t x, uint8_t y, uint8_t d, uint16_t color);

/**
 * @brief         Draws a text string centered horizontally on the display.
 * 
 * @param[in]     y       Start row position (0 to ST7735_HEIGHT-1).
 * @param[in]     str     Null-terminated string to draw.
 * @param[in]     font    Font data structure containing bitmap data.
 * @param[in]     color   16-bit RGB565 color for text.
 * @param[in]     bgcolor 16-bit RGB565 color for background.
 *
 * @return        st7735_status_t ST7735_OK if string drawn successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if parameters are invalid.
 * 
 * @note          - String is centered horizontally based on its pixel width.
 *                - Drawing stops if string reaches bottom of display.
 *                - Only prints ASCII characters from space (32) to tilde (127).
 * 
 * @attention     str must be null-terminated; ensure y+font.height is within display bounds.
 */
st7735_status_t bsp_st7735_draw_string_center(uint8_t y, char *str, font_t font, uint16_t color, uint16_t bgcolor);

/**
 * @brief         Draws a password input mask centered on the display.
 * 
 * @param[in]     entered_digits  Number of digits entered (filled).
 * @param[in]     total_digits    Total number of digits in the password.
 * @param[in]     y               Row position to draw the mask (0 to ST7735_HEIGHT-1).
 * @param[in]     active_color    16-bit RGB565 color for entered digits.
 * @param[in]     inactive_color  16-bit RGB565 color for remaining digits.
 *
 * @return        st7735_status_t ST7735_OK if mask drawn successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_PARAM_ERROR if parameters are invalid.
 * 
 * @note          - Draws filled circles for entered digits and empty circles for remaining digits.
 *                - The mask is centered horizontally based on total_digits.
 * 
 * @attention     Ensure y+circle_diameter is within display bounds; total_digits should be >0.
 */
st7735_status_t bsp_st7735_draw_password_mask(uint8_t entered_digits, uint8_t total_digits, uint8_t y, uint16_t active_color, uint16_t inactive_color);

/**
 * @brief         Displays a predefined layout on the ST7735 screen.
 * 
 * @return        st7735_status_t ST7735_OK if display updated successfully,
 *                                ST7735_NOT_INIT if BSP is not initialized,
 *                                ST7735_BUSY if display is busy,
 *                                ST7735_SPI_ERROR if SPI communication fails.
 * 
 * @note          This function fills the screen with a black background, then draws centered title text,
 *                a password prompt, a password input mask, and a list of access methods with their statuses.
 * @attention     Ensure the display is initialized before calling this function.
 */
st7735_status_t bsp_st7735_display();

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __BSP_ST7735_H */

/* End of file -------------------------------------------------------------- */
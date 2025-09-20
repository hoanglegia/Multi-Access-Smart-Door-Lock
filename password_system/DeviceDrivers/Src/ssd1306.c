/**
 * @file       ssd1306.c
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-16
 * @author     Tran Van Diep
 *             
 * @brief      SSD1306 OLED Display Device Driver Implementation
 *             Low-level driver implementation for SSD1306 128x64 OLED display using I2C interface
 * @note       This driver provides direct hardware control of SSD1306 controller
 */

/* Includes ----------------------------------------------------------- */
#include "ssd1306.h"
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/* Private defines ---------------------------------------------------- */
#define SSD1306_TIMEOUT_MS        (100)  /*!< I2C communication timeout in milliseconds */
#define SSD1306_CMD_PREFIX        (0x00) /*!< Command prefix for I2C communication */
#define SSD1306_DATA_PREFIX       (0x40) /*!< Data prefix for I2C communication */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */
/**
 * @brief  Simple delay implementation
 */
static void ssd1306_delay_ms(uint32_t ms)
{
  volatile uint32_t count = ms * 8000; // Approximate delay
  while(count > 0)
  {
    count--;
  }
}

/* Public variables --------------------------------------------------- */
uint8_t g_ssd1306_buffer[SSD1306_BUFFER_SIZE];

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief  Send initialization sequence to SSD1306
 *
 * @param[in]  handle  Pointer to SSD1306 handle structure
 *
 * @return  
 *  - SSD1306_OK: Success
 *  - SSD1306_ERROR: Initialization failed
 *  - SSD1306_TIMEOUT: I2C communication timeout
 */
static ssd1306_status_t ssd1306_send_init_sequence(ssd1306_handle_t *handle);

/* Function definitions ----------------------------------------------- */
ssd1306_status_t ssd1306_init(ssd1306_handle_t *handle)
{
  ssd1306_status_t status;
  
  if (handle == NULL || handle->hi2c == NULL)
  {
    return SSD1306_ERROR;
  }
  
  /* Initialize handle */
  handle->current_x = 0;
  handle->current_y = 0;
  handle->inverted = 0;
  handle->initialized = 0;
  
  /* Wait for display to be ready */
  ssd1306_delay_ms(100);
  
  /* Send initialization sequence */
  status = ssd1306_send_init_sequence(handle);
  if (status != SSD1306_OK)
  {
    return status;
  }
  
  /* Clear buffer */
  ssd1306_fill(SSD1306_COLOR_BLACK);
  
  /* Update screen */
  status = ssd1306_update_screen(handle);
  if (status != SSD1306_OK)
  {
    return status;
  }
  
  /* Turn on display */
  status = ssd1306_display_on(handle);
  if (status != SSD1306_OK)
  {
    return status;
  }
  
  handle->initialized = 1;
  
  return SSD1306_OK;
}

ssd1306_status_t ssd1306_write_command(ssd1306_handle_t *handle, uint8_t cmd)
{
  HAL_StatusTypeDef hal_status;
  uint8_t data[2] = {SSD1306_CMD_PREFIX, cmd};
  
  if (handle == NULL || handle->hi2c == NULL)
  {
    return SSD1306_ERROR;
  }
  
  hal_status = HAL_I2C_Master_Transmit(handle->hi2c, SSD1306_I2C_ADDR, data, 2, SSD1306_TIMEOUT_MS);
  
  switch (hal_status)
  {
    case HAL_OK:
      return SSD1306_OK;
    case HAL_TIMEOUT:
      return SSD1306_TIMEOUT;
    default:
      return SSD1306_ERROR;
  }
}

ssd1306_status_t ssd1306_write_data(ssd1306_handle_t *handle, uint8_t *data, uint16_t length)
{
  HAL_StatusTypeDef hal_status;
  uint8_t *tx_buffer;
  uint16_t tx_length = length + 1;
  
  if (handle == NULL || handle->hi2c == NULL || data == NULL || length == 0)
  {
    return SSD1306_ERROR;
  }
  
  /* Allocate buffer for data prefix + data */
  tx_buffer = (uint8_t*)malloc(tx_length);
  if (tx_buffer == NULL)
  {
    return SSD1306_ERROR;
  }
  
  /* Prepare data with prefix */
  tx_buffer[0] = SSD1306_DATA_PREFIX;
  memcpy(&tx_buffer[1], data, length);
  
  hal_status = HAL_I2C_Master_Transmit(handle->hi2c, SSD1306_I2C_ADDR, tx_buffer, tx_length, SSD1306_TIMEOUT_MS);
  
  free(tx_buffer);
  
  switch (hal_status)
  {
    case HAL_OK:
      return SSD1306_OK;
    case HAL_TIMEOUT:
      return SSD1306_TIMEOUT;
    default:
      return SSD1306_ERROR;
  }
}

ssd1306_status_t ssd1306_update_screen(ssd1306_handle_t *handle)
{
  ssd1306_status_t status;
  uint8_t page;
  
  if (handle == NULL)
  {
    return SSD1306_ERROR;
  }
  
  for (page = 0; page < 8; page++)
  {
    /* Set page address */
    status = ssd1306_write_command(handle, 0xB0 + page);
    if (status != SSD1306_OK)
    {
      return status;
    }
    
    /* Set column address */
    status = ssd1306_write_command(handle, 0x00); // Low column
    if (status != SSD1306_OK)
    {
      return status;
    }
    
    status = ssd1306_write_command(handle, 0x10); // High column
    if (status != SSD1306_OK)
    {
      return status;
    }
    
    /* Send page data */
    status = ssd1306_write_data(handle, &g_ssd1306_buffer[SSD1306_WIDTH * page], SSD1306_WIDTH);
    if (status != SSD1306_OK)
    {
      return status;
    }
  }
  
  return SSD1306_OK;
}

void ssd1306_fill(ssd1306_color_t color)
{
  uint16_t i;
  uint8_t fill_value = (color == SSD1306_COLOR_BLACK) ? 0x00 : 0xFF;
  
  for (i = 0; i < SSD1306_BUFFER_SIZE; i++)
  {
    g_ssd1306_buffer[i] = fill_value;
  }
}

void ssd1306_draw_pixel(uint16_t x, uint16_t y, ssd1306_color_t color)
{
  uint16_t buffer_index;
  uint8_t bit_position;
  
  /* Check bounds */
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
  {
    return;
  }
  
  /* Calculate buffer position */
  buffer_index = x + (y / 8) * SSD1306_WIDTH;
  bit_position = y % 8;
  
  /* Set or clear pixel */
  if (color == SSD1306_COLOR_WHITE)
  {
    g_ssd1306_buffer[buffer_index] |= (1 << bit_position);
  }
  else
  {
    g_ssd1306_buffer[buffer_index] &= ~(1 << bit_position);
  }
}

ssd1306_color_t ssd1306_get_pixel(uint16_t x, uint16_t y)
{
  uint16_t buffer_index;
  uint8_t bit_position;
  
  /* Check bounds */
  if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
  {
    return SSD1306_COLOR_BLACK;
  }
  
  /* Calculate buffer position */
  buffer_index = x + (y / 8) * SSD1306_WIDTH;
  bit_position = y % 8;
  
  /* Return pixel color */
  return (g_ssd1306_buffer[buffer_index] & (1 << bit_position)) ? SSD1306_COLOR_WHITE : SSD1306_COLOR_BLACK;
}

void ssd1306_goto_xy(ssd1306_handle_t *handle, uint16_t x, uint16_t y)
{
  if (handle == NULL)
  {
    return;
  }
  
  handle->current_x = x;
  handle->current_y = y;
}

ssd1306_status_t ssd1306_display_on(ssd1306_handle_t *handle)
{
  return ssd1306_write_command(handle, SSD1306_CMD_DISPLAY_ON);
}

ssd1306_status_t ssd1306_display_off(ssd1306_handle_t *handle)
{
  return ssd1306_write_command(handle, SSD1306_CMD_DISPLAY_OFF);
}

uint8_t ssd1306_write_char(ssd1306_handle_t *handle, char ch, const font_t *font, ssd1306_color_t color)
{
  uint16_t x, y;
  uint8_t byte_index, bit_index;
  const uint8_t *char_data;
  uint8_t char_index;
  
  /* Validate inputs */
  if (handle == NULL || font == NULL)
  {
    return 0;
  }
  
  /* Check if character is printable */
  if (ch < FONT_6X8_FIRST_CHAR || ch > FONT_6X8_LAST_CHAR)
  {
    ch = ' '; /* Replace with space for non-printable characters */
  }
  
  /* Check bounds */
  if (handle->current_x + font->width > SSD1306_WIDTH || 
      handle->current_y + font->height > SSD1306_HEIGHT)
  {
    return 0;
  }
  
  /* Calculate character index in font data */
  char_index = ch - FONT_6X8_FIRST_CHAR;
  char_data = &font->data[char_index * font->width];
  
  /* Draw character pixel by pixel */
  for (x = 0; x < font->width; x++)
  {
    for (y = 0; y < font->height; y++)
    {
      /* Extract bit from font data */
      byte_index = x;
      bit_index = y;
      
      if (char_data[byte_index] & (1 << bit_index))
      {
        ssd1306_draw_pixel(handle->current_x + x, handle->current_y + y, color);
      }
      else
      {
        /* Draw background pixel (opposite color) */
        ssd1306_draw_pixel(handle->current_x + x, handle->current_y + y, 
                          (color == SSD1306_COLOR_WHITE) ? SSD1306_COLOR_BLACK : SSD1306_COLOR_WHITE);
      }
    }
  }
  
  /* Advance cursor */
  handle->current_x += font->width;
  
  return font->width;
}

uint16_t ssd1306_write_string(ssd1306_handle_t *handle, const char *str, const font_t *font, ssd1306_color_t color)
{
  uint16_t total_width = 0;
  uint8_t char_width;
  
  /* Validate inputs */
  if (handle == NULL || str == NULL || font == NULL)
  {
    return 0;
  }
  
  /* Write each character */
  while (*str != '\0')
  {
    /* Handle newline character */
    if (*str == '\n')
    {
      handle->current_x = 0;
      handle->current_y += font->height;
      
      /* Check if we've gone off the bottom of the screen */
      if (handle->current_y + font->height > SSD1306_HEIGHT)
      {
        break;
      }
    }
    /* Handle carriage return */
    else if (*str == '\r')
    {
      handle->current_x = 0;
    }
    /* Regular character */
    else
    {
      char_width = ssd1306_write_char(handle, *str, font, color);
      if (char_width == 0)
      {
        /* Character didn't fit, stop writing */
        break;
      }
      total_width += char_width;
    }
    
    str++;
  }
  
  return total_width;
}

ssd1306_status_t ssd1306_text_demo(ssd1306_handle_t *handle)
{
  ssd1306_status_t status;
  
  if (handle == NULL)
  {
    return SSD1306_ERROR;
  }
  
  /* Clear screen */
  ssd1306_fill(SSD1306_COLOR_BLACK);
  
  /* Demo various text functions */
  ssd1306_write_string_at(handle, 0, 0,  "SSD1306 Demo", &font_6x8, SSD1306_COLOR_WHITE);
  ssd1306_write_string_at(handle, 0, 10, "0123456789",  &font_6x8, SSD1306_COLOR_WHITE);
  ssd1306_write_string_at(handle, 0, 20, "ABCDEFGHIJK", &font_6x8, SSD1306_COLOR_WHITE);
  ssd1306_write_string_at(handle, 0, 30, "abcdefghijk", &font_6x8, SSD1306_COLOR_WHITE);
  ssd1306_write_string_at(handle, 0, 40, "!@#$%^&*()",  &font_6x8, SSD1306_COLOR_WHITE);
  ssd1306_write_string_at(handle, 0, 50, "128x64 OLED", &font_6x8, SSD1306_COLOR_WHITE);
  
  /* Update display */
  status = ssd1306_update_screen(handle);
  
  return status;
}

uint16_t ssd1306_write_string_at(ssd1306_handle_t *handle, uint16_t x, uint16_t y, 
                                 const char *str, const font_t *font, ssd1306_color_t color)
{
  if (handle == NULL)
  {
    return 0;
  }
  
  /* Set cursor position */
  ssd1306_goto_xy(handle, x, y);
  
  /* Write string */
  return ssd1306_write_string(handle, str, font, color);
}

/* Private definitions ----------------------------------------------- */
static ssd1306_status_t ssd1306_send_init_sequence(ssd1306_handle_t *handle)
{
  ssd1306_status_t status;
  
  if (handle == NULL)
  {
    return SSD1306_ERROR;
  }
  
  /* Display OFF */
  status = ssd1306_write_command(handle, SSD1306_CMD_DISPLAY_OFF);
  if (status != SSD1306_OK) return status;
  
  /* Set Memory Addressing Mode to Horizontal */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_MEMORY_MODE);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, SSD1306_MEMORY_MODE_PAGE);
  if (status != SSD1306_OK) return status;
  
  /* Set Page Start Address */
  status = ssd1306_write_command(handle, 0xB0);
  if (status != SSD1306_OK) return status;
  
  /* Set COM Output Scan Direction */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_COM_SCAN_DIR);
  if (status != SSD1306_OK) return status;
  
  /* Set low column address */
  status = ssd1306_write_command(handle, 0x00);
  if (status != SSD1306_OK) return status;
  
  /* Set high column address */
  status = ssd1306_write_command(handle, 0x10);
  if (status != SSD1306_OK) return status;
  
  /* Set start line address */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_START_LINE);
  if (status != SSD1306_OK) return status;
  
  /* Set contrast control register */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_CONTRAST);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, 0xFF);
  if (status != SSD1306_OK) return status;
  
  /* Set segment re-map */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_SEGMENT_REMAP);
  if (status != SSD1306_OK) return status;
  
  /* Set normal display */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_NORMAL_DISPLAY);
  if (status != SSD1306_OK) return status;
  
  /* Set multiplex ratio */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_MULTIPLEX_RATIO);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, 0x3F); // 64-1
  if (status != SSD1306_OK) return status;
  
  /* Set display offset */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_DISPLAY_OFFSET);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, 0x00);
  if (status != SSD1306_OK) return status;
  
  /* Set display clock divide ratio */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_DISPLAY_CLOCK);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, 0x80);
  if (status != SSD1306_OK) return status;
  
  /* Set pre-charge period */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_PRECHARGE);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, 0xF1);
  if (status != SSD1306_OK) return status;
  
  /* Set COM pins configuration */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_COM_PINS);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, 0x12);
  if (status != SSD1306_OK) return status;
  
  /* Set VCOM detect */
  status = ssd1306_write_command(handle, SSD1306_CMD_SET_VCOM_DETECT);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, 0x40);
  if (status != SSD1306_OK) return status;
  
  /* Enable charge pump */
  status = ssd1306_write_command(handle, SSD1306_CMD_CHARGE_PUMP);
  if (status != SSD1306_OK) return status;
  status = ssd1306_write_command(handle, 0x14);
  if (status != SSD1306_OK) return status;
  
  return SSD1306_OK;
}

/* End of file -------------------------------------------------------- */

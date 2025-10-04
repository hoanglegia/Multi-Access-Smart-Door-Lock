/**
 * @file       driver_st7735.c
 * @copyright  Copyright (c) 2025
 * @license    
 * @version    1.0.0
 * @date       2025-17-9
 * @author     Hoang Le
 * @brief      Driver for TFT LCD Display (ST7735)
 * @note       None
 * @example    None
 */

/* Public includes ---------------------------------------------------------- */
#include "driver_st7735.h"

/* Private includes --------------------------------------------------------- */
/* Private defines ---------------------------------------------------------- */
/* System Function Commands */
#define ST7735_SOFT_RESET                     (0x01)    /**< Software reset command, reinitializes display */
#define ST7735_SLEEP_OUT                      (0x11)    /**< Exit sleep mode command */
#define ST7735_DISPLAY_ON                     (0x29)    /**< Turn on display command */
#define ST7735_DISPLAY_INVERSION_OFF          (0x20)    /**< Disable display inversion command */

/* Memory and Address Commands */
#define ST7735_MEMORY_DATA_ACCESS_CONTROL     (0x36)    /**< Set memory access control (orientation) */
#define ST7735_INTERFACE_PIXEL_FORMAT         (0x3A)    /**< Set color mode (16/18-bit) */
#define ST7735_COLUMN_ADDRESS_SET             (0x2A)    /**< Set column address window */
#define ST7735_ROW_ADDRESS_SET                (0x2B)    /**< Set row address window */
#define ST7735_MEMORY_WRITE                   (0x2C)    /**< Write data to defined memory area */

/* Frame Rate Control Commands */
#define ST7735_FRAME_RATE_CONTROL_NORMAL      (0xB1)    /**< Set frame rate in normal mode */
#define ST7735_FRAME_RATE_CONTROL_IDLE        (0xB2)    /**< Set frame rate in idle mode */
#define ST7735_FRAME_RATE_CONTROL_PARTIAL     (0xB3)    /**< Set frame rate in partial mode */
#define ST7735_DISPLAY_INVERSION_CONTROL      (0xB4)    /**< Set display inversion mode */

/* Power Control Commands */
#define ST7735_POWER_CONTROL_1                (0xC0)    /**< Set GVDD level */
#define ST7735_POWER_CONTROL_2                (0xC1)    /**< Set VGH and VGL supply */
#define ST7735_POWER_CONTROL_3                (0xC2)    /**< Set op-amp current */
#define ST7735_POWER_CONTROL_4                (0xC3)    /**< Set BCLK/2, Opamp current small & medium low */
#define ST7735_POWER_CONTROL_5                (0xC4)    /**< Set BCLK/2, Opamp current small & medium high */
#define ST7735_VCOM_CONTROL_1                 (0xC5)    /**< Set VCOM voltage */

/* Gamma Adjustment Commands */
#define ST7735_POSITIVE_GAMMA_CORRECTION      (0xE0)    /**< Set positive gamma correction */
#define ST7735_NEGATIVE_GAMMA_CORRECTION      (0xE1)    /**< Set negative gamma correction */

/* Control Commands */
#define ST7735_CMD_END                        (0xFF)    /**< End of command list marker */

/* Timing Constants */
#define ST7735_RESET_DELAY                    (20U)     /**< Hardware reset delay in ms */
#define ST7735_INIT_DELAY                     (150U)    /**< Init sequence delay in ms */
#define ST7735_SLEEP_OUT_DELAY                (255U)    /**< Sleep out delay in ms */
#define ST7735_DISPLAY_ON_DELAY               (100U)    /**< Display on delay in ms */
#define ST7735_SPI_TIMEOUT                    (100U)    /**< SPI timeout in ms */

/* Private enumerate/structure ---------------------------------------------- */
/* Private macros ----------------------------------------------------------- */
#define ST7735_CHECK_STATUS(call) \
    do { \
        st7735_status_t status = (call); \
        if (status != ST7735_OK) { \
            return status; \
        } \
    } while(0)

/* Public variables --------------------------------------------------------- */
const uint8_t ST7735_FRAME_RATE_CONTROL_NORMAL_VAL[] = {0x01, 0x2C, 0x2D};
const uint8_t ST7735_FRAME_RATE_CONTROL_IDLE_VAL[] = {0x01, 0x2C, 0x2D};
const uint8_t ST7735_FRAME_RATE_CONTROL_PARTIAL_VAL[] = {0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D};
const uint8_t ST7735_DISPLAY_INVERSION_CONTROL_VAL[] = {0x07};
const uint8_t ST7735_POWER_CONTROL_1_VAL[] = {0xA2, 0x02, 0x84};
const uint8_t ST7735_POWER_CONTROL_2_VAL[] = {0xC5};
const uint8_t ST7735_POWER_CONTROL_3_VAL[] = {0x0A, 0x00};
const uint8_t ST7735_POWER_CONTROL_4_VAL[] = {0x8A, 0x2A};
const uint8_t ST7735_POWER_CONTROL_5_VAL[] = {0x8A, 0xEE};
const uint8_t ST7735_VCOM_CONTROL_1_VAL[] = {0x0E};
const uint8_t ST7735_POSITIVE_GAMMA_CORRECTION_VAL[] = {0x0F, 0x1A, 0x0F, 0x18, 0x2F, 0x28, 0x20, 0x22, 0x1F, 0x1B, 0x23, 0x37, 0x00, 0x07, 0x02, 0x10};
const uint8_t ST7735_NEGATIVE_GAMMA_CORRECTION_VAL[] = {0x0F, 0x1B, 0x0F, 0x17, 0x33, 0x2C, 0x29, 0x2E, 0x30, 0x30, 0x39, 0x3F, 0x00, 0x07, 0x03, 0x10};
const uint8_t ST7735_MEMORY_DATA_ACCESS_CONTROL_VAL = 0xC8; // Row/column exchange, BGR
const uint8_t ST7735_INTERFACE_PIXEL_FORMAT_VAL = 0x05;     // 16-bit/pixel

/* Private variables -------------------------------------------------------- */
static const st7735_init_cmd_t init_cmds[] = {
  {ST7735_FRAME_RATE_CONTROL_NORMAL, 3, ST7735_FRAME_RATE_CONTROL_NORMAL_VAL},
  {ST7735_FRAME_RATE_CONTROL_IDLE, 3, ST7735_FRAME_RATE_CONTROL_IDLE_VAL},
  {ST7735_FRAME_RATE_CONTROL_PARTIAL, 6, ST7735_FRAME_RATE_CONTROL_PARTIAL_VAL},
  {ST7735_DISPLAY_INVERSION_CONTROL, 1, ST7735_DISPLAY_INVERSION_CONTROL_VAL},
  {ST7735_POWER_CONTROL_1, 3, ST7735_POWER_CONTROL_1_VAL},
  {ST7735_POWER_CONTROL_2, 1, ST7735_POWER_CONTROL_2_VAL},
  {ST7735_POWER_CONTROL_3, 2, ST7735_POWER_CONTROL_3_VAL},
  {ST7735_POWER_CONTROL_4, 2, ST7735_POWER_CONTROL_4_VAL},
  {ST7735_POWER_CONTROL_5, 2, ST7735_POWER_CONTROL_5_VAL},
  {ST7735_VCOM_CONTROL_1, 1, ST7735_VCOM_CONTROL_1_VAL},
  {ST7735_POSITIVE_GAMMA_CORRECTION, 16, ST7735_POSITIVE_GAMMA_CORRECTION_VAL},
  {ST7735_NEGATIVE_GAMMA_CORRECTION, 16, ST7735_NEGATIVE_GAMMA_CORRECTION_VAL},
  {ST7735_CMD_END, 0, NULL}
};
/* Private prototypes ------------------------------------------------------- */

/* Public implementations --------------------------------------------------- */

st7735_status_t st7735_write_cmd(st7735_t *me, uint8_t cmd)
{
  if (me == NULL || me->hspi == NULL) 
    return ST7735_PARAM_ERROR;

  HAL_GPIO_WritePin(me->port, me->pin_a0, GPIO_PIN_RESET); // A0 = 0 for command
  HAL_GPIO_WritePin(me->port, me->pin_cs, GPIO_PIN_RESET); // CS = 0, select display

  if (HAL_SPI_Transmit(me->hspi, &cmd, 1, ST7735_SPI_TIMEOUT) != HAL_OK)
  {
    HAL_GPIO_WritePin(me->port, me->pin_cs, GPIO_PIN_SET); // CS = 1, deselect display
    return ST7735_SPI_ERROR;
  }
  
  HAL_GPIO_WritePin(me->port, me->pin_cs, GPIO_PIN_SET);   // CS = 1, deselect display
  return ST7735_OK;
}

st7735_status_t st7735_write_data(st7735_t *me, uint8_t data)
{
  if (me == NULL || me->hspi == NULL) 
    return ST7735_PARAM_ERROR;

  HAL_GPIO_WritePin(me->port, me->pin_a0, GPIO_PIN_SET);   // A0 = 1 for data mode
  HAL_GPIO_WritePin(me->port, me->pin_cs, GPIO_PIN_RESET); // CS = 0, select display

  if (HAL_SPI_Transmit(me->hspi, &data, 1, ST7735_SPI_TIMEOUT) != HAL_OK)
  {
    HAL_GPIO_WritePin(me->port, me->pin_cs, GPIO_PIN_SET); // CS = 1, deselect display
    return ST7735_SPI_ERROR;
  }
  
  HAL_GPIO_WritePin(me->port, me->pin_cs, GPIO_PIN_SET);   // CS = 1, deselect display
  return ST7735_OK;
}

st7735_status_t st7735_send_cmd_list(st7735_t *me, const st7735_init_cmd_t *cmd_list)
{
  if (me == NULL || cmd_list == NULL) 
    return ST7735_PARAM_ERROR;

  /* Process each command in the list */
  for (uint8_t i = 0; cmd_list[i].cmd != ST7735_CMD_END; i++) {
    uint8_t cmd = cmd_list[i].cmd;
    ST7735_CHECK_STATUS(st7735_write_cmd(me, cmd));                      // Send command byte

    /* Send command arguments if any */
    for (uint8_t j = 0; j < cmd_list[i].num_args; j++) {
      ST7735_CHECK_STATUS(st7735_write_data(me, cmd_list[i].args[j]));   // Send each argument byte
    }
  }
  return ST7735_OK;
}

st7735_status_t st7735_init(st7735_t *me)
{
  if (me == NULL || me->hspi == NULL || me->port == NULL) 
    return ST7735_PARAM_ERROR;
  
  /* Hardware reset sequence */
  HAL_GPIO_WritePin(me->port, me->pin_reset, GPIO_PIN_RESET);                      // Pull reset pin low
  HAL_Delay(ST7735_RESET_DELAY);                                                   // Hold reset for 20ms
  HAL_GPIO_WritePin(me->port, me->pin_reset, GPIO_PIN_SET);                        // Release reset
  HAL_Delay(ST7735_INIT_DELAY);                                                    // Wait for reset recovery

  /* Software reset and wake up */
  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_SOFT_RESET));                    // Software reset
  HAL_Delay(ST7735_INIT_DELAY);                                                    // Wait for reset to complete
  
  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_SLEEP_OUT));                     // Exit sleep mode
  HAL_Delay(ST7735_SLEEP_OUT_DELAY);                                               // Wait for sleep out
  
  /* Initialize display parameters */
  ST7735_CHECK_STATUS(st7735_send_cmd_list(me, init_cmds));                        // Send initialization commands

  /* Configure display settings */
  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_MEMORY_DATA_ACCESS_CONTROL));    // Set display orientation
  ST7735_CHECK_STATUS(st7735_write_data(me, ST7735_MEMORY_DATA_ACCESS_CONTROL_VAL));

  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_INTERFACE_PIXEL_FORMAT));        // Set color mode (16-bit per pixel)
  ST7735_CHECK_STATUS(st7735_write_data(me, ST7735_INTERFACE_PIXEL_FORMAT_VAL));

  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_DISPLAY_INVERSION_OFF));         // Disable display inversion

  /* Set full display window and turn on display */
  ST7735_CHECK_STATUS(st7735_set_pos(me, 0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1));
  
  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_DISPLAY_ON));                    // Turn on display
  HAL_Delay(ST7735_DISPLAY_ON_DELAY);                                              // Wait for display to stabilize
  
  return ST7735_OK;
}

st7735_status_t st7735_set_pos(st7735_t *me, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  if (x0 >= ST7735_WIDTH || y0 >= ST7735_HEIGHT || 
      x1 >= ST7735_WIDTH || y1 >= ST7735_HEIGHT ||
      x1 < x0 || y1 < y0) 
  {
    return ST7735_PARAM_ERROR;
  }

  /* Set column address window */
  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_COLUMN_ADDRESS_SET));
  ST7735_CHECK_STATUS(st7735_write_data(me, 0x00));     // Start column high byte
  ST7735_CHECK_STATUS(st7735_write_data(me, x0));       // Start column low byte
  ST7735_CHECK_STATUS(st7735_write_data(me, 0x00));     // End column high byte
  ST7735_CHECK_STATUS(st7735_write_data(me, x1));       // End column low byte

  /* Set row address window */
  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_ROW_ADDRESS_SET));
  ST7735_CHECK_STATUS(st7735_write_data(me, 0x00));     // Start row high byte
  ST7735_CHECK_STATUS(st7735_write_data(me, y0));       // Start row low byte
  ST7735_CHECK_STATUS(st7735_write_data(me, 0x00));     // End row high byte
  ST7735_CHECK_STATUS(st7735_write_data(me, y1));       // End row low byte
  
  return ST7735_OK;
}

st7735_status_t st7735_draw_pixel(st7735_t *me, uint8_t x, uint8_t y, uint16_t color)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  if (x >= ST7735_WIDTH || y >= ST7735_HEIGHT) 
    return ST7735_PARAM_ERROR;

  ST7735_CHECK_STATUS(st7735_set_pos(me, x, y, x, y));                   // Set window to single pixel
  
  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_MEMORY_WRITE));        // Prepare to write pixel data
  
  ST7735_CHECK_STATUS(st7735_write_data(me, (uint8_t)(color >> 8)));     // Send color high byte (ST7735 expects MSB first for 16-bit color)
  ST7735_CHECK_STATUS(st7735_write_data(me, (uint8_t)(color & 0xFF)));   // Send color low byte
  
  return ST7735_OK;
}

st7735_status_t st7735_draw_char(st7735_t *me, uint8_t x, uint8_t y, char ch, font_t font, uint16_t color, uint16_t bgcolor)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  /* Validate input character (printable ASCII only) */
  if (ch < 32 || ch > 127) 
    return ST7735_PARAM_ERROR;
  /* Check if character would be drawn outside display bounds */
  if((x >= ST7735_WIDTH) || (y >= ST7735_HEIGHT) || 
     ((x + font.width) > ST7735_WIDTH) || 
     ((y + font.height) > ST7735_HEIGHT))
  {
    return ST7735_PARAM_ERROR;
  }

  uint16_t line;

  /* Draw character pixel by pixel */
  for(uint8_t i = 0; i < font.height; i++)
  {
    line = font.data[(ch - 32) * font.height + i];    // Get font data for current row
    for(uint8_t j = 0; j < font.width; j++)
    {
      if((line << j) & 0x8000)                        // Move to next pixel in font data and check if pixel is set in font
        ST7735_CHECK_STATUS(st7735_draw_pixel(me, x + j, y + i, color));   // Draw foreground color
      else
        ST7735_CHECK_STATUS(st7735_draw_pixel(me, x + j, y + i, bgcolor)); // Draw background color
    }
  }
  return ST7735_OK;
}

st7735_status_t st7735_draw_string(st7735_t *me, uint8_t x, uint8_t y, char *str, font_t font, uint16_t color, uint16_t bgcolor)
{
  if (!str || !me) 
    return ST7735_PARAM_ERROR;
  
  uint8_t current_x = x;
  uint8_t current_y = y;

  while(*str)
  {
    /* Check for line wrap */
    if((current_x + font.width) > ST7735_WIDTH)          // If next char would exceed display width
    {
      current_x = 0;                                     // Return to left margin
      current_y += font.height;                          // Move to next line
      if((current_y + font.height) > ST7735_HEIGHT)      // If next line would exceed display height
        break;                                           // Stop drawing string
    }

    ST7735_CHECK_STATUS(st7735_draw_char(me, current_x, current_y, *str, font, color, bgcolor));
    
    current_x += font.width;                             // Move to next character position
    str++;                                               // Move to next character in string
  }
  return ST7735_OK;
}

st7735_status_t st7735_full_screen(st7735_t *me, uint16_t color)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  
  /* Set address window to entire display */
  ST7735_CHECK_STATUS(st7735_set_pos(me, 0, 0, ST7735_WIDTH - 1, ST7735_HEIGHT - 1));

  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_MEMORY_WRITE));
  for(uint16_t i = 0; i < ST7735_WIDTH * ST7735_HEIGHT; i++)
  {
		ST7735_CHECK_STATUS(st7735_write_data(me, color >> 8)); 
		ST7735_CHECK_STATUS(st7735_write_data(me, color & 0xFF));
  }
  return ST7735_OK;
}

st7735_status_t st7735_draw_rectangle(st7735_t *me, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  if (x0 >= ST7735_WIDTH || y0 >= ST7735_HEIGHT || 
      x1 >= ST7735_WIDTH || y1 >= ST7735_HEIGHT ||
      x1 < x0 || y1 < y0) 
  {
    return ST7735_PARAM_ERROR;
  }

  /* Set address window to rectangle area */
  ST7735_CHECK_STATUS(st7735_set_pos(me, x0, y0, x1, y1));

  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_MEMORY_WRITE));
  for(uint16_t x = x0; x <= x1; x++)
  {
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, x, y0, color)); // Top edge
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, x, y1, color)); // Bottom edge
  }
  for(uint16_t y = y0; y <= y1; y++)
  {
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, x0, y, color)); // Left edge
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, x1, y, color)); // Right edge
  }
  return ST7735_OK;
}

st7735_status_t st7735_draw_rhombus(st7735_t *me, uint8_t x, uint8_t y, uint8_t d, uint16_t color)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  if (d == 0 || (d % 2) != 0) // d must be even and >0
    return ST7735_PARAM_ERROR;

  uint8_t half_d = d / 2;

  /* Check if rhombus fits within display bounds */
  if (x < half_d || x + half_d >= ST7735_WIDTH || 
      y < half_d || y + half_d >= ST7735_HEIGHT) 
  {
    return ST7735_PARAM_ERROR;
  }

  /* Draw rhombus by connecting vertices */
  for (uint8_t i = 0; i <= half_d; i++) {
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, x - i, y + (half_d - i), color)); // Bottem to left
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, x + i, y + (half_d - i), color)); // Bottem to right
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, x - i, y - (half_d - i), color)); // Top to left
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, x + i, y - (half_d - i), color)); // Top to right
  }
  return ST7735_OK;
}

st7735_status_t st7735_draw_circle(st7735_t *me, uint8_t x, uint8_t y, uint8_t r, uint16_t color)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  if (r == 0) // Radius must be >0
    return ST7735_PARAM_ERROR;

  /* Check if circle fits within display bounds */
  if (x < r || x + r >= ST7735_WIDTH || 
      y < r || y + r >= ST7735_HEIGHT) 
  {
    return ST7735_PARAM_ERROR;
  }

  for(uint16_t angle = 0; angle < 360; angle++)
  {
    uint8_t px = x + (uint8_t)(r * cos(angle * 3.14159 / 180.0)); // Calculate x coordinate
    uint8_t py = y + (uint8_t)(r * sin(angle * 3.14159 / 180.0)); // Calculate y coordinate
    ST7735_CHECK_STATUS(st7735_draw_pixel(me, px, py, color));    // Draw pixel at calculated position
  }
  return ST7735_OK;
}

st7735_status_t st7735_fill_rectangle(st7735_t *me, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  if (x0 >= ST7735_WIDTH || y0 >= ST7735_HEIGHT || 
      x1 >= ST7735_WIDTH || y1 >= ST7735_HEIGHT ||
      x1 < x0 || y1 < y0) 
  {
    return ST7735_PARAM_ERROR;
  }

  /* Set address window to rectangle area */
  ST7735_CHECK_STATUS(st7735_set_pos(me, x0, y0, x1, y1));

  ST7735_CHECK_STATUS(st7735_write_cmd(me, ST7735_MEMORY_WRITE));
  for(uint16_t i = 0; i < (x1 - x0 + 1) * (y1 - y0 + 1); i++)
  {
		ST7735_CHECK_STATUS(st7735_write_data(me, color & 0xFF));   // Send color low byte
		ST7735_CHECK_STATUS(st7735_write_data(me, color >> 8));     // Send color high byte
  }
  return ST7735_OK;
}

st7735_status_t st7735_fill_rhombus(st7735_t *me, uint8_t x, uint8_t y, uint8_t d, uint16_t color)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  if (d == 0 || (d % 2) != 0) // d must be even and >0
    return ST7735_PARAM_ERROR;

  uint8_t half_d = d / 2;

  /* Check if rhombus fits within display bounds */
  if (x < half_d || x + half_d >= ST7735_WIDTH || 
      y < half_d || y + half_d >= ST7735_HEIGHT) 
  {
    return ST7735_PARAM_ERROR;
  }

  /* Fill rhombus by drawing horizontal lines between left and right edges */
  for (uint8_t i = 0; i <= half_d; i++) {
    ST7735_CHECK_STATUS(st7735_fill_rectangle(me, x - i, y - (half_d - i), x + i, y - (half_d - i), color)); // Top half
    ST7735_CHECK_STATUS(st7735_fill_rectangle(me, x - i, y + (half_d - i), x + i, y + (half_d - i), color)); // Bottom half
  }
  return ST7735_OK;
}

st7735_status_t st7735_draw_string_center(st7735_t *me, uint8_t y, char *str, font_t font, uint16_t color, uint16_t bgcolor)
{
  if (!str || !me) 
    return ST7735_PARAM_ERROR;
  
  /* Calculate string pixel width */
  uint16_t str_len = strlen(str);
  uint16_t pixel_width = str_len * font.width;

  /* Calculate starting x position for centered text */
  if (pixel_width > ST7735_WIDTH) 
    return ST7735_PARAM_ERROR; // String too wide to fit
  uint8_t x = (ST7735_WIDTH - pixel_width) / 2;

  return st7735_draw_string(me, x, y, str, font, color, bgcolor);
}

st7735_status_t st7735_draw_password_mask(st7735_t *me, uint8_t entered_digits, uint8_t total_digits, uint8_t y, uint16_t active_color, uint16_t inactive_color)
{
  if (me == NULL) 
    return ST7735_PARAM_ERROR;
  if (entered_digits > total_digits || total_digits == 0 || total_digits > 10) 
    return ST7735_PARAM_ERROR;

  uint8_t rhombus_d = 8; // Rhombus diagonal length
  uint8_t spacing = 4;   // Space between rhombuses
  uint8_t total_mask_width = total_digits * rhombus_d + (total_digits - 1) * spacing;
  uint8_t start_x = (ST7735_WIDTH - total_mask_width) / 2;
  uint8_t current_x_center;

  for (uint8_t i = 0; i < total_digits; i++)
  {
    current_x_center = start_x + (rhombus_d / 2) + (i * (rhombus_d + spacing));
    if (i < entered_digits)
    {
      ST7735_CHECK_STATUS(st7735_fill_rhombus(me, current_x_center, y, rhombus_d, active_color));
    }
    else
    {
      ST7735_CHECK_STATUS(st7735_draw_rhombus(me, current_x_center, y, rhombus_d, inactive_color));
    }
  }
  
  return ST7735_OK;
}

/* Private implementations -------------------------------------------------- */


/* End of file -------------------------------------------------------------- */
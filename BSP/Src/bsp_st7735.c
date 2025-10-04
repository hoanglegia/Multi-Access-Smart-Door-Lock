/**
 * @file       bsp_st7735.c
 * @copyright  Copyright (c) 2025
 * @license    
 * @version    1.0.0
 * @date       2025-20-9
 * @author     Hoang Le
 * @brief      Board support package for TFT LCD Display (ST7735)
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "bsp_st7735.h"

/* Public defines ----------------------------------------------------------- */
/* SPI Handle */
#define ST7735_HSPI                       hspi1
#define ST7735_HSPI_INSTANCE              SPI1
/* Public enumerate/structure ----------------------------------------------- */
/* Public macros ------------------------------------------------------------ */
/* Public variables --------------------------------------------------------- */
SPI_HandleTypeDef ST7735_HSPI;
/* Private variables -------------------------------------------------------- */
static st7735_t st7735_device = 
{
  .port = NULL,
  .pin_cs = 0,
  .pin_a0 = 0,
  .pin_reset = 0,
  .hspi = NULL,
};

static st7735_handler_t st7735_bsp_handler = 
{
  .st7735 = &st7735_device,
  .st7735_is_init = false,
  .status = ST7735_ERROR,
};

/* Public implementations --------------------------------------------------- */

st7735_status_t bsp_st7735_gpio_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  ST7735_HAL_SPI_PORT_CLK_ENABLE();
  ST7735_HAL_GPIO_PORT_CLK_ENABLE();

  /*Configure ST7735 control pins (CS, A0, RST) as Output Push Pull */
  HAL_GPIO_WritePin(ST7735_GPIO_PORT, ST7735_GPIO_PIN_CS | ST7735_GPIO_PIN_A0 | ST7735_GPIO_PIN_RST, GPIO_PIN_RESET);

  GPIO_InitStruct.Pin = ST7735_GPIO_PIN_CS | ST7735_GPIO_PIN_A0 | ST7735_GPIO_PIN_RST;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(ST7735_GPIO_PORT, &GPIO_InitStruct);

  return ST7735_OK;
}

st7735_status_t bsp_st7735_spi_init(void)
{
  /* Peripheral clock enable */
  ST7735_HAL_SPI_CLK_ENABLE();
  ST7735_HAL_SPI_PORT_CLK_ENABLE(); 
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  GPIO_InitStruct.Pin = ST7735_SPI_SCK_PIN | ST7735_SPI_MOSI_PIN; // SCK, MOSI
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(ST7735_SPI_GPIO_PORT, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = ST7735_SPI_MISO_PIN; // MISO
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(ST7735_SPI_GPIO_PORT, &GPIO_InitStruct);

  ST7735_HSPI.Instance = ST7735_HSPI_INSTANCE;
  ST7735_HSPI.Init.Mode = SPI_MODE_MASTER;
  ST7735_HSPI.Init.Direction = SPI_DIRECTION_2LINES; // Full-duplex, but we mostly transmit
  ST7735_HSPI.Init.DataSize = SPI_DATASIZE_8BIT;
  ST7735_HSPI.Init.CLKPolarity = SPI_POLARITY_LOW;   // ST7735 typically uses CPOL=0
  ST7735_HSPI.Init.CLKPhase = SPI_PHASE_1EDGE;       // ST7735 typically uses CPHA=0
  ST7735_HSPI.Init.NSS = SPI_NSS_SOFT;               // Software slave select
  ST7735_HSPI.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2; // Max speed (PCLK/2), adjust if unstable
  ST7735_HSPI.Init.FirstBit = SPI_FIRSTBIT_MSB;
  ST7735_HSPI.Init.TIMode = SPI_TIMODE_DISABLE;
  ST7735_HSPI.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  ST7735_HSPI.Init.CRCPolynomial = 10;

  if (HAL_SPI_Init(&ST7735_HSPI) != HAL_OK)
  {
    return ST7735_SPI_ERROR;
  }

  return ST7735_OK;
}

st7735_status_t bsp_st7735_init(void)
{
  // Initialize GPIOs
  BSP_ST7735_INIT_CHECK(bsp_st7735_gpio_init());

  // Initialize SPI
  BSP_ST7735_INIT_CHECK(bsp_st7735_spi_init());
  
  // Link BSP hardware resources to the ST7735 driver instance
  st7735_bsp_handler.st7735->port = ST7735_GPIO_PORT;
  st7735_bsp_handler.st7735->pin_cs = ST7735_GPIO_PIN_CS;
  st7735_bsp_handler.st7735->pin_a0 = ST7735_GPIO_PIN_A0;
  st7735_bsp_handler.st7735->pin_reset = ST7735_GPIO_PIN_RST;
  st7735_bsp_handler.st7735->hspi = &ST7735_HSPI;

  // Perform ST7735 chip initialization
  st7735_status_t status = st7735_init(st7735_bsp_handler.st7735);
  if (status == ST7735_OK)
  {
    st7735_bsp_handler.st7735_is_init = true;
    st7735_bsp_handler.status = ST7735_OK; // Set BSP status to OK after successful init
  }
  else
  {
    st7735_bsp_handler.st7735_is_init = false;
    st7735_bsp_handler.status = status;
  }

  return status;
}

st7735_status_t bsp_st7735_write_cmd(uint8_t cmd)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_write_cmd(st7735_bsp_handler.st7735, cmd));
}

st7735_status_t bsp_st7735_write_data(uint8_t data)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_write_data(st7735_bsp_handler.st7735, data));
}

st7735_status_t bsp_st7735_set_pos(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_set_pos(st7735_bsp_handler.st7735, x0, y0, x1, y1));
}

st7735_status_t bsp_st7735_draw_pixel(uint8_t x, uint8_t y, uint16_t color)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_draw_pixel(st7735_bsp_handler.st7735, x, y, color));
}

st7735_status_t bsp_st7735_draw_char(uint8_t x, uint8_t y, char ch, font_t font, uint16_t color, uint16_t bgcolor)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_draw_char(st7735_bsp_handler.st7735, x, y, ch, font, color, bgcolor));
}

st7735_status_t bsp_st7735_draw_string(uint8_t x, uint8_t y, char *str, font_t font, uint16_t color, uint16_t bgcolor)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_draw_string(st7735_bsp_handler.st7735, x, y, str, font, color, bgcolor));
}

st7735_status_t bsp_st7735_full_screen(uint16_t color)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_full_screen(st7735_bsp_handler.st7735, color));
}

st7735_status_t bsp_st7735_draw_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_draw_rectangle(st7735_bsp_handler.st7735, x0, y0, x1, y1, color));
}

st7735_status_t bsp_st7735_draw_rhombus(uint8_t x, uint8_t y, uint8_t d, uint16_t color)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_draw_rhombus(st7735_bsp_handler.st7735, x, y, d, color));
}

st7735_status_t bsp_st7735_draw_circle(uint8_t x, uint8_t y, uint8_t r, uint16_t color)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_draw_circle(st7735_bsp_handler.st7735, x, y, r, color));
}

st7735_status_t bsp_st7735_fill_rectangle(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint16_t color)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_fill_rectangle(st7735_bsp_handler.st7735, x0, y0, x1, y1, color));
}

st7735_status_t bsp_st7735_fill_rhombus(uint8_t x, uint8_t y, uint8_t d, uint16_t color)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_fill_rhombus(st7735_bsp_handler.st7735, x, y, d, color));
}

st7735_status_t bsp_st7735_draw_string_center(uint8_t y, char *str, font_t font, uint16_t color, uint16_t bgcolor)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_draw_string_center(st7735_bsp_handler.st7735, y, str, font, color, bgcolor));
}

st7735_status_t bsp_st7735_draw_password_mask(uint8_t entered_digits, uint8_t total_digits, uint8_t y, uint16_t active_color, uint16_t inactive_color)
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  BSP_ST7735_CHECK_STATUS(&st7735_bsp_handler, st7735_draw_password_mask(st7735_bsp_handler.st7735, entered_digits, total_digits, y, active_color, inactive_color));
}

st7735_status_t bsp_st7735_display()
{
  BSP_ST7735_CHECK_HANDLER_INIT(&st7735_bsp_handler);
  st7735_full_screen(st7735_bsp_handler.st7735, ST7735_BLUE);
	uint8_t text1_width = strlen("COOL BOYS ' HOUSE") * font_7x10.width;
  uint8_t text_height = font_7x10.height;
  uint8_t text1_x = (ST7735_WIDTH - text1_width) / 2;
  uint8_t text1_y = 10;
	st7735_fill_rectangle(st7735_bsp_handler.st7735, text1_x - 3, text1_y - 3, text1_x + text1_width + 2, text1_y + text_height + 2, ST7735_BLACK);
  st7735_fill_rectangle(st7735_bsp_handler.st7735, text1_x - 2, text1_y - 2, text1_x + text1_width + 1, text1_y + text_height + 1, ST7735_WHITE);
	st7735_draw_string_center(st7735_bsp_handler.st7735, 10, "COOL BOYS ' HOUSE", font_7x10, ST7735_BLACK, ST7735_WHITE);

	st7735_draw_string_center(st7735_bsp_handler.st7735, 35, "Enter Password", font_7x10, ST7735_WHITE, ST7735_BLUE);
  uint8_t password_mask_y = 55;
  st7735_draw_password_mask(st7735_bsp_handler.st7735, 0, 8, password_mask_y, ST7735_GREEN, ST7735_WHITE);


  uint8_t method_x_start = 5; 
  uint8_t method_y_start = 90; 
  uint8_t line_spacing = 20; 
	uint8_t text2_width = strlen("1.Keypad") * font_7x10.width;
	
	st7735_fill_rectangle(st7735_bsp_handler.st7735, method_x_start - 1, method_y_start - 1, method_x_start + text2_width + 1, method_y_start + 11, ST7735_WHITE); 
  st7735_draw_string(st7735_bsp_handler.st7735, method_x_start, method_y_start, "1.Keypad", font_7x10, ST7735_BLACK, ST7735_GREEN);
  st7735_draw_string(st7735_bsp_handler.st7735, method_x_start, method_y_start + line_spacing, "2.RFID", font_7x10, ST7735_WHITE, ST7735_BLUE);
  st7735_draw_string(st7735_bsp_handler.st7735, method_x_start, method_y_start + (2 * line_spacing), "3.Fingerprint", font_7x10, ST7735_WHITE, ST7735_BLUE);
}

/* -------------------------------------------------------------------------- */

#ifdef __cplusplus
} /* extern "C" { */
#endif


/* End of file -------------------------------------------------------------- */
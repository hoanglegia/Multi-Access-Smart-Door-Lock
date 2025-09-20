/**
 * @file       bsp_ssd1306.c
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    2.0.0
 * @date       2025-09-16
 * @author     Tran Van Diep
 *             
 * @brief      BSP SSD1306 OLED Display Implementation
 *             Board Support Package implementation with version control and hardware integration
 * @note       Eliminates separate I2C layer, integrates I2C directly into SSD1306 BSP
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_ssd1306.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include "stm32f4xx_hal.h"
#include <string.h>

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static I2C_HandleTypeDef g_hi2c_ssd1306;   /*!< I2C handle for SSD1306 */
static ssd1306_handle_t  g_ssd1306_handle; /*!< SSD1306 device handle */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief  Configure GPIO pins for I2C according to hardware version
 *
 * @param[in]  config  Pointer to BSP configuration structure
 *
 * @return  
 *  - BSP_SSD1306_OK: GPIO configuration successful
 *  - BSP_SSD1306_ERROR: Invalid parameters
 *  - BSP_SSD1306_HW_ERROR: GPIO configuration failed
 */
static bsp_ssd1306_status_t bsp_ssd1306_gpio_config(bsp_ssd1306_config_t *config);

/**
 * @brief  Configure I2C peripheral according to hardware version
 *
 * @param[in]  config  Pointer to BSP configuration structure
 *
 * @return  
 *  - BSP_SSD1306_OK: I2C configuration successful
 *  - BSP_SSD1306_ERROR: Invalid parameters
 *  - BSP_SSD1306_HW_ERROR: I2C configuration failed
 */
static bsp_ssd1306_status_t bsp_ssd1306_i2c_config(bsp_ssd1306_config_t *config);

/* Function definitions ----------------------------------------------- */
bsp_ssd1306_status_t bsp_ssd1306_init_system(bsp_ssd1306_config_t *config)
{
  bsp_ssd1306_status_t status;
  
  if (config == NULL)
  {
    return BSP_SSD1306_ERROR;
  }
  
  /* Initialize configuration structure */
  config->hi2c = &g_hi2c_ssd1306;
  config->version = BSP_SSD1306_VERSION;
  config->initialized = 0;
  
  /* Initialize hardware */
  status = bsp_ssd1306_hw_init(config);
  if (status != BSP_SSD1306_OK)
  {
    return status;
  }
  
  config->initialized = 1;
  
  return BSP_SSD1306_OK;
}

bsp_ssd1306_status_t bsp_ssd1306_hw_init(bsp_ssd1306_config_t *config)
{
  bsp_ssd1306_status_t status;
  
  if (config == NULL || config->hi2c == NULL)
  {
    return BSP_SSD1306_ERROR;
  }
  
  /* Configure GPIO pins */
  status = bsp_ssd1306_gpio_config(config);
  if (status != BSP_SSD1306_OK)
  {
    return status;
  }
  
  /* Configure I2C peripheral */
  status = bsp_ssd1306_i2c_config(config);
  if (status != BSP_SSD1306_OK)
  {
    return status;
  }
  
  return BSP_SSD1306_OK;
}

bsp_ssd1306_status_t bsp_ssd1306_init_display(bsp_ssd1306_config_t *config)
{
  ssd1306_status_t ssd_status;
  
  if (config == NULL || config->hi2c == NULL || !config->initialized)
  {
    return BSP_SSD1306_ERROR;
  }
  
  /* Setup SSD1306 device handle */
  g_ssd1306_handle.hi2c = config->hi2c;
  
  /* Initialize SSD1306 driver */
  ssd_status = ssd1306_init(&g_ssd1306_handle);
  
  switch (ssd_status)
  {
    case SSD1306_OK:
      return BSP_SSD1306_OK;
    case SSD1306_TIMEOUT:
      return BSP_SSD1306_TIMEOUT;
    default:
      return BSP_SSD1306_ERROR;
  }
}

bsp_ssd1306_status_t bsp_ssd1306_display_on(bsp_ssd1306_config_t *config)
{
  ssd1306_status_t ssd_status;
  
  if (config == NULL || !config->initialized)
  {
    return BSP_SSD1306_ERROR;
  }
  
  ssd_status = ssd1306_display_on(&g_ssd1306_handle);
  
  switch (ssd_status)
  {
    case SSD1306_OK:
      return BSP_SSD1306_OK;
    case SSD1306_TIMEOUT:
      return BSP_SSD1306_TIMEOUT;
    default:
      return BSP_SSD1306_ERROR;
  }
}

bsp_ssd1306_status_t bsp_ssd1306_display_off(bsp_ssd1306_config_t *config)
{
  ssd1306_status_t ssd_status;
  
  if (config == NULL || !config->initialized)
  {
    return BSP_SSD1306_ERROR;
  }
  
  ssd_status = ssd1306_display_off(&g_ssd1306_handle);
  
  switch (ssd_status)
  {
    case SSD1306_OK:
      return BSP_SSD1306_OK;
    case SSD1306_TIMEOUT:
      return BSP_SSD1306_TIMEOUT;
    default:
      return BSP_SSD1306_ERROR;
  }
}

void bsp_ssd1306_clear(void)
{
  ssd1306_fill(SSD1306_COLOR_BLACK);
}

bsp_ssd1306_status_t bsp_ssd1306_display(bsp_ssd1306_config_t *config)
{
  ssd1306_status_t ssd_status;
  
  if (config == NULL || !config->initialized)
  {
    return BSP_SSD1306_ERROR;
  }
  
  ssd_status = ssd1306_update_screen(&g_ssd1306_handle);
  
  switch (ssd_status)
  {
    case SSD1306_OK:
      return BSP_SSD1306_OK;
    case SSD1306_TIMEOUT:
      return BSP_SSD1306_TIMEOUT;
    default:
      return BSP_SSD1306_ERROR;
  }
}

uint16_t bsp_ssd1306_write_string_default(bsp_ssd1306_config_t *config, const char *str)
{
  if (config == NULL || str == NULL || !config->initialized)
  {
    return 0;
  }
  
  return ssd1306_write_string(&g_ssd1306_handle, str, &font_6x8, SSD1306_COLOR_WHITE);
}

uint16_t bsp_ssd1306_write_string_at(bsp_ssd1306_config_t *config, uint16_t x, uint16_t y, const char *str)
{
  if (config == NULL || str == NULL || !config->initialized)
  {
    return 0;
  }
  
  return ssd1306_write_string_at(&g_ssd1306_handle, x, y, str, &font_6x8, SSD1306_COLOR_WHITE);
}

uint8_t bsp_ssd1306_get_version(bsp_ssd1306_config_t *config)
{
  if (config == NULL)
  {
    return 0;
  }
  
  return config->version;
}

/* Private function implementations ----------------------------------- */
static bsp_ssd1306_status_t bsp_ssd1306_gpio_config(bsp_ssd1306_config_t *config)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  if (config == NULL)
  {
    return BSP_SSD1306_ERROR;
  }

  /* Enable GPIO clocks */
  // __HAL_RCC_GPIOB_CLK_ENABLE();
  if (BSP_SSD1306_I2C_INSTANCE == I2C1)
    __HAL_RCC_GPIOB_CLK_ENABLE();
  else if (BSP_SSD1306_I2C_INSTANCE == I2C2)
    __HAL_RCC_GPIOB_CLK_ENABLE();
  else if (BSP_SSD1306_I2C_INSTANCE == I2C3)
    __HAL_RCC_GPIOC_CLK_ENABLE();
  else
    return BSP_SSD1306_HW_ERROR;

  
  /* Configure I2C1 GPIO pins: SCL and SDA */
  GPIO_InitStruct.Pin = BSP_SSD1306_I2C_SCL_PIN | BSP_SSD1306_I2C_SDA_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = BSP_SSD1306_I2C_ALT_FUNC;
  
  HAL_GPIO_Init(BSP_SSD1306_I2C_SCL_PORT, &GPIO_InitStruct);
  
  /* enable i2c clocks */
  if (BSP_SSD1306_I2C_INSTANCE == I2C1)
    __HAL_RCC_I2C1_CLK_ENABLE();
  else if (BSP_SSD1306_I2C_INSTANCE == I2C2)
    __HAL_RCC_I2C2_CLK_ENABLE();
  else if (BSP_SSD1306_I2C_INSTANCE == I2C3)
    __HAL_RCC_I2C3_CLK_ENABLE();
  else
    return BSP_SSD1306_HW_ERROR;
  return BSP_SSD1306_OK;
}

static bsp_ssd1306_status_t bsp_ssd1306_i2c_config(bsp_ssd1306_config_t *config)
{
  HAL_StatusTypeDef hal_status;
  
  if (config == NULL || config->hi2c == NULL)
  {
    return BSP_SSD1306_ERROR;
  }
  
#if (BSP_SSD1306_VERSION == 1)
  /* Enable I2C1 clock */
  __HAL_RCC_I2C1_CLK_ENABLE();
  
  /* Configure I2C1 parameters */
  config->hi2c->Instance = BSP_SSD1306_I2C_INSTANCE;
  config->hi2c->Init.ClockSpeed = BSP_SSD1306_I2C_CLOCK_SPEED;
  config->hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
  config->hi2c->Init.OwnAddress1 = 0;
  config->hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  config->hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  config->hi2c->Init.OwnAddress2 = 0;
  config->hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  config->hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  
#elif (BSP_SSD1306_VERSION == 2)
  /* Enable I2C2 clock */
  __HAL_RCC_I2C2_CLK_ENABLE();
  
  /* Configure I2C2 parameters */
  config->hi2c->Instance = BSP_SSD1306_I2C_INSTANCE;
  config->hi2c->Init.ClockSpeed = BSP_SSD1306_I2C_CLOCK_SPEED;
  config->hi2c->Init.DutyCycle = I2C_DUTYCYCLE_2;
  config->hi2c->Init.OwnAddress1 = 0;
  config->hi2c->Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  config->hi2c->Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  config->hi2c->Init.OwnAddress2 = 0;
  config->hi2c->Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  config->hi2c->Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
#endif
  
  /* Initialize I2C */
  hal_status = HAL_I2C_Init(config->hi2c);
  if (hal_status != HAL_OK)
  {
    return BSP_SSD1306_HW_ERROR;
  }
  
  return BSP_SSD1306_OK;
}

bsp_ssd1306_status_t bsp_ssd1306_init(bsp_ssd1306_handle_t *handle,
                                      bsp_system_config_t *system_config)
{
  bsp_ssd1306_status_t status;
  
  if (handle == NULL || system_config == NULL)
  {
    return BSP_SSD1306_ERROR;
  }
  
  /* Initialize BSP system first */
  status = bsp_ssd1306_init_system(handle);
  if (status != BSP_SSD1306_OK)
  {
    return status;
  }
  
  /* Initialize hardware */
  status = bsp_ssd1306_hw_init(handle);
  if (status != BSP_SSD1306_OK)
  {
    return status;
  }
  
  /* Initialize display driver */
  status = bsp_ssd1306_init_display(handle);
  if (status != BSP_SSD1306_OK)
  {
    return status;
  }
  
  return BSP_SSD1306_OK;
}

bsp_ssd1306_status_t bsp_ssd1306_clear_screen(bsp_ssd1306_handle_t *handle)
{
  if (handle == NULL || !handle->initialized)
  {
    return BSP_SSD1306_ERROR;
  }
  
  bsp_ssd1306_clear();
  return BSP_SSD1306_OK;
}

bsp_ssd1306_status_t bsp_ssd1306_write_string(bsp_ssd1306_handle_t *handle, 
                                               uint16_t x, uint16_t y, const char *str)
{
  if (handle == NULL || !handle->initialized || str == NULL)
  {
    return BSP_SSD1306_ERROR;
  }
  
  /* Convert line number to pixel Y coordinate (8 pixels per line) */
  uint16_t pixel_y = y * 8;
  
  bsp_ssd1306_write_string_at(handle, x, pixel_y, str);
  return BSP_SSD1306_OK;
}

bsp_ssd1306_status_t bsp_ssd1306_update_screen(bsp_ssd1306_handle_t *handle)
{
  if (handle == NULL || !handle->initialized)
  {
    return BSP_SSD1306_ERROR;
  }
  
  return bsp_ssd1306_display(handle);
}

/* End of file -------------------------------------------------------- */

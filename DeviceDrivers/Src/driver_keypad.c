/**
 * @file       driver_keypad.c
 * @copyright  Copyright (C) 2023, Your Name. All rights reserved.
 * @license    This project is released under the MIT License.
 * @version    1.0.0
 * @date       2025-09-21
 * @author     Duy Nguyen Khanh
 *
 * @brief      Interrupt-driven Driver for 4x4 Matrix Keypad
 */

/* Includes ----------------------------------------------------------- */
#include "driver_keypad.h"

/* Private defines ---------------------------------------------------- */
#define DEBOUNCE_TIME_MS (50) /*!< Debounce time in milliseconds */

/* Private function prototypes ---------------------------------------- */
static void set_sleep_mode(keypad_t* handle);
static char scan_for_key(keypad_t* handle, uint8_t col_index);
static int8_t get_col_index(keypad_t* handle, uint16_t gpio_pin);

/* Function definitions ----------------------------------------------- */
void keypad_driver_init(keypad_t* handle, const keypad_config_t* config, const char (*keymap)[KEYPAD_NUM_COLS])
{
  handle->config = config;
  handle->keymap = keymap;
  handle->key_buffer = KEYPAD_NOT_PRESSED;
  handle->last_interrupt_time = 0;

  set_sleep_mode(handle);
}

void keypad_driver_handler(keypad_t* handle, uint16_t gpio_pin)
{
  int8_t col_index = get_col_index(handle, gpio_pin);
  if (col_index == -1)
  {
    return;
  }

  uint32_t current_time = HAL_GetTick();
  if ((current_time - handle->last_interrupt_time) > DEBOUNCE_TIME_MS)
  {
    char pressed_key = scan_for_key(handle, col_index);
    if (pressed_key != KEYPAD_NOT_PRESSED)
    {
      handle->key_buffer = pressed_key;
    }
    set_sleep_mode(handle);
  }
  handle->last_interrupt_time = current_time;
}

char keypad_driver_get_key(keypad_t* handle)
{
  char key = handle->key_buffer;
  handle->key_buffer = KEYPAD_NOT_PRESSED;
  return key;
}

/* Private definitions ----------------------------------------------- */
static void set_sleep_mode(keypad_t* handle)
{
  for (uint8_t i = 0; i < KEYPAD_NUM_ROWS; i++)
  {
    HAL_GPIO_WritePin(handle->config->rows[i].port, handle->config->rows[i].pin, GPIO_PIN_RESET);
  }
}

static char scan_for_key(keypad_t* handle, uint8_t col_index)
{
  char key = KEYPAD_NOT_PRESSED;

  for (uint8_t i = 0; i < KEYPAD_NUM_ROWS; i++)
  {
    HAL_GPIO_WritePin(handle->config->rows[i].port, handle->config->rows[i].pin, GPIO_PIN_SET);
  }

  for (uint8_t row = 0; row < KEYPAD_NUM_ROWS; row++)
  {
    HAL_GPIO_WritePin(handle->config->rows[row].port, handle->config->rows[row].pin, GPIO_PIN_RESET);
    //HAL_Delay(1);
    for (volatile uint32_t i = 0; i < 500; i++);

    if (HAL_GPIO_ReadPin(handle->config->cols[col_index].port, handle->config->cols[col_index].pin) == GPIO_PIN_RESET)
    {
      key = handle->keymap[row][col_index];
      break;
    }

    HAL_GPIO_WritePin(handle->config->rows[row].port, handle->config->rows[row].pin, GPIO_PIN_SET);
  }
  return key;
}

static int8_t get_col_index(keypad_t* handle, uint16_t gpio_pin)
{
  for (int i = 0; i < KEYPAD_NUM_COLS; i++)
  {
    if (gpio_pin == handle->config->cols[i].pin)
    {
      // A more robust check would also compare the port if multiple pins have the same number.
      // For this specific hardware, comparing pin number is sufficient.
      return i;
    }
  }
  return -1;
}

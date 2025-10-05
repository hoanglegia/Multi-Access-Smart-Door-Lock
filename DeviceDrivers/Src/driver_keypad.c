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
#include "bsp_keypad.h"
/* Private defines ---------------------------------------------------- */
#define DEBOUNCE_TIME_MS        (50) // 50ms debounce time

/* Private variables -------------------------------------------------- */
static const char g_keypad_map[BSP_KEYPAD_NUM_ROWS][BSP_KEYPAD_NUM_COLS] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

// Buffer to store the pressed key from ISR
static volatile char g_keypad_key_buffer = KEYPAD_NOT_PRESSED;
static volatile uint32_t g_last_interrupt_time = 0;

/* Private function prototypes ---------------------------------------- */
static void keypad_set_sleep_mode(void);
static char keypad_scan_for_key(uint8_t col_index);

/* Function definitions ----------------------------------------------- */
void keypad_init(void)
{
	bsp_keypad_init();
	keypad_set_sleep_mode();
}

void keypad_handler(uint8_t col_index)
{
  uint32_t current_time = HAL_GetTick();

  // Debounce check
  if ((current_time - g_last_interrupt_time) > DEBOUNCE_TIME_MS)
  {
    char pressed_key = keypad_scan_for_key(col_index);

    if (pressed_key != KEYPAD_NOT_PRESSED)
    {
      g_keypad_key_buffer = pressed_key;
    }

    // After scanning, return to sleep mode configuration
    keypad_set_sleep_mode();
  }
  g_last_interrupt_time = current_time;
}

char keypad_get_key(void)
{
  char key = g_keypad_key_buffer;
  g_keypad_key_buffer = KEYPAD_NOT_PRESSED; // Clear buffer after reading
  return key;
}

/* Private definitions ----------------------------------------------- */

/**
 * @brief  Configures the keypad for low-power sleep mode, ready for interrupts.
 *         Sets all row pins to LOW.
 */

static void keypad_set_sleep_mode(void)
{
  // Dùng hàm BSP để ghi
  for (uint8_t i = 0; i < BSP_KEYPAD_NUM_ROWS; i++)
  {
    bsp_keypad_write_row(i, BSP_KEYPAD_PIN_RESET);
  }
}

/**
 * @brief  Performs a selective scan to identify the exact key pressed.
 *
 * @param[in]  triggered_col_pin  The column pin that caused the interrupt.
 *
 * @return The character of the pressed key, or KEYPAD_NOT_PRESSED.
 */

static char keypad_scan_for_key(uint8_t col_index)
{
  char key = KEYPAD_NOT_PRESSED;

  // Dùng hàm BSP để ghi
  for (uint8_t i = 0; i < BSP_KEYPAD_NUM_ROWS; i++)
  {
    bsp_keypad_write_row(i, BSP_KEYPAD_PIN_SET);
  }

  for (uint8_t row = 0; row < BSP_KEYPAD_NUM_ROWS; row++)
  {
    bsp_keypad_write_row(row, BSP_KEYPAD_PIN_RESET); // Kích hoạt hàng hiện tại
    //HAL_Delay(1); // Delay nhỏ
    for (volatile uint32_t i = 0; i < 500; i++);

    // Dùng hàm BSP để đọc
    if (bsp_keypad_read_col(col_index) == BSP_KEYPAD_PIN_RESET)
    {
      key = g_keypad_map[row][col_index];
      break;
    }

    bsp_keypad_write_row(row, BSP_KEYPAD_PIN_SET); // Hủy kích hoạt hàng
  }

  return key;
}

/* End of file -------------------------------------------------------- */


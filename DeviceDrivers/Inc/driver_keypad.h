/**
 * @file       driver_keypad.h
 * @copyright  Copyright (C) 2023, Your Name. All rights reserved.
 * @license    This project is released under the MIT License.
 * @version    1.0.0
 * @date       2025-09-21
 * @author     Duy Nguyen Khanh
 *
 * @brief      Interrupt-driven Driver for 4x4 Matrix Keypad
 *
 * @note       This driver uses an interrupt-based, power-saving method.
 *             Rows should be configured as Output Push-Pull.
 *             Columns should be configured as External Interrupt with Pull-up.
 */

#ifndef __DRIVER_KEYPAD_H
#define __DRIVER_KEYPAD_H

/* Includes ----------------------------------------------------------- */
#include "stm32f1xx_hal.h"
#include <stdint.h>
/* Public defines ----------------------------------------------------- */
#define KEYPAD_NOT_PRESSED ('\0') /*!< Value returned when no key is pressed */
#define KEYPAD_NUM_ROWS    (4)    /*!< Number of rows in the keypad matrix */
#define KEYPAD_NUM_COLS    (4)    /*!< Number of columns in the keypad matrix */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Structure to define the GPIO pins for a single row or column.
 */
typedef struct
{
  GPIO_TypeDef* port; /*!< GPIO port */
  uint16_t      pin;  /*!< GPIO pin */
}
keypad_pin_t;

/**
 * @brief Configuration structure for the keypad, provided by the BSP.
 */
typedef struct
{
  keypad_pin_t rows[KEYPAD_NUM_ROWS]; /*!< Array of row pin configurations */
  keypad_pin_t cols[KEYPAD_NUM_COLS]; /*!< Array of column pin configurations */
}
keypad_config_t;

/**
 * @brief Keypad handle structure, contains all data for a keypad instance.
 */
typedef struct
{
  const keypad_config_t* config;              /*!< Pointer to the hardware configuration */
  const char (*keymap)[KEYPAD_NUM_COLS];     /*!< Pointer to the key mapping array */
  volatile char        key_buffer;            /*!< Single-character buffer for the pressed key */
  volatile uint32_t    last_interrupt_time;   /*!< Timestamp for debouncing */
}
keypad_t;

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initializes the keypad driver handle.
 *
 * @param[in]     handle  Pointer to the keypad handle structure.
 * @param[in]     config  Pointer to the hardware configuration structure.
 * @param[in]     keymap  Pointer to the 4x4 key mapping array.
 *
 * @return None.
 */
void keypad_driver_init(keypad_t* handle, const keypad_config_t* config, const char (*keymap)[KEYPAD_NUM_COLS]);

/**
 * @brief  Interrupt handler for the keypad driver, called by the BSP.
 *
 * @param[in,out] handle    Pointer to the keypad handle.
 * @param[in]     gpio_pin  The specific GPIO pin that triggered the interrupt.
 *
 * @return None.
 */
void keypad_driver_handler(keypad_t* handle, uint16_t gpio_pin);

/**
 * @brief  Gets the last pressed key from the driver's buffer.
 *
 * @param[in,out] handle  Pointer to the keypad handle.
 *
 * @return The character of the pressed key or KEYPAD_NOT_PRESSED.
 */
char keypad_driver_get_key(keypad_t* handle);

#endif // __DRIVER_KEYPAD_H

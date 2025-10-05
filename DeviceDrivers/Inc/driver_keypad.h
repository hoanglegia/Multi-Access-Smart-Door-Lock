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

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __KEYPAD_H
#define __KEYPAD_H

/* Includes ----------------------------------------------------------- */
#include "stm32f1xx_hal.h"
#include <stdint.h>
/* Public defines ----------------------------------------------------- */
#define KEYPAD_NOT_PRESSED ('\0') /*!< Value returned when no new key is available */

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initializes the keypad driver for interrupt-based operation.
 *
 * @param[in]  None
 *
 * @attention  GPIO pins must be configured in the IOC file before calling.
 *
 * @return None
 */
void keypad_init(void);

/**
 * @brief  This function is called when a keypad interrupt occurs.
 *         It handles the key detection logic.
 *
 * @param[in]  col_index  The logical index (0-3) of the column that triggered the interrupt.
 *
 * @return None
 */
void keypad_handler(uint8_t col_index);

/**
 * @brief  Gets the last key that was pressed. (Non-blocking)
 *
 * @param[in]  None
 *
 * @attention  This function should be called periodically in the main loop.
 *             It retrieves the key from a buffer and clears the buffer.
 *
 * @return
 *  - The character of the last pressed key.
 *  - KEYPAD_NOT_PRESSED ('\0') if no new key has been pressed since the last call.
 */
char keypad_get_key(void);

#endif // __KEYPAD_H

/* End of file -------------------------------------------------------- */

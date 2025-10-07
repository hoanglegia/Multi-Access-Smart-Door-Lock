/**
 * @file       bsp_keypad.h
 * @copyright  Copyright (C) 2023, Your Name. All rights reserved.
 * @license    This project is released under the MIT License.
 * @version    1.0.0
 * @date       2025-09-21
 * @author     Duy Nguyen Khanh
 *
 * @brief      Board Support Package for the 4x4 Keypad on STM32F103C8T6 board.
 */

#ifndef __BSP_KEYPAD_H
#define __BSP_KEYPAD_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
/**
 * @brief Hardware version selection for different board layouts.
 */
#define BSP_KEYPAD_VERSION 1

#if (BSP_KEYPAD_VERSION == 1)
/* Row Pin Configuration */
#define BSP_KEYPAD_ROW0_PORT GPIOA
#define BSP_KEYPAD_ROW0_PIN  GPIO_PIN_9
#define BSP_KEYPAD_ROW1_PORT GPIOA
#define BSP_KEYPAD_ROW1_PIN  GPIO_PIN_10
#define BSP_KEYPAD_ROW2_PORT GPIOA
#define BSP_KEYPAD_ROW2_PIN  GPIO_PIN_11
#define BSP_KEYPAD_ROW3_PORT GPIOA
#define BSP_KEYPAD_ROW3_PIN  GPIO_PIN_12

/* Column Pin Configuration */
#define BSP_KEYPAD_COL0_PORT GPIOA
#define BSP_KEYPAD_COL0_PIN  GPIO_PIN_15
#define BSP_KEYPAD_COL1_PORT GPIOB
#define BSP_KEYPAD_COL1_PIN  GPIO_PIN_3
#define BSP_KEYPAD_COL2_PORT GPIOB
#define BSP_KEYPAD_COL2_PIN  GPIO_PIN_4
#define BSP_KEYPAD_COL3_PORT GPIOB
#define BSP_KEYPAD_COL3_PIN  GPIO_PIN_5

#define BSP_KEYPAD_GPIO_CLK_ENABLE()   do { __HAL_RCC_GPIOA_CLK_ENABLE(); \
                                            __HAL_RCC_GPIOB_CLK_ENABLE(); } while(0) /**< Enable clocks for all keypad ports */

#elif (BSP_KEYPAD_VERSION == 2) // Example of a second hardware version

/* === Row Pin Configuration (Board Version 2) === */
#define BSP_KEYPAD_ROW0_PORT          GPIOA               /**< GPIO port for Row 0 */
#define BSP_KEYPAD_ROW0_PIN           GPIO_PIN_0          /**< GPIO pin for Row 0 */
// ... (and so on for other pins of version 2)

/* === Column Pin Configuration (Board Version 2) === */
#define BSP_KEYPAD_COL0_PORT          GPIOA               /**< GPIO port for Column 0 */
#define BSP_KEYPAD_COL0_PIN           GPIO_PIN_4          /**< GPIO pin for Column 0 */
// ... (and so on for other pins of version 2)

/* === GPIO Clock Enable Macros (Board Version 2) === */
//#define BSP_KEYPAD_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE() /**< Enable clocks for all keypad ports */

#else
#error "No BSP_KEYPAD_VERSION defined or supported version selected!"
#endif /* BSP_KEYPAD_VERSION */

/* Public APIs for Application Layer ---------------------------------- */

/**
 * @brief  Initializes the Keypad hardware and its low-level driver.
 *
 * @param  None
 * @return None
 */
void bsp_keypad_init(void);

/**
 * @brief  Gets the last key that was pressed.
 *
 * @param  None
 * @return The character of the pressed key or a null character.
 */
char bsp_keypad_get_key(void);

///**
// * @brief  Interrupt callback to be called from the system's IRQ handler (stm32f1xx_it.c).
// *
// * @param[in]  gpio_pin  The pin that triggered the interrupt.
// * @return None.
// */
//void bsp_keypad_irq_callback(uint16_t gpio_pin);
//
#endif // __BSP_KEYPAD_H

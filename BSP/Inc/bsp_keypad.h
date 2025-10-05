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
#include "stm32f1xx_hal.h"
//#include "stm32f4xx_hal.h"

/* Public defines ----------------------------------------------------- */
#define BSP_KEYPAD_NUM_ROWS (4)
#define BSP_KEYPAD_NUM_COLS (4)

/**
 * @brief Hardware version selection for different board layouts.
 *        Uncomment the version you are using.
 */
#define BSP_KEYPAD_VERSION    1    // Version for PA9-12 (Rows) and PA15, PB3-5 (Cols)
// #define BSP_KEYPAD_VERSION    2

#if (BSP_KEYPAD_VERSION == 1)

/* === Row Pin Configuration (Board Version 1) === */
#define BSP_KEYPAD_ROW0_PORT          GPIOA               /**< GPIO port for Row 0 */
#define BSP_KEYPAD_ROW0_PIN           GPIO_PIN_9          /**< GPIO pin for Row 0 */
#define BSP_KEYPAD_ROW1_PORT          GPIOA               /**< GPIO port for Row 1 */
#define BSP_KEYPAD_ROW1_PIN           GPIO_PIN_10         /**< GPIO pin for Row 1 */
#define BSP_KEYPAD_ROW2_PORT          GPIOA               /**< GPIO port for Row 2 */
#define BSP_KEYPAD_ROW2_PIN           GPIO_PIN_11         /**< GPIO pin for Row 2 */
#define BSP_KEYPAD_ROW3_PORT          GPIOA               /**< GPIO port for Row 3 */
#define BSP_KEYPAD_ROW3_PIN           GPIO_PIN_12         /**< GPIO pin for Row 3 */

/* === Column Pin Configuration (Board Version 1) === */
#define BSP_KEYPAD_COL0_PORT          GPIOA               /**< GPIO port for Column 0 */
#define BSP_KEYPAD_COL0_PIN           GPIO_PIN_15         /**< GPIO pin for Column 0 */
#define BSP_KEYPAD_COL1_PORT          GPIOB               /**< GPIO port for Column 1 */
#define BSP_KEYPAD_COL1_PIN           GPIO_PIN_3          /**< GPIO pin for Column 1 */
#define BSP_KEYPAD_COL2_PORT          GPIOB               /**< GPIO port for Column 2 */
#define BSP_KEYPAD_COL2_PIN           GPIO_PIN_4          /**< GPIO pin for Column 2 */
#define BSP_KEYPAD_COL3_PORT          GPIOB               /**< GPIO port for Column 3 */
#define BSP_KEYPAD_COL3_PIN           GPIO_PIN_5          /**< GPIO pin for Column 3 */

/* === GPIO Clock Enable Macros (Board Version 1) === */
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
#define BSP_KEYPAD_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE() /**< Enable clocks for all keypad ports */

#else
#error "No BSP_KEYPAD_VERSION defined or supported version selected!"
#endif /* BSP_KEYPAD_VERSION */
/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Enumeration for GPIO pin state
 */
typedef enum
{
  BSP_KEYPAD_PIN_RESET = 0,
  BSP_KEYPAD_PIN_SET
}
bsp_keypad_pin_state_t;

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Initializes the GPIO pins used by the keypad.
 * @note This function is typically called by a global BSP_Init() function.
 *       For this example, it's called directly. GPIO Clocks must be enabled.
 */
void bsp_keypad_init(void);

/**
 * @brief Sets a specific row pin to a given state (SET or RESET).
 * @param row_index The index of the row (0 to 3).
 * @param pin_state The state to set the pin to.
 */
void bsp_keypad_write_row(uint8_t row_index, bsp_keypad_pin_state_t pin_state);

/**
 * @brief Reads the state of a specific column pin.
 * @param col_index The index of the column (0 to 3).
 * @return The state of the pin (BSP_KEYPAD_PIN_SET or BSP_KEYPAD_PIN_RESET).
 */
bsp_keypad_pin_state_t bsp_keypad_read_col(uint8_t col_index);

/**
 * @brief  Gets the logical column index from a physical GPIO pin.
 * @param  gpio_pin The HAL GPIO_Pin identifier (e.g., GPIO_PIN_15).
 * @return The logical column index (0-3), or -1 if the pin is not a keypad column.
 */
int8_t bsp_keypad_get_col_index(uint16_t gpio_pin);

//// moi them vao luc sau, ham callback xu li ngat
///**
// * @brief  The BSP's main interrupt handler for GPIO EXTI events.
// * @note   This function should be called from the HAL_GPIO_EXTI_Callback.
// *         It dispatches the interrupt to the appropriate higher-level driver.
// * @param  gpio_pin The pin that triggered the interrupt.
// */
//void bsp_keypad_irq_handler(uint16_t gpio_pin);

#endif // __BSP_KEYPAD_H


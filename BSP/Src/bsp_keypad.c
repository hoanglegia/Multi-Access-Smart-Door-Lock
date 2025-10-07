/**
 * @file       bsp_keypad.c
 * @copyright  Copyright (C) 2023, Your Name. All rights reserved.
 * @license    This project is released under the MIT License.
 * @version    1.0.0
 * @date       2025-09-21
 * @author     Duy Nguyen Khanh
 *
 * @brief      Board Support Package for the 4x4 Keypad.
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_keypad.h"
#include "driver_keypad.h"

/* Private variables -------------------------------------------------- */
static keypad_t g_keypad_handle; /*!< Handle for the low-level driver */

/* Keypad hardware configuration for the selected BSP version */
static const keypad_config_t g_keypad_config =
{
  .rows = {
    {BSP_KEYPAD_ROW0_PORT, BSP_KEYPAD_ROW0_PIN},
    {BSP_KEYPAD_ROW1_PORT, BSP_KEYPAD_ROW1_PIN},
    {BSP_KEYPAD_ROW2_PORT, BSP_KEYPAD_ROW2_PIN},
    {BSP_KEYPAD_ROW3_PORT, BSP_KEYPAD_ROW3_PIN},
  },
  .cols = {
    {BSP_KEYPAD_COL0_PORT, BSP_KEYPAD_COL0_PIN},
    {BSP_KEYPAD_COL1_PORT, BSP_KEYPAD_COL1_PIN},
    {BSP_KEYPAD_COL2_PORT, BSP_KEYPAD_COL2_PIN},
    {BSP_KEYPAD_COL3_PORT, BSP_KEYPAD_COL3_PIN},
  }
};

/* Key mapping for the keypad */
static const char g_keypad_map[KEYPAD_NUM_ROWS][KEYPAD_NUM_COLS] =
{
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

/* Public implementations --------------------------------------------- */
void bsp_keypad_init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* 1. Enable GPIO Clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /* 2. Configure Row pins as Output */
  GPIO_InitStruct.Pin = BSP_KEYPAD_ROW0_PIN | BSP_KEYPAD_ROW1_PIN | BSP_KEYPAD_ROW2_PIN | BSP_KEYPAD_ROW3_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(BSP_KEYPAD_ROW0_PORT, &GPIO_InitStruct);

  /* 3. Configure Column pins as Interrupt */
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;

  /* Columns on Port A */
  GPIO_InitStruct.Pin = BSP_KEYPAD_COL0_PIN;
  HAL_GPIO_Init(BSP_KEYPAD_COL0_PORT, &GPIO_InitStruct);

  /* Columns on Port B */
  GPIO_InitStruct.Pin = BSP_KEYPAD_COL1_PIN | BSP_KEYPAD_COL2_PIN | BSP_KEYPAD_COL3_PIN;
  HAL_GPIO_Init(BSP_KEYPAD_COL1_PORT, &GPIO_InitStruct);

  /* 4. Enable and set EXTI interrupt priority */
  HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

  /* 5. Initialize the low-level driver */
  keypad_driver_init(&g_keypad_handle, &g_keypad_config, g_keypad_map);
}

char bsp_keypad_get_key(void)
{
  return keypad_driver_get_key(&g_keypad_handle);
}

//void bsp_keypad_irq_callback(uint16_t gpio_pin)
//{
//  keypad_driver_handler(&g_keypad_handle, gpio_pin);
//}
/******************************************************************************/
/*           INTERRUPT SERVICE ROUTINE IMPLEMENTATIONS (WEAK OVERRIDE)        */
/******************************************************************************/

/**
  * @brief  EXTI line detection callbacks. Overrides the weak HAL implementation.
  * @param  GPIO_Pin: Specifies the pin that triggered the interrupt.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  // BSP gọi trực tiếp handler của DRIVER
  keypad_driver_handler(&g_keypad_handle, GPIO_Pin);

  // Lưu ý: Vì keypad_driver_handler đã có logic kiểm tra xem pin có thuộc keypad không,
  // chúng ta không cần lặp lại logic đó ở đây.
  // Nếu có các thiết bị khác, bạn có thể thêm else if.
  // else if (GPIO_Pin == BSP_USER_BUTTON_PIN) { ... }
}

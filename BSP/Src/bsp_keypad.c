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
//include moi
#include "driver_keypad.h"
/* Private defines ---------------------------------------------------- */
/* Private variables -------------------------------------------------- */
// Store pin configurations in arrays for easier access
static GPIO_TypeDef* const ROW_PORTS[BSP_KEYPAD_NUM_ROWS] = {
  BSP_KEYPAD_ROW0_PORT, BSP_KEYPAD_ROW1_PORT, BSP_KEYPAD_ROW2_PORT, BSP_KEYPAD_ROW3_PORT
};

static const uint16_t ROW_PINS[BSP_KEYPAD_NUM_ROWS] = {
  BSP_KEYPAD_ROW0_PIN, BSP_KEYPAD_ROW1_PIN, BSP_KEYPAD_ROW2_PIN, BSP_KEYPAD_ROW3_PIN
};

static GPIO_TypeDef* const COL_PORTS[BSP_KEYPAD_NUM_COLS] = {
  BSP_KEYPAD_COL0_PORT, BSP_KEYPAD_COL1_PORT, BSP_KEYPAD_COL2_PORT, BSP_KEYPAD_COL3_PORT
};

static const uint16_t COL_PINS[BSP_KEYPAD_NUM_COLS] = {
  BSP_KEYPAD_COL0_PIN, BSP_KEYPAD_COL1_PIN, BSP_KEYPAD_COL2_PIN, BSP_KEYPAD_COL3_PIN
};

/* Function definitions ----------------------------------------------- */
void bsp_keypad_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
    uint16_t col_pins_on_porta = 0;
    uint16_t col_pins_on_portb = 0;

	/* GPIO Ports Clock Enable */
    BSP_KEYPAD_GPIO_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
    /* 2. Configure Row pins */
    GPIO_InitStruct.Pin = BSP_KEYPAD_ROW0_PIN | BSP_KEYPAD_ROW1_PIN | BSP_KEYPAD_ROW2_PIN | BSP_KEYPAD_ROW3_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BSP_KEYPAD_ROW0_PORT, &GPIO_InitStruct); // Assuming all rows on the same port

    /* 3. Configure Column pins, grouping by port */
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLUP;

    // Check which pins are on which port based on definitions
    if (BSP_KEYPAD_COL0_PORT == GPIOA) col_pins_on_porta |= BSP_KEYPAD_COL0_PIN; else col_pins_on_portb |= BSP_KEYPAD_COL0_PIN;
    if (BSP_KEYPAD_COL1_PORT == GPIOA) col_pins_on_porta |= BSP_KEYPAD_COL1_PIN; else col_pins_on_portb |= BSP_KEYPAD_COL1_PIN;
    if (BSP_KEYPAD_COL2_PORT == GPIOA) col_pins_on_porta |= BSP_KEYPAD_COL2_PIN; else col_pins_on_portb |= BSP_KEYPAD_COL2_PIN;
    if (BSP_KEYPAD_COL3_PORT == GPIOA) col_pins_on_porta |= BSP_KEYPAD_COL3_PIN; else col_pins_on_portb |= BSP_KEYPAD_COL3_PIN;

    if (col_pins_on_porta != 0) {
      GPIO_InitStruct.Pin = col_pins_on_porta;
      HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }
    if (col_pins_on_portb != 0) {
      GPIO_InitStruct.Pin = col_pins_on_portb;
      HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }

    /* 4. Configure and enable EXTI interrupts */
    // This part is specific to the pins used in VERSION 1
    #if (BSP_KEYPAD_VERSION == 1)
	/* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI3_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI3_IRQn);

	HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI4_IRQn);

	HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

	HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	#endif

	 HAL_GPIO_WritePin(ROW_PORTS[0], ROW_PINS[0] | ROW_PINS[1] | ROW_PINS[2] | ROW_PINS[3], GPIO_PIN_SET);

}

void bsp_keypad_write_row(uint8_t row_index, bsp_keypad_pin_state_t pin_state)
{
  if (row_index >= BSP_KEYPAD_NUM_ROWS)
  {
    return; // Invalid index
  }
  HAL_GPIO_WritePin(ROW_PORTS[row_index], ROW_PINS[row_index], (GPIO_PinState)pin_state);
}

bsp_keypad_pin_state_t bsp_keypad_read_col(uint8_t col_index)
{
  if (col_index >= BSP_KEYPAD_NUM_COLS)
  {
    return BSP_KEYPAD_PIN_RESET; // Invalid index, return default
  }
  return (bsp_keypad_pin_state_t)HAL_GPIO_ReadPin(COL_PORTS[col_index], COL_PINS[col_index]);
}

int8_t bsp_keypad_get_col_index(uint16_t gpio_pin)
{
  for (int i = 0; i < BSP_KEYPAD_NUM_COLS; i++)
  {
    // So sánh pin gây ra ngắt với từng pin cột đã được định nghĩa trong BSP
    if (gpio_pin == COL_PINS[i])
    {
      return i; // Trả về chỉ số logic (0, 1, 2, hoặc 3)
    }
  }
  return -1; // Trả về -1 nếu pin này không thuộc về bàn phím
}

//void bsp_keypad_irq_handler(uint16_t gpio_pin)
//{
//    // Logic phân luồng bây giờ nằm trong BSP
//
//    // 1. Kiểm tra xem có phải ngắt từ Keypad không?
//    int8_t col_index = bsp_keypad_get_col_index(gpio_pin);
//    if (col_index != -1)
//    {
//        // BSP gọi ngược lên DRIVER
//        keypad_handler((uint8_t)col_index);
//        return;
//    }
//}
/******************************************************************************/
/*           INTERRUPT SERVICE ROUTINE IMPLEMENTATIONS                        */
/******************************************************************************/

/**
  * @brief  EXTI line detection callbacks.
  * @note   This function is defined here to override the weak one in the HAL library.
  *         It acts as the central dispatcher for all GPIO interrupts.
  * @param  GPIO_Pin: Specifies the pins connected to the EXTI line.
  * @retval None
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    // 1. Kiểm tra xem có phải ngắt từ Keypad không?
    int8_t col_index = bsp_keypad_get_col_index(GPIO_Pin);
    if (col_index != -1)
    {
        // BSP gọi lên DRIVER
        keypad_handler((uint8_t)col_index);
        return;
    }

    // Nếu dự án có các ngắt khác, bạn cần xử lý chúng ở đây.
    // Ví dụ:
    // if (GPIO_Pin == USER_BUTTON_PIN) {
    //     // Gọi hàm xử lý cho nút nhấn
    // }
}

/**
 * @file       bsp_sg90.c
 * @copyright  Copyright (C) 2025. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-27
 * @author     le Ba Hien
 *
 * @brief      Source file for SG90 BSP layer.
 * @note       This BSP provides an abstraction layer for the SG90 servo motor,
 *             handling hardware-specific initializations and exposing simplified APIs.
 */

/* Includes ------------------------------------------------------------------*/
#include "bsp_sg90.h"
#include "main.h"     /**< For HAL defines */

/* Private variables --------------------------------------------------------- */
extern TIM_HandleTypeDef htim2;               /**< Timer handle from main.c */
static bool bsp_servo_is_initialized = false; /**< Initialization flag */

/* Global instance ----------------------------------------------------------- */
bsp_servo_t servo1 = {
    .handle = {0},                            /**< Driver handle initialized to zero */
    .active = BSP_SERVO_DEACTIVE,             /**< Initially inactive */
    .angle  = 0                               /**< Initial angle set to 0 */
};

/* Private function prototypes ----------------------------------------------- */
/**
 * @brief  Initializes the low-level hardware (GPIO, SPI) required for the SG90 servo motor.
 * @return drv_sg90_status_t - DRV_SG90_OK on success, DRV_SG90_ERROR on failure.
 */
static drv_sg90_status_t bsp_sg90_hw_init(void);

/* Public implementations ---------------------------------------------------- */
drv_sg90_status_t bsp_sg90_init(bsp_servo_t *servo)
{
    // Check for null pointer to avoid dereferencing invalid memory
    if (servo == NULL) {
        return DRV_SG90_ERROR;
    }

    // Prevent re-initialization if already initialized
    if (bsp_servo_is_initialized) {
        return DRV_SG90_OK;
    }

    /* Step 1: Init low-level hardware (GPIO, Timer, PWM) */
    // Initialize hardware resources required for servo operation
    if (bsp_sg90_hw_init() != DRV_SG90_OK) {
        return DRV_SG90_ERROR;
    }

    /* Step 2: Init driver handle */
    // Initialize the SG90 driver and set default servo parameters
    if (drv_sg90_init(&servo->handle, &htim2, BSP_SG90_TIM_CHANNEL) == DRV_SG90_OK) {
        servo->active = BSP_SERVO_ACTIVE;
        servo->angle = 90;   /* Default center position */
        drv_sg90_set_angle(&servo->handle, servo->angle);

        bsp_servo_is_initialized = true;
        return DRV_SG90_OK;
    }

    // Return error if initialization fails
    return DRV_SG90_ERROR;
}

drv_sg90_status_t bsp_sg90_set_angle(bsp_servo_t *servo, uint8_t angle)
{
    // Validate input: ensure servo pointer is not NULL and servo is active
    if ((servo == NULL) || (servo->active != BSP_SERVO_ACTIVE)) {
        return DRV_SG90_ERROR;
    }

    // Set the servo angle using the driver function and update the BSP state
    if (drv_sg90_set_angle(&servo->handle, angle) == DRV_SG90_OK) {
        servo->angle = angle;
        return DRV_SG90_OK;
    }

    // Return error if driver fails to set angle
    return DRV_SG90_ERROR;
}

drv_sg90_status_t bsp_sg90_stop(bsp_servo_t *servo)
{
    if ((servo == NULL) || (servo->active != BSP_SERVO_ACTIVE)) {
        return DRV_SG90_ERROR;
    }

    if (drv_sg90_stop(&servo->handle) == DRV_SG90_OK) {
        servo->active = BSP_SERVO_DEACTIVE;
	    bsp_servo_is_initialized = false;
        return DRV_SG90_OK;
    }

    return DRV_SG90_ERROR;
}


/* Private implementations --------------------------------------------------- */
static drv_sg90_status_t bsp_sg90_hw_init(void)
{
    GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* Enable GPIO and Timer clock */
    BSP_SG90_GPIO_CLK_ENABLE();
    BSP_SG90_TIM_CLK_ENABLE();

    /* Configure PA0 as Alternate Function Push-Pull (TIM2_CH1) */
    GPIO_InitStruct.Pin = BSP_SG90_GPIO_PIN;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(BSP_SG90_GPIO_PORT, &GPIO_InitStruct);

    /* Timer init is handled by CubeMX (MX_TIM2_Init in main.c) */
    return DRV_SG90_OK;
}

/* End of file ---------------------------------------------------------------*/


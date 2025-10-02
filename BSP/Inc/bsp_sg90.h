/**
 * @file       bsp_sg90.h
 * @copyright  Copyright (C) 2025. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-27
 * @author     le Ba Hien
 *
 * @brief      Board Support Package (BSP) for SG90 servo motor
 * @note       This BSP provides an abstraction layer for the SG90 servo motor,
 *             handling hardware-specific initializations and exposing simplified APIs.
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_SG90_H
#define __BSP_SG90_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "drv_sg90.h"

/* Public defines ----------------------------------------------------- */
/**
 * @brief Hardware version selection for different board layouts.
 *        Currently, only version 1 is defined.
 */
#ifndef BSP_SG90_VERSION
#define BSP_SG90_VERSION    1    /**< Default hardware version */
#endif

#if (BSP_SG90_VERSION == 1)

/* === Timer & PWM Configuration === */
#define BSP_SG90_TIM_INSTANCE        TIM2                         /**< Timer instance */
#define BSP_SG90_TIM_CHANNEL         TIM_CHANNEL_1                /**< PWM Channel */

/* === Servo GPIO Pin Configuration === */
#define BSP_SG90_GPIO_PORT           GPIOA                        /**< Servo GPIO port */
#define BSP_SG90_GPIO_PIN            GPIO_PIN_0                   /**< Servo pin (PA0) */
#define BSP_SG90_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE() /**< GPIO Clock Enable */

/* === Timer Clock Enable === */
#define BSP_SG90_TIM_CLK_ENABLE()    __HAL_RCC_TIM2_CLK_ENABLE()  /**< Timer Clock Enable */

#endif /* BSP_SG90_VERSION == 1 */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief BSP servo status
 */
typedef enum {
    BSP_SERVO_DEACTIVE,  /**< Servo inactive */
    BSP_SERVO_ACTIVE     /**< Servo active */
} bsp_servo_status_t;

/**
 * @brief BSP servo structure
 */
typedef struct {
    sg90_handle_t       handle;  /**< SG90 driver handle */
    bsp_servo_status_t  active;  /**< Active status */
    uint8_t             angle;   /**< Current angle */
} bsp_servo_t;

/* Global instance */
extern bsp_servo_t servo1;  /**< Global servo instance */

/* Public BSP APIs */
/**
 * @brief Initializes the BSP for the SG90 servo motor, including hardware and driver.
 * @note Must be called once before using other BSP functions.
 * @return drv_sg90_status_t Operation status.
 */
drv_sg90_status_t bsp_sg90_init(bsp_servo_t *servo);

/**
 * @brief Sets the angle of the SG90 servo motor.
 * @param angle Desired angle (0-180 degrees).
 * @return drv_sg90_status_t Operation status.
 */
drv_sg90_status_t bsp_sg90_set_angle(bsp_servo_t *servo, uint8_t angle);

/**
 * @brief Stops the SG90 servo motor (disable PWM output).
 * @param servo Pointer to BSP servo instance.
 * @return drv_sg90_status_t Operation status.
 */
drv_sg90_status_t bsp_sg90_stop(bsp_servo_t *servo);

#endif /* __BSP_SG90_H */

/* End of file -------------------------------------------------------- */

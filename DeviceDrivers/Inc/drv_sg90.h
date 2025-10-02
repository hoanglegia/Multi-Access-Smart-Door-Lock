/**
 * @file       drv_sg90.h
 * @copyright  Copyright (C) 202025. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-27
 * @author     Le Ba Hien
 *
 * @brief      SG90 Servo driver for abstracting PWM control.
 *
 * @note       This driver allows pin connection initialization and angle control for SG90 servo.
 */

 /* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DRV_SG90_H
#define __DRV_SG90_H

/* Includes ----------------------------------------------------------- */
#include "stm32f1xx_hal.h"
#include <stdint.h>
#include <stdbool.h>

/* Servo configuration -------------------------------------------------*/
#define SG90_MIN_PULSE   500   // 0.5 ms pulse -> 0°
#define SG90_MAX_PULSE   2500  // 2.5 ms pulse -> 180°
#define SG90_MIN_ANGLE   0     // Minimum servo angle
#define SG90_MAX_ANGLE   180   // Maximum servo angle

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Status codes for SG90 driver
 */
typedef enum {
    DRV_SG90_OK = 0,         /**< Operation with SG90 completed successfully */
    DRV_SG90_ERROR,          /**< Operation with SG90 failed */
    DRV_SG90_INVALID_PARAM,  /**< Invalid parameter passed to SG90 function */
    DRV_SG90_NOT_INIT        /**< SG90 driver not initialized */
} drv_sg90_status_t;

/**
 * @brief SG90 driver handle structure
 */
typedef struct {
    TIM_HandleTypeDef *htim; /**< Pointer to TIM handle for PWM */
    uint32_t channel;        /**< PWM channel for the servo */
    bool is_init;            /**< Initialization status */
} sg90_handle_t;

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize SG90 servo
 *
 * @param[in] handle  Pointer to SG90 handle structure
 * @param[in] htim    Pointer to TIM handle used for PWM generation
 * @param[in] channel PWM channel for the servo
 *
 * @return
 *  - DRV_SG90_OK: Success
 *  - DRV_SG90_ERROR: Error
 *  - DRV_SG90_BUSY
 */
drv_sg90_status_t drv_sg90_init(sg90_handle_t *handle, TIM_HandleTypeDef *htim, uint32_t channel);

/**
 * @brief  Set the angle of the SG90 servo
 *
 * @param[in] handle Pointer to SG90 handle structure
 * @param[in] angle  Desired angle (0-180 degrees)
 *
 * @return
 *  - DRV_SG90_OK: Success
 *  - DRV_SG90_ERROR: Error
 *  - DRV_SG90_NOT_INIT: Driver not initialized
 */
drv_sg90_status_t drv_sg90_set_angle(sg90_handle_t *handle, uint8_t angle);

/**
 * @brief  Stop SG90 servo (disable PWM)
 *
 * @param[in] handle Pointer to SG90 handle structure
 *
 * @return
 *  - DRV_SG90_OK: Success
 *  - DRV_SG90_NOT_INIT: Driver not initialized
 */
drv_sg90_status_t drv_sg90_stop(sg90_handle_t *handle);


#endif /* __DRV_SG90_H */

/* End of file -------------------------------------------------------- */

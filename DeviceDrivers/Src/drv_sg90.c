/**
 * @file       drv_sg90.c
 * @copyright  Copyright (C) 2025. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-27
 * @author     le Ba Hien
 *
 * @brief      Source file for SG90 servo driver.
 * @note       This driver provides functions to initialize and control SG90 servo motors using PWM.
 */

/* Includes ------------------------------------------------------------------*/
#include "drv_sg90.h"

/* Public implementations ---------------------------------------------------- */
drv_sg90_status_t drv_sg90_init(sg90_handle_t *handle, TIM_HandleTypeDef *htim, uint32_t channel){
    // Validate input parameters to prevent null pointer dereference
    if (handle == NULL || htim == NULL) return DRV_SG90_INVALID_PARAM;

    // Store timer handle and channel in the SG90 handle structure
    handle->htim = htim;
    handle->channel = channel;
    handle->is_init = false;

    // Start PWM on the specified timer and channel; set init flag if successful
    if (HAL_TIM_PWM_Start(htim, channel) == HAL_OK) {
        handle->is_init = true;
        return DRV_SG90_OK;
    }

    // Return error if PWM start fails
    return DRV_SG90_ERROR;
}


drv_sg90_status_t drv_sg90_set_angle(sg90_handle_t *handle, uint8_t angle) {
    // Check if handle is valid and initialized before proceeding
    if (handle == NULL || !handle->is_init) return DRV_SG90_NOT_INIT;

    // Clamp angle to valid range to prevent out-of-bounds pulse calculation
    if (angle < SG90_MIN_ANGLE) angle = SG90_MIN_ANGLE;
    if (angle > SG90_MAX_ANGLE) angle = SG90_MAX_ANGLE;

    // Calculate PWM pulse width corresponding to the requested angle
    uint32_t pulse = SG90_MIN_PULSE +
                     ((angle * (SG90_MAX_PULSE - SG90_MIN_PULSE)) / 180);

    // Set the PWM compare value to move the servo to the desired angle
    __HAL_TIM_SET_COMPARE(handle->htim, handle->channel, pulse);

    // Return success status
    return DRV_SG90_OK;
}

drv_sg90_status_t drv_sg90_stop(sg90_handle_t *handle) {
    if (handle == NULL || !handle->is_init) {
        return DRV_SG90_NOT_INIT;
    }

    // Stop PWM on the configured channel
    if (HAL_TIM_PWM_Stop(handle->htim, handle->channel) == HAL_OK) {
        handle->is_init = false;
        return DRV_SG90_OK;
    }

    return DRV_SG90_ERROR;
}


/* End of file ---------------------------------------------------------------*/

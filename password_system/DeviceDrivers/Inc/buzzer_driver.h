/**
 * @file       buzzer_driver.h
 * @copyright  Copyright (C) 2025 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-18
 * @author     Tran Diep
 *             
 * @brief      Generic buzzer driver for STM32 microcontrollers
 *             This driver provides a generic interface for controlling
 *             passive buzzers using any STM32 timer with PWM capability
 * @note       This driver is hardware-independent and can work with any timer
 * @example    buzzer_app.c
 *             Application layer example usage
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BUZZER_DRIVER_H
#define __BUZZER_DRIVER_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "main.h"

/* Public defines ----------------------------------------------------- */
#define BUZZER_DEFAULT_TIMER_FREQ   1000000UL /*!< Default 1MHz timer frequency */
#define BUZZER_DEFAULT_MIN_VOLUME   0         /*!< Minimum volume (0%) */
#define BUZZER_DEFAULT_MAX_VOLUME   100       /*!< Maximum volume (100%) */
#define BUZZER_DEFAULT_VOLUME       20        /*!< Default volume (20%) */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Buzzer driver status enumeration
 */
typedef enum 
{
  BUZZER_STATUS_OK = 0,        /**< Operation successful */
  BUZZER_STATUS_ERROR,         /**< General error */
  BUZZER_STATUS_INVALID_PARAM, /**< Invalid parameter */
  BUZZER_STATUS_NOT_INIT,      /**< Driver not initialized */
  BUZZER_STATUS_BUSY,          /**< Buzzer is busy */
  BUZZER_STATUS_TIMEOUT        /**< Operation timeout */
}
buzzer_status_t;

/**
 * @brief Buzzer driver state enumeration
 */
typedef enum 
{
  BUZZER_STATE_IDLE = 0,   /**< Buzzer is idle */
  BUZZER_STATE_PLAYING,    /**< Buzzer is playing sound */
  BUZZER_STATE_STOPPED,    /**< Buzzer is stopped */
  BUZZER_STATE_ERROR       /**< Buzzer is in error state */
}
buzzer_state_t;

/**
 * @brief Buzzer tone structure
 */
typedef struct 
{
  uint32_t frequency; /**< Tone frequency in Hz (0 = rest/silence) */
  uint32_t duration;  /**< Tone duration in milliseconds */
  uint32_t volume;    /**< Volume level (0-100%) */
}
buzzer_tone_t;

/**
 * @brief Buzzer configuration structure
 */
typedef struct 
{
  TIM_HandleTypeDef *buzzer_timer;  /**< Timer handle for PWM generation */
  uint32_t           buzzer_channel; /**< Timer channel for PWM output */
  uint32_t           clock_source;   /**< Timer clock frequency in Hz */
  uint32_t           prescaler;      /**< Timer prescaler value */
  uint32_t           min_volume;     /**< Minimum volume (duty cycle %) */
  uint32_t           max_volume;     /**< Maximum volume (duty cycle %) */
  uint32_t           min_freq;       /**< Minimum frequency in Hz */
  uint32_t           max_freq;       /**< Maximum frequency in Hz */
}
buzzer_config_t;

/**
 * @brief Buzzer driver handle structure
 */
typedef struct 
{
  buzzer_config_t config;     /**< Buzzer configuration */
  buzzer_state_t  state;      /**< Current buzzer state */
  buzzer_status_t status;     /**< Last operation status */
  bool            is_init;    /**< Initialization flag */
}
buzzer_handle_t;

/* Public macros ------------------------------------------------------ */
/**
 * @brief  Calculate ARR value for given frequency
 *
 * @param[in]     timer_freq  Timer frequency in Hz
 * @param[in]     pwm_freq    Desired PWM frequency in Hz
 *
 * @return  ARR register value
 */
#define BUZZER_CALC_ARR(timer_freq, pwm_freq) \
  ((pwm_freq > 0) ? ((timer_freq / pwm_freq) - 1) : 0)

/**
 * @brief  Calculate CCR value for given duty cycle
 *
 * @param[in]     arr_val     ARR register value
 * @param[in]     duty_cycle  Duty cycle in percentage (0-100)
 *
 * @return  CCR register value
 */
#define BUZZER_CALC_CCR(arr_val, duty_cycle) \
  ((arr_val * duty_cycle) / 100)

/**
 * @brief  Check if frequency is in valid range
 *
 * @param[in]     freq        Frequency to check
 * @param[in]     min_freq    Minimum allowed frequency
 * @param[in]     max_freq    Maximum allowed frequency
 *
 * @return  
 *  - true: Frequency is valid
 *  - false: Frequency is invalid
 */
#define BUZZER_IS_FREQ_VALID(freq, min_freq, max_freq) \
  ((freq == 0) || ((freq >= min_freq) && (freq <= max_freq)))

/**
 * @brief  Check if volume is in valid range
 *
 * @param[in]     vol         Volume to check
 * @param[in]     min_vol     Minimum allowed volume
 * @param[in]     max_vol     Maximum allowed volume
 *
 * @return  
 *  - true: Volume is valid
 *  - false: Volume is invalid
 */
#define BUZZER_IS_VOLUME_VALID(vol, min_vol, max_vol) \
  ((vol >= min_vol) && (vol <= max_vol))

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize buzzer driver with configuration
 *
 * @param[inout]  handle      Pointer to buzzer handle
 * @param[in]     config      Pointer to buzzer configuration
 *
 * @attention  Must be called before using other buzzer functions
 *
 * @return  
 *  - BUZZER_STATUS_OK: Success
 *  - BUZZER_STATUS_INVALID_PARAM: Invalid parameter
 *  - BUZZER_STATUS_ERROR: Initialization failed
 */
buzzer_status_t buzzer_init(buzzer_handle_t *handle, buzzer_config_t *config);

/**
 * @brief  Deinitialize buzzer driver
 *
 * @param[inout]  handle      Pointer to buzzer handle
 *
 * @return  
 *  - BUZZER_STATUS_OK: Success
 *  - BUZZER_STATUS_INVALID_PARAM: Invalid parameter
 */
buzzer_status_t buzzer_deinit(buzzer_handle_t *handle);

/**
 * @brief  Start buzzer with specific tone
 *
 * @param[inout]  handle      Pointer to buzzer handle
 * @param[in]     tone        Pointer to tone structure (includes volume)
 *
 * @return  
 *  - BUZZER_STATUS_OK: Success
 *  - BUZZER_STATUS_INVALID_PARAM: Invalid parameter
 *  - BUZZER_STATUS_NOT_INIT: Driver not initialized
 *  - BUZZER_STATUS_ERROR: Hardware error
 */
buzzer_status_t buzzer_start_tone(buzzer_handle_t *handle, buzzer_tone_t *tone);

/**
 * @brief  Stop buzzer immediately
 *
 * @param[inout]  handle      Pointer to buzzer handle
 *
 * @return  
 *  - BUZZER_STATUS_OK: Success
 *  - BUZZER_STATUS_INVALID_PARAM: Invalid parameter
 *  - BUZZER_STATUS_NOT_INIT: Driver not initialized
 */
buzzer_status_t buzzer_stop(buzzer_handle_t *handle);

/**
 * @brief  Get current buzzer status
 *
 * @param[in]     handle      Pointer to buzzer handle
 *
 * @return  Current buzzer status
 */
buzzer_status_t buzzer_get_status(buzzer_handle_t *handle);

/**
 * @brief  Get current buzzer state
 *
 * @param[in]     handle      Pointer to buzzer handle
 *
 * @return  Current buzzer state
 */
buzzer_state_t buzzer_get_state(buzzer_handle_t *handle);

/**
 * @brief  Check if buzzer is initialized
 *
 * @param[in]     handle      Pointer to buzzer handle
 *
 * @return  
 *  - true: Buzzer is initialized
 *  - false: Buzzer is not initialized
 */
bool buzzer_is_initialized(buzzer_handle_t *handle);

#endif // __BUZZER_DRIVER_H

/* End of file -------------------------------------------------------- */

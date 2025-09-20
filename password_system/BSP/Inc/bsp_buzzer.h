/**
 * @file       bsp_buzzer.h
 * @copyright  Copyright (C) 2025 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-18
 * @author     Tran Diep
 *             
 * @brief      Board Support Package for buzzer on STM32F411CEU6
 *             This module provides hardware-specific configuration and
 *             APIs for buzzer control on the target board
 * @note       Configured for PA8 (TIM1_CH1) with passive buzzer
 * @example    buzzer_app.c
 *             Application layer example usage
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_BUZZER_H
#define __BSP_BUZZER_H

/* Includes ----------------------------------------------------------- */
#include "buzzer_driver.h"
#include "buzzer_notes.h"
#include "main.h"
#include <stddef.h>

/* Public defines ----------------------------------------------------- */
/**
 * @brief Hardware version selection
 * Define BSP_BUZZER_VERSION to select hardware configuration
 */
#ifndef BSP_BUZZER_VERSION
#define BSP_BUZZER_VERSION    1    /**< Default hardware version */
#endif

/* Version 1 Configuration ------------------------------------------- */
#if (BSP_BUZZER_VERSION == 1)

/* Timer Configuration */
#define BSP_BUZZER_TIMER_INSTANCE      TIM1                /**< Timer instance */
#define BSP_BUZZER_TIMER_CHANNEL       TIM_CHANNEL_1       /**< Timer channel */
#define BSP_BUZZER_TIMER_CLK_ENABLE()  __HAL_RCC_TIM1_CLK_ENABLE()   /**< Timer clock enable */
#define BSP_BUZZER_TIMER_CLK_DISABLE() __HAL_RCC_TIM1_CLK_DISABLE()  /**< Timer clock disable */

/* GPIO Configuration */
#define BSP_BUZZER_GPIO_PORT           GPIOA               /**< GPIO port */
#define BSP_BUZZER_GPIO_PIN            GPIO_PIN_8          /**< GPIO pin */
#define BSP_BUZZER_GPIO_CLK_ENABLE()   __HAL_RCC_GPIOA_CLK_ENABLE()  /**< GPIO clock enable */
#define BSP_BUZZER_GPIO_AF             GPIO_AF1_TIM1       /**< Alternate function */

/* Timer Parameters */
#define BSP_BUZZER_TIMER_PRESCALER     99                  /**< Timer prescaler (100MHz/100 = 1MHz) */
#define BSP_BUZZER_TIMER_PERIOD        999                 /**< Default timer period */
#define BSP_BUZZER_TIMER_PULSE         500                 /**< Default PWM pulse (50% duty) */
#define BSP_BUZZER_TIMER_FREQ          1000000UL           /**< Timer frequency after prescaler */

/* Audio Parameters */
#define BSP_BUZZER_MIN_FREQ            50                  /**< Minimum frequency (Hz) */
#define BSP_BUZZER_MAX_FREQ            10000               /**< Maximum frequency (Hz) */
#define BSP_BUZZER_MIN_VOLUME          0                   /**< Minimum volume (%) */
#define BSP_BUZZER_MAX_VOLUME          100                 /**< Maximum volume (%) */
#define BSP_BUZZER_DEFAULT_VOLUME      20                  /**< Default volume (%) */

#endif /* BSP_BUZZER_VERSION == 1 */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief BSP buzzer status enumeration
 */
typedef enum 
{
  BSP_BUZZER_OK = 0,           /**< Operation successful */
  BSP_BUZZER_ERROR,            /**< General error */
  BSP_BUZZER_INVALID_PARAM,    /**< Invalid parameter */
  BSP_BUZZER_NOT_INITIALIZED,  /**< BSP not initialized */
  BSP_BUZZER_HARDWARE_ERROR    /**< Hardware error */
}
bsp_buzzer_status_t;

/* Public macros ------------------------------------------------------ */
/**
 * @brief  Create a tone with frequency, duration and volume
 *
 * @param[in]     freq        Frequency in Hz
 * @param[in]     dur         Duration in milliseconds
 * @param[in]     vol         Volume level (0-100%)
 *
 * @return  buzzer_tone_t structure
 */
#define BSP_BUZZER_TONE(freq, dur, vol)  {.frequency = (freq), .duration = (dur), .volume = (vol)}

/**
 * @brief  Create a rest/silence with duration
 *
 * @param[in]     dur         Duration in milliseconds
 *
 * @return  buzzer_tone_t structure for silence
 */
#define BSP_BUZZER_REST(dur)        {.frequency = 0, .duration = (dur), .volume = 0}

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief Initialize BSP buzzer system including hardware configuration
 * 
 * This function initializes the complete buzzer system including:
 * - Timer hardware configuration
 * - GPIO pin setup  
 * - PWM channel configuration
 * - Buzzer driver initialization
 * 
 * @attention Must be called before using other BSP buzzer functions
 *
 * @return  
 *  - BSP_BUZZER_OK: Success
 *  - BSP_BUZZER_ERROR: Initialization failed
 *  - BSP_BUZZER_HARDWARE_ERROR: Hardware initialization failed
 */
bsp_buzzer_status_t bsp_buzzer_init_system(void);

/**
 * @brief  Initialize BSP buzzer with default configuration
 *
 * @attention  Must be called before using other BSP buzzer functions
 *
 * @return  
 *  - BSP_BUZZER_OK: Success
 *  - BSP_BUZZER_ERROR: Initialization failed
 *  - BSP_BUZZER_HARDWARE_ERROR: Hardware initialization failed
 */
bsp_buzzer_status_t bsp_buzzer_init(void);

/**
 * @brief  Deinitialize BSP buzzer
 *
 * @return  
 *  - BSP_BUZZER_OK: Success
 *  - BSP_BUZZER_ERROR: Deinitialization failed
 */
bsp_buzzer_status_t bsp_buzzer_deinit(void);

/**
 * @brief  Play a tone
 *
 * @param[in]     tone        Pointer to tone structure (includes frequency, duration, volume)
 *
 * @return  
 *  - BSP_BUZZER_OK: Success
 *  - BSP_BUZZER_INVALID_PARAM: Invalid parameter
 *  - BSP_BUZZER_NOT_INITIALIZED: BSP not initialized
 *  - BSP_BUZZER_HARDWARE_ERROR: Hardware error
 */
bsp_buzzer_status_t bsp_buzzer_play_tone(buzzer_tone_t *tone);

/**
 * @brief  Stop buzzer immediately
 *
 * @return  
 *  - BSP_BUZZER_OK: Success
 *  - BSP_BUZZER_NOT_INITIALIZED: BSP not initialized
 *  - BSP_BUZZER_HARDWARE_ERROR: Hardware error
 */
bsp_buzzer_status_t bsp_buzzer_stop(void);

/**
 * @brief  Get current buzzer status
 *
 * @return  Current BSP buzzer status
 */
bsp_buzzer_status_t bsp_buzzer_get_status(void);

/**
 * @brief  Check if buzzer is initialized
 *
 * @return  
 *  - true: Buzzer is initialized
 *  - false: Buzzer is not initialized
 */
bool bsp_buzzer_is_initialized(void);

/**
 * @brief  Get buzzer driver handle for advanced operations
 *
 * @return  Pointer to buzzer driver handle (NULL if not initialized)
 */
buzzer_handle_t* bsp_buzzer_get_handle(void);

#endif // __BSP_BUZZER_H

/* End of file -------------------------------------------------------- */

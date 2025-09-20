/**
 * @file       buzzer_app.h
 * @copyright  Copyright (C) 2025 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-18
 * @author     Tran Diep
 *             
 * @brief      Application layer for buzzer control
 *             This module provides high-level APIs for common buzzer
 *             sound patterns and melodies
 * @note       Uses BSP layer for hardware abstraction
 * @example    main.c
 *             Application usage example
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BUZZER_APP_H
#define __BUZZER_APP_H

/* Includes ----------------------------------------------------------- */
#include "bsp_buzzer.h"
#include "buzzer_notes.h"

/* Public defines ----------------------------------------------------- */
/* Default sound parameters */
#define APP_BUZZER_DEFAULT_SHORT_BEEP   150             /*!< Short beep duration (ms) */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Application buzzer status enumeration
 */
typedef enum 
{
  APP_BUZZER_OK = 0,              /**< Operation successful */
  APP_BUZZER_ERROR,               /**< General error */
  APP_BUZZER_INVALID_PARAM,       /**< Invalid parameter */
  APP_BUZZER_NOT_INITIALIZED,     /**< Application not initialized */
  APP_BUZZER_BSP_ERROR            /**< BSP layer error */
}
app_buzzer_status_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize buzzer application layer
 *
 * @attention  Must be called before using other application functions
 *
 * @return  
 *  - APP_BUZZER_OK: Success
 *  - APP_BUZZER_ERROR: Initialization failed
 *  - APP_BUZZER_BSP_ERROR: BSP initialization failed
 */
app_buzzer_status_t app_buzzer_init(void);

/**
 * @brief  Deinitialize buzzer application layer
 *
 * @return  
 *  - APP_BUZZER_OK: Success
 *  - APP_BUZZER_ERROR: Deinitialization failed
 */
app_buzzer_status_t app_buzzer_deinit(void);

/**
 * @brief  Play success notification sound (ascending tones)
 *
 * @param[in]     volume      Volume level (0-100%)
 *
 * @return  
 *  - APP_BUZZER_OK: Success
 *  - APP_BUZZER_NOT_INITIALIZED: Not initialized
 *  - APP_BUZZER_BSP_ERROR: BSP error
 */
app_buzzer_status_t app_buzzer_play_success(uint32_t volume);

/**
 * @brief  Play error notification sound (3 short beeps)
 *
 * @param[in]     volume      Volume level (0-100%)
 *
 * @return  
 *  - APP_BUZZER_OK: Success
 *  - APP_BUZZER_NOT_INITIALIZED: Not initialized
 *  - APP_BUZZER_BSP_ERROR: BSP error
 */
app_buzzer_status_t app_buzzer_play_error(uint32_t volume);

/**
 * @brief  Play warning sound (alternating high/low frequencies)
 *
 * @param[in]     volume      Volume level (0-100%)
 *
 * @note This function runs infinite loop until bsp_buzzer_stop() is called
 *
 * @return  
 *  - APP_BUZZER_OK: Success (never reached due to infinite loop)
 *  - APP_BUZZER_NOT_INITIALIZED: Not initialized
 *  - APP_BUZZER_BSP_ERROR: BSP error
 */
app_buzzer_status_t app_buzzer_play_warning(uint32_t volume);

#endif // __BUZZER_APP_H

/* End of file -------------------------------------------------------- */

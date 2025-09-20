/**
 * @file       buzzer_app.c
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
 */

/* Includes ----------------------------------------------------------- */
#include "buzzer_app.h"
#include <stdint.h>
#include <stdbool.h>

/* Private defines ---------------------------------------------------- */
/* No private defines needed for simplified implementation */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static bool app_initialized = false;  /**< Application initialization flag */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief  Convert BSP status to application status
 *
 * @param[in]     bsp_status  BSP status to convert
 *
 * @return  Corresponding application status
 */
static app_buzzer_status_t app_convert_bsp_status(bsp_buzzer_status_t bsp_status);

/* Function definitions ----------------------------------------------- */
app_buzzer_status_t app_buzzer_init(void)
{
  /* Check if already initialized */
  if (app_initialized)
  {
    return APP_BUZZER_OK;
  }

  /* Initialize BSP buzzer (hardware + driver) */
  bsp_buzzer_status_t bsp_status = bsp_buzzer_init();
  if (bsp_status != BSP_BUZZER_OK)
  {
    return app_convert_bsp_status(bsp_status);
  }

  /* Mark as initialized */
  app_initialized = true;

  return APP_BUZZER_OK;
}

app_buzzer_status_t app_buzzer_deinit(void)
{
  /* Check if initialized */
  if (!app_initialized)
  {
    return APP_BUZZER_NOT_INITIALIZED;
  }

  /* Deinitialize BSP layer */
  bsp_buzzer_status_t bsp_status = bsp_buzzer_deinit();
  if (bsp_status != BSP_BUZZER_OK)
  {
    return app_convert_bsp_status(bsp_status);
  }

  /* Mark as not initialized */
  app_initialized = false;

  return APP_BUZZER_OK;
}

app_buzzer_status_t app_buzzer_play_success(uint32_t volume)
{
  /* Check if initialized */
  if (!app_initialized)
  {
    return APP_BUZZER_NOT_INITIALIZED;
  }

  /* Create success sound pattern (ascending tones) with volume */
  buzzer_tone_t success_tones[] = {
    {NOTE_C4, 200, volume},
    {NOTE_E4, 200, volume}, 
    {NOTE_G4, 300, volume}
  };
  
  const uint32_t tone_count = sizeof(success_tones) / sizeof(success_tones[0]);
  
  /* Play success melody using individual tone calls */
  for (uint32_t i = 0; i < tone_count; i++)
  {
    bsp_buzzer_status_t bsp_status = bsp_buzzer_play_tone(&success_tones[i]);
    if (bsp_status != BSP_BUZZER_OK)
    {
      return app_convert_bsp_status(bsp_status);
    }
  }
  
  return APP_BUZZER_OK;
}

app_buzzer_status_t app_buzzer_play_error(uint32_t volume)
{
  /* Check if initialized */
  if (!app_initialized)
  {
    return APP_BUZZER_NOT_INITIALIZED;
  }

  /* Create error sound pattern (3 short beeps) with volume */
  buzzer_tone_t error_tones[] = {
    {NOTE_G4, APP_BUZZER_DEFAULT_SHORT_BEEP, volume},
    {NOTE_REST, 100, 0},
    {NOTE_G4, APP_BUZZER_DEFAULT_SHORT_BEEP, volume},
    {NOTE_REST, 100, 0},
    {NOTE_G4, APP_BUZZER_DEFAULT_SHORT_BEEP, volume}
  };
  
  const uint32_t tone_count = sizeof(error_tones) / sizeof(error_tones[0]);
  
  /* Play error melody using individual tone calls */
  for (uint32_t i = 0; i < tone_count; i++)
  {
    bsp_buzzer_status_t bsp_status = bsp_buzzer_play_tone(&error_tones[i]);
    if (bsp_status != BSP_BUZZER_OK)
    {
      return app_convert_bsp_status(bsp_status);
    }
  }
  
  return APP_BUZZER_OK;
}

app_buzzer_status_t app_buzzer_play_warning(uint32_t volume)
{
  /* Check if initialized */
  if (!app_initialized)
  {
    return APP_BUZZER_NOT_INITIALIZED;
  }

  /* Play warning sound (alternating high/low frequencies) - infinite loop */
  while (1)
  {
    /* High frequency tone */
    buzzer_tone_t high_tone = {FREQ_WARNING, 200, volume};
    bsp_buzzer_status_t bsp_status = bsp_buzzer_play_tone(&high_tone);
    if (bsp_status != BSP_BUZZER_OK)
    {
      return app_convert_bsp_status(bsp_status);
    }

    /* Low frequency tone */
    buzzer_tone_t low_tone = {FREQ_ALARM_LOW, 200, volume};
    bsp_status = bsp_buzzer_play_tone(&low_tone);
    if (bsp_status != BSP_BUZZER_OK)
    {
      return app_convert_bsp_status(bsp_status);
    }
  }

  /* This line will never be reached */
  return APP_BUZZER_OK;
}

/* Private definitions ----------------------------------------------- */
static app_buzzer_status_t app_convert_bsp_status(bsp_buzzer_status_t bsp_status)
{
  switch (bsp_status)
  {
    case BSP_BUZZER_OK:
      return APP_BUZZER_OK;
    
    case BSP_BUZZER_INVALID_PARAM:
      return APP_BUZZER_INVALID_PARAM;
    
    case BSP_BUZZER_NOT_INITIALIZED:
      return APP_BUZZER_NOT_INITIALIZED;
    
    case BSP_BUZZER_ERROR:
    case BSP_BUZZER_HARDWARE_ERROR:
    default:
      return APP_BUZZER_BSP_ERROR;
  }
}

/* End of file -------------------------------------------------------- */

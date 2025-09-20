/**
 * @file       buzzer_driver.c
 * @copyright  Copyright (C) 2025 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-18
 * @author     Tran Diep
 *             
 * @brief      Generic buzzer driver implementation for STM32 microcontrollers
 *             This driver provides a generic interface for controlling
 *             passive buzzers using any STM32 timer with PWM capability
 * @note       This driver is hardware-independent and can work with any timer
 */

/* Includes ----------------------------------------------------------- */
#include "buzzer_driver.h"

/* Private defines ---------------------------------------------------- */
#define BUZZER_NULL_CHECK(ptr)          ((ptr) != NULL)        /*!< Null pointer check */
#define BUZZER_RANGE_CHECK(val, min, max) ((val >= min) && (val <= max)) /*!< Range check */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */
/**
 * @brief  Validate buzzer handle
 *
 * @param[in]     handle      Pointer to buzzer handle
 *
 * @return  
 *  - true: Handle is valid
 *  - false: Handle is invalid
 */
#define BUZZER_IS_HANDLE_VALID(handle) \
  (BUZZER_NULL_CHECK(handle) && (handle)->is_init)

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief  Validate buzzer configuration
 *
 * @param[in]     config      Pointer to buzzer configuration
 *
 * @return  
 *  - BUZZER_STATUS_OK: Configuration is valid
 *  - BUZZER_STATUS_INVALID_PARAM: Invalid configuration
 */
static buzzer_status_t buzzer_validate_config(buzzer_config_t *config);

/**
 * @brief  Calculate and set PWM parameters for given frequency
 *
 * @param[in]     handle      Pointer to buzzer handle
 * @param[in]     frequency   Target frequency in Hz
 * @param[in]     volume      Volume level (0-100%)
 *
 * @return  
 *  - BUZZER_STATUS_OK: Success
 *  - BUZZER_STATUS_ERROR: Hardware error
 */
static buzzer_status_t buzzer_set_pwm_params(buzzer_handle_t *handle, uint32_t frequency, uint32_t volume);

/**
 * @brief  Start PWM output
 *
 * @param[in]     handle      Pointer to buzzer handle
 *
 * @return  
 *  - BUZZER_STATUS_OK: Success
 *  - BUZZER_STATUS_ERROR: Hardware error
 */
static buzzer_status_t buzzer_start_pwm(buzzer_handle_t *handle);

/**
 * @brief  Stop PWM output
 *
 * @param[in]     handle      Pointer to buzzer handle
 *
 * @return  
 *  - BUZZER_STATUS_OK: Success
 *  - BUZZER_STATUS_ERROR: Hardware error
 */
static buzzer_status_t buzzer_stop_pwm(buzzer_handle_t *handle);

/* Function definitions ----------------------------------------------- */
buzzer_status_t buzzer_init(buzzer_handle_t *handle, buzzer_config_t *config)
{
  /* Validate input parameters */
  if (!BUZZER_NULL_CHECK(handle) || !BUZZER_NULL_CHECK(config))
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Validate configuration */
  buzzer_status_t config_status = buzzer_validate_config(config);
  if (config_status != BUZZER_STATUS_OK)
  {
    return config_status;
  }

  /* Copy configuration to handle */
  handle->config = *config;

  /* Initialize handle state */
  handle->state = BUZZER_STATE_IDLE;
  handle->status = BUZZER_STATUS_OK;
  handle->is_init = true;

  /* Stop PWM to ensure clean initialization */
  buzzer_stop_pwm(handle);

  return BUZZER_STATUS_OK;
}

buzzer_status_t buzzer_deinit(buzzer_handle_t *handle)
{
  /* Validate handle */
  if (!BUZZER_NULL_CHECK(handle))
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Stop buzzer if playing */
  if (handle->is_init)
  {
    buzzer_stop(handle);
  }

  /* Reset handle */
  handle->is_init = false;
  handle->state = BUZZER_STATE_IDLE;
  handle->status = BUZZER_STATUS_OK;

  return BUZZER_STATUS_OK;
}

buzzer_status_t buzzer_start_tone(buzzer_handle_t *handle, buzzer_tone_t *tone)
{
  /* Validate input parameters */
  if (!BUZZER_IS_HANDLE_VALID(handle) || !BUZZER_NULL_CHECK(tone))
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Validate volume range */
  if (!BUZZER_IS_VOLUME_VALID(tone->volume, handle->config.min_volume, handle->config.max_volume))
  {
    handle->status = BUZZER_STATUS_INVALID_PARAM;
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Validate frequency range (0 is valid for silence) */
  if (!BUZZER_IS_FREQ_VALID(tone->frequency, handle->config.min_freq, handle->config.max_freq))
  {
    handle->status = BUZZER_STATUS_INVALID_PARAM;
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Handle silence/rest */
  if (tone->frequency == 0)
  {
    buzzer_stop_pwm(handle);
    handle->state = BUZZER_STATE_STOPPED;
    handle->status = BUZZER_STATUS_OK;
    return BUZZER_STATUS_OK;
  }

  /* Set PWM parameters */
  buzzer_status_t pwm_status = buzzer_set_pwm_params(handle, tone->frequency, tone->volume);
  if (pwm_status != BUZZER_STATUS_OK)
  {
    handle->status = pwm_status;
    handle->state = BUZZER_STATE_ERROR;
    return pwm_status;
  }

  /* Start PWM output */
  pwm_status = buzzer_start_pwm(handle);
  if (pwm_status != BUZZER_STATUS_OK)
  {
    handle->status = pwm_status;
    handle->state = BUZZER_STATE_ERROR;
    return pwm_status;
  }

  /* Update handle state */
  handle->state = BUZZER_STATE_PLAYING;
  handle->status = BUZZER_STATUS_OK;

  return BUZZER_STATUS_OK;
}

buzzer_status_t buzzer_stop(buzzer_handle_t *handle)
{
  /* Validate handle */
  if (!BUZZER_IS_HANDLE_VALID(handle))
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Stop PWM output */
  buzzer_status_t pwm_status = buzzer_stop_pwm(handle);
  if (pwm_status != BUZZER_STATUS_OK)
  {
    handle->status = pwm_status;
    handle->state = BUZZER_STATE_ERROR;
    return pwm_status;
  }

  /* Update handle state */
  handle->state = BUZZER_STATE_STOPPED;
  handle->status = BUZZER_STATUS_OK;

  return BUZZER_STATUS_OK;
}

buzzer_status_t buzzer_get_status(buzzer_handle_t *handle)
{
  if (!BUZZER_NULL_CHECK(handle))
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  return handle->status;
}

buzzer_state_t buzzer_get_state(buzzer_handle_t *handle)
{
  if (!BUZZER_NULL_CHECK(handle))
  {
    return BUZZER_STATE_ERROR;
  }

  return handle->state;
}

bool buzzer_is_initialized(buzzer_handle_t *handle)
{
  if (!BUZZER_NULL_CHECK(handle))
  {
    return false;
  }

  return handle->is_init;
}

/* Private definitions ----------------------------------------------- */
static buzzer_status_t buzzer_validate_config(buzzer_config_t *config)
{
  /* Validate timer handle */
  if (!BUZZER_NULL_CHECK(config->buzzer_timer))
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Validate frequency range */
  if (config->min_freq >= config->max_freq)
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Validate volume range */
  if (config->min_volume >= config->max_volume || config->max_volume > 100)
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  /* Validate clock source */
  if (config->clock_source == 0)
  {
    return BUZZER_STATUS_INVALID_PARAM;
  }

  return BUZZER_STATUS_OK;
}

static buzzer_status_t buzzer_set_pwm_params(buzzer_handle_t *handle, uint32_t frequency, uint32_t volume)
{
  /* Calculate ARR value for target frequency */
  uint32_t timer_freq = handle->config.clock_source / (handle->config.prescaler + 1);
  uint32_t arr_value = BUZZER_CALC_ARR(timer_freq, frequency);
  
  /* Calculate CCR value for desired volume (duty cycle) */
  uint32_t ccr_value = BUZZER_CALC_CCR(arr_value, volume);

  /* Set timer ARR register */
  __HAL_TIM_SET_AUTORELOAD(handle->config.buzzer_timer, arr_value);

  /* Set timer CCR register */
  __HAL_TIM_SET_COMPARE(handle->config.buzzer_timer, handle->config.buzzer_channel, ccr_value);

  return BUZZER_STATUS_OK;
}

static buzzer_status_t buzzer_start_pwm(buzzer_handle_t *handle)
{
  /* Start PWM output */
  HAL_StatusTypeDef hal_status = HAL_TIM_PWM_Start(handle->config.buzzer_timer, handle->config.buzzer_channel);
  
  if (hal_status != HAL_OK)
  {
    return BUZZER_STATUS_ERROR;
  }

  return BUZZER_STATUS_OK;
}

static buzzer_status_t buzzer_stop_pwm(buzzer_handle_t *handle)
{
  /* Stop PWM output */
  HAL_StatusTypeDef hal_status = HAL_TIM_PWM_Stop(handle->config.buzzer_timer, handle->config.buzzer_channel);
  
  /* Set CCR to 0 for complete silence */
  __HAL_TIM_SET_COMPARE(handle->config.buzzer_timer, handle->config.buzzer_channel, 0);
  
  if (hal_status != HAL_OK)
  {
    return BUZZER_STATUS_ERROR;
  }

  return BUZZER_STATUS_OK;
}

/* End of file -------------------------------------------------------- */

/**
 * @file       bsp_buzzer.c
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
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_buzzer.h"
#include "stm32f4xx_hal.h"

/* Private defines ---------------------------------------------------- */
#define BSP_BUZZER_DEFAULT_GAP_MS   50  /*!< Default gap between tones in milliseconds */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static buzzer_handle_t buzzer_handle;      /**< Buzzer driver handle */
static buzzer_config_t buzzer_config;      /**< Buzzer configuration */
static bool            bsp_initialized = false; /**< BSP initialization flag */
static TIM_HandleTypeDef htim_buzzer;      /**< Timer handle for buzzer PWM */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief  Hardware initialization 
 *
 * @return  
 *  - BSP_BUZZER_OK: Success
 *  - BSP_BUZZER_HARDWARE_ERROR: Hardware initialization failed
 */
static bsp_buzzer_status_t bsp_buzzer_hw_init(void);

/**
 * @brief  Hardware deinitialization
 *
 * @return  
 *  - BSP_BUZZER_OK: Success
 *  - BSP_BUZZER_HARDWARE_ERROR: Hardware deinitialization failed
 */
static bsp_buzzer_status_t bsp_buzzer_hw_deinit(void);

/**
 * @brief  Convert BSP status to driver status for error handling
 *
 * @param[in]     driver_status   Driver status to convert
 *
 * @return  Corresponding BSP status
 */
static bsp_buzzer_status_t bsp_convert_driver_status(buzzer_status_t driver_status);

/**
 * @brief  Initialize default buzzer configuration
 *
 * @param[out]    config      Pointer to configuration structure to initialize
 *
 * @return  None
 */
static void bsp_buzzer_init_default_config(buzzer_config_t *config);

/* Function definitions ----------------------------------------------- */
bsp_buzzer_status_t bsp_buzzer_init(void)
{
  bsp_buzzer_status_t status;
  
  /* Check if already initialized */
  if (bsp_initialized)
  {
    return BSP_BUZZER_OK;
  }
  
  /* Initialize hardware first */
  status = bsp_buzzer_init_system();
  if (status != BSP_BUZZER_OK)
  {
    return status;
  }
  
  /* Initialize default configuration */
  bsp_buzzer_init_default_config(&buzzer_config);
  
  /* Initialize buzzer driver */
  buzzer_status_t driver_status = buzzer_init(&buzzer_handle, &buzzer_config);
  if (driver_status != BUZZER_STATUS_OK)
  {
    return bsp_convert_driver_status(driver_status);
  }
  
  bsp_initialized = true;
  return BSP_BUZZER_OK;
}

bsp_buzzer_status_t bsp_buzzer_init_system(void)
{
  /* Initialize hardware using defines from header */
  return bsp_buzzer_hw_init();
}

bsp_buzzer_status_t bsp_buzzer_deinit(void)
{
  /* Check if initialized */
  if (!bsp_initialized)
  {
    return BSP_BUZZER_NOT_INITIALIZED;
  }

  /* Deinitialize buzzer driver */
  buzzer_status_t driver_status = buzzer_deinit(&buzzer_handle);
  if (driver_status != BUZZER_STATUS_OK)
  {
    return bsp_convert_driver_status(driver_status);
  }

  /* Mark as not initialized */
  bsp_initialized = false;

  return BSP_BUZZER_OK;
}

bsp_buzzer_status_t bsp_buzzer_play_tone(buzzer_tone_t *tone)
{
  /* Check if initialized */
  if (!bsp_initialized)
  {
    return BSP_BUZZER_NOT_INITIALIZED;
  }

  /* Validate input */
  if (tone == NULL)
  {
    return BSP_BUZZER_INVALID_PARAM;
  }

  /* Play tone using driver */
  buzzer_status_t driver_status = buzzer_start_tone(&buzzer_handle, tone);
  if (driver_status != BUZZER_STATUS_OK)
  {
    return bsp_convert_driver_status(driver_status);
  }

  /* Wait for tone duration if specified */
  if (tone->duration > 0)
  {
    HAL_Delay(tone->duration);
    buzzer_stop(&buzzer_handle);
  }

  return BSP_BUZZER_OK;
}

bsp_buzzer_status_t bsp_buzzer_stop(void)
{
  /* Check if initialized */
  if (!bsp_initialized)
  {
    return BSP_BUZZER_NOT_INITIALIZED;
  }

  /* Stop buzzer using driver */
  buzzer_status_t driver_status = buzzer_stop(&buzzer_handle);
  if (driver_status != BUZZER_STATUS_OK)
  {
    return bsp_convert_driver_status(driver_status);
  }

  return BSP_BUZZER_OK;
}

bsp_buzzer_status_t bsp_buzzer_get_status(void)
{
  if (!bsp_initialized)
  {
    return BSP_BUZZER_NOT_INITIALIZED;
  }

  buzzer_status_t driver_status = buzzer_get_status(&buzzer_handle);
  return bsp_convert_driver_status(driver_status);
}

bool bsp_buzzer_is_initialized(void)
{
  return bsp_initialized;
}

buzzer_handle_t* bsp_buzzer_get_handle(void)
{
  if (!bsp_initialized)
  {
    return NULL;
  }

  return &buzzer_handle;
}

/* Private definitions ----------------------------------------------- */
static bsp_buzzer_status_t bsp_convert_driver_status(buzzer_status_t driver_status)
{
  switch (driver_status)
  {
    case BUZZER_STATUS_OK:
      return BSP_BUZZER_OK;
    
    case BUZZER_STATUS_INVALID_PARAM:
      return BSP_BUZZER_INVALID_PARAM;
    
    case BUZZER_STATUS_NOT_INIT:
      return BSP_BUZZER_NOT_INITIALIZED;
    
    case BUZZER_STATUS_ERROR:
    case BUZZER_STATUS_BUSY:
    case BUZZER_STATUS_TIMEOUT:
    default:
      return BSP_BUZZER_HARDWARE_ERROR;
  }
}

static void bsp_buzzer_init_default_config(buzzer_config_t *config)
{
  config->buzzer_timer = &htim_buzzer;
  config->buzzer_channel = BSP_BUZZER_TIMER_CHANNEL;
  config->clock_source = 100000000UL;  /* 100MHz system clock */
  config->prescaler = BSP_BUZZER_TIMER_PRESCALER;
  config->min_volume = BSP_BUZZER_MIN_VOLUME;
  config->max_volume = BSP_BUZZER_MAX_VOLUME;
  config->min_freq = BSP_BUZZER_MIN_FREQ;
  config->max_freq = BSP_BUZZER_MAX_FREQ;
}

/* Hardware-specific implementations --------------------------------- */
static bsp_buzzer_status_t bsp_buzzer_hw_init(void)
{
  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  TIM_BreakDeadTimeConfigTypeDef sBreakDeadTimeConfig = {0};
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  
  /* Check if required defines are available */
#ifndef BSP_BUZZER_TIMER_INSTANCE
  #error "BSP_BUZZER_TIMER_INSTANCE not defined in header"
#endif
#ifndef BSP_BUZZER_GPIO_PORT
  #error "BSP_BUZZER_GPIO_PORT not defined in header"
#endif
#ifndef BSP_BUZZER_GPIO_PIN
  #error "BSP_BUZZER_GPIO_PIN not defined in header"
#endif
  
  /* Enable clocks */
  BSP_BUZZER_TIMER_CLK_ENABLE();
  BSP_BUZZER_GPIO_CLK_ENABLE();
  
  /* Configure GPIO pin for TIM1_CH1 (PA8) */
  GPIO_InitStruct.Pin = BSP_BUZZER_GPIO_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  GPIO_InitStruct.Alternate = BSP_BUZZER_GPIO_AF;
  HAL_GPIO_Init(BSP_BUZZER_GPIO_PORT, &GPIO_InitStruct);
  
  /* Configure TIM1 */
  htim_buzzer.Instance = BSP_BUZZER_TIMER_INSTANCE;
  htim_buzzer.Init.Prescaler = BSP_BUZZER_TIMER_PRESCALER;
  htim_buzzer.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim_buzzer.Init.Period = BSP_BUZZER_TIMER_PERIOD;
  htim_buzzer.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim_buzzer.Init.RepetitionCounter = 0;
  htim_buzzer.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  
  if (HAL_TIM_Base_Init(&htim_buzzer) != HAL_OK)
  {
    return BSP_BUZZER_HARDWARE_ERROR;
  }
  
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim_buzzer, &sClockSourceConfig) != HAL_OK)
  {
    return BSP_BUZZER_HARDWARE_ERROR;
  }
  
  if (HAL_TIM_PWM_Init(&htim_buzzer) != HAL_OK)
  {
    return BSP_BUZZER_HARDWARE_ERROR;
  }
  
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim_buzzer, &sMasterConfig) != HAL_OK)
  {
    return BSP_BUZZER_HARDWARE_ERROR;
  }
  
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = BSP_BUZZER_TIMER_PULSE;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCNPolarity = TIM_OCNPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(&htim_buzzer, &sConfigOC, BSP_BUZZER_TIMER_CHANNEL) != HAL_OK)
  {
    return BSP_BUZZER_HARDWARE_ERROR;
  }
  
  sBreakDeadTimeConfig.OffStateRunMode = TIM_OSSR_DISABLE;
  sBreakDeadTimeConfig.OffStateIDLEMode = TIM_OSSI_DISABLE;
  sBreakDeadTimeConfig.LockLevel = TIM_LOCKLEVEL_OFF;
  sBreakDeadTimeConfig.DeadTime = 0;
  sBreakDeadTimeConfig.BreakState = TIM_BREAK_DISABLE;
  sBreakDeadTimeConfig.BreakPolarity = TIM_BREAKPOLARITY_HIGH;
  sBreakDeadTimeConfig.AutomaticOutput = TIM_AUTOMATICOUTPUT_DISABLE;
  if (HAL_TIMEx_ConfigBreakDeadTime(&htim_buzzer, &sBreakDeadTimeConfig) != HAL_OK)
  {
    return BSP_BUZZER_HARDWARE_ERROR;
  }
  
  return BSP_BUZZER_OK;
}

static bsp_buzzer_status_t bsp_buzzer_hw_deinit(void)
{
  /* Deinitialize timer */
  if (HAL_TIM_Base_DeInit(&htim_buzzer) != HAL_OK)
  {
    return BSP_BUZZER_HARDWARE_ERROR;
  }
  
  /* Deinitialize GPIO */
  HAL_GPIO_DeInit(BSP_BUZZER_GPIO_PORT, BSP_BUZZER_GPIO_PIN);
  
  /* Disable clocks */
  BSP_BUZZER_TIMER_CLK_DISABLE();
  
  return BSP_BUZZER_OK;
}

/* End of file -------------------------------------------------------- */

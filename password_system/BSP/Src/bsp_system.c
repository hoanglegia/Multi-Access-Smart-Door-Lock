/**
 * @file       bsp_system.c
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-19
 * @author     Tran Van Diep
 *             
 * @brief      BSP System Implementation
 *             Board Support Package for system-level initialization and configuration
 * @note       Provides version-controlled system initialization with clock and GPIO management
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_system.h"
#include <string.h>
#include <stdio.h>

/* Private defines ---------------------------------------------------- */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief  Validate hardware version
 *
 * @param[in]  version  Hardware version to validate
 *
 * @return  
 *  - BSP_SYSTEM_OK: Version is valid
 *  - BSP_SYSTEM_VERSION_ERROR: Version is not supported
 */
static bsp_system_status_t bsp_system_validate_version(uint8_t version);

/**
 * @brief  Configure power settings according to hardware version
 *
 * @param[in]  config  Pointer to BSP system configuration structure
 *
 * @return  
 *  - BSP_SYSTEM_OK: Power configuration successful
 *  - BSP_SYSTEM_ERROR: Power configuration failed
 */
static bsp_system_status_t bsp_system_power_config(bsp_system_config_t *config);

/* Function definitions ----------------------------------------------- */
bsp_system_status_t bsp_system_init(bsp_system_config_t *config)
{
  bsp_system_status_t status;
  
  if (config == NULL)
  {
    return BSP_SYSTEM_ERROR;
  }
  
  /* Initialize configuration structure */
  config->version = BSP_SYSTEM_VERSION;
  config->sysclk_freq = BSP_SYSTEM_SYSCLK_FREQ;
  config->hclk_freq = BSP_SYSTEM_HCLK_FREQ;
  config->pclk1_freq = BSP_SYSTEM_PCLK1_FREQ;
  config->pclk2_freq = BSP_SYSTEM_PCLK2_FREQ;
  config->initialized = 0;
  config->mcu_type = BSP_SYSTEM_MCU_TYPE;
  config->board_type = BSP_SYSTEM_BOARD_TYPE;
  
  /* Validate hardware version */
  status = bsp_system_validate_version(config->version);
  if (status != BSP_SYSTEM_OK)
  {
    return status;
  }
  
  /* Configure power settings */
  status = bsp_system_power_config(config);
  if (status != BSP_SYSTEM_OK)
  {
    return status;
  }
  
  /* Configure system clock */
  status = bsp_system_clock_config(config);
  if (status != BSP_SYSTEM_OK)
  {
    return status;
  }
  
  /* Enable GPIO ports clocks */
  status = bsp_system_gpio_clocks_enable(config);
  if (status != BSP_SYSTEM_OK)
  {
    return status;
  }
  
  config->initialized = 1;
  
  return BSP_SYSTEM_OK;
}

bsp_system_status_t bsp_system_clock_config(bsp_system_config_t *config)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  HAL_StatusTypeDef hal_status;
  
  if (config == NULL)
  {
    return BSP_SYSTEM_ERROR;
  }
  
  /* Configure oscillator based on hardware version */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  
#if (BSP_SYSTEM_VERSION == 1)
  /* STM32F411CEU6 BlackPill - 25MHz HSE */
  RCC_OscInitStruct.PLL.PLLM = BSP_SYSTEM_PLL_M;
  RCC_OscInitStruct.PLL.PLLN = BSP_SYSTEM_PLL_N;
  RCC_OscInitStruct.PLL.PLLP = BSP_SYSTEM_PLL_P;
  RCC_OscInitStruct.PLL.PLLQ = BSP_SYSTEM_PLL_Q;
  
#elif (BSP_SYSTEM_VERSION == 2)
  /* STM32F103C8T6 BluePill - 8MHz HSE */
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLMUL = BSP_SYSTEM_PLL_MUL;
  
#elif (BSP_SYSTEM_VERSION == 3)
  /* STM32F407VGT6 Discovery - 8MHz HSE */
  RCC_OscInitStruct.PLL.PLLM = BSP_SYSTEM_PLL_M;
  RCC_OscInitStruct.PLL.PLLN = BSP_SYSTEM_PLL_N;
  RCC_OscInitStruct.PLL.PLLP = BSP_SYSTEM_PLL_P;
  RCC_OscInitStruct.PLL.PLLQ = BSP_SYSTEM_PLL_Q;
#endif
  
  hal_status = HAL_RCC_OscConfig(&RCC_OscInitStruct);
  if (hal_status != HAL_OK)
  {
    return BSP_SYSTEM_CLOCK_ERROR;
  }
  
  /* Configure system and bus clocks */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
                              | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  
#if (BSP_SYSTEM_VERSION == 1)
  /* STM32F411CEU6 BlackPill */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  
#elif (BSP_SYSTEM_VERSION == 2)
  /* STM32F103C8T6 BluePill */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;
  
#elif (BSP_SYSTEM_VERSION == 3)
  /* STM32F407VGT6 Discovery */
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;
#endif
  
  hal_status = HAL_RCC_ClockConfig(&RCC_ClkInitStruct, BSP_SYSTEM_FLASH_LATENCY);
  if (hal_status != HAL_OK)
  {
    return BSP_SYSTEM_CLOCK_ERROR;
  }
  
  return BSP_SYSTEM_OK;
}

bsp_system_status_t bsp_system_gpio_clocks_enable(bsp_system_config_t *config)
{
  if (config == NULL)
  {
    return BSP_SYSTEM_ERROR;
  }
  
  /* Enable GPIO ports clocks based on hardware version */
#if (BSP_SYSTEM_VERSION == 1) || (BSP_SYSTEM_VERSION == 3)
  /* STM32F4xx - Enable GPIOA, GPIOB, GPIOC */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  
#elif (BSP_SYSTEM_VERSION == 2)
  /* STM32F1xx - Enable GPIOA, GPIOB, GPIOC */
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
#endif
  
  return BSP_SYSTEM_OK;
}

bsp_system_status_t bsp_system_get_clocks(bsp_system_config_t *config,
                                          uint32_t *sysclk, 
                                          uint32_t *hclk, 
                                          uint32_t *pclk1, 
                                          uint32_t *pclk2)
{
  if (config == NULL || !config->initialized)
  {
    return BSP_SYSTEM_ERROR;
  }
  
  if (sysclk != NULL) *sysclk = config->sysclk_freq;
  if (hclk != NULL)   *hclk = config->hclk_freq;
  if (pclk1 != NULL)  *pclk1 = config->pclk1_freq;
  if (pclk2 != NULL)  *pclk2 = config->pclk2_freq;
  
  return BSP_SYSTEM_OK;
}

uint8_t bsp_system_get_version(bsp_system_config_t *config)
{
  if (config == NULL)
  {
    return 0;
  }
  
  return config->version;
}

const char* bsp_system_get_mcu_type(bsp_system_config_t *config)
{
  if (config == NULL || !config->initialized)
  {
    return NULL;
  }
  
  return config->mcu_type;
}

const char* bsp_system_get_board_type(bsp_system_config_t *config)
{
  if (config == NULL || !config->initialized)
  {
    return NULL;
  }
  
  return config->board_type;
}

void bsp_system_reset(void)
{
  HAL_NVIC_SystemReset();
}

bsp_system_status_t bsp_system_enter_low_power(bsp_system_config_t *config)
{
  if (config == NULL || !config->initialized)
  {
    return BSP_SYSTEM_ERROR;
  }
  
  /* Configure system for low power mode */
  /* Disable unnecessary clocks */
  /* Configure wake-up sources */
  /* Enter sleep/stop mode */
  
  /* Implementation depends on specific low power requirements */
  HAL_PWR_EnterSLEEPMode(PWR_MAINREGULATOR_ON, PWR_SLEEPENTRY_WFI);
  
  return BSP_SYSTEM_OK;
}

/* Private function implementations ----------------------------------- */
static bsp_system_status_t bsp_system_validate_version(uint8_t version)
{
  switch (version)
  {
    case 1:  /* STM32F411CEU6 BlackPill */
    case 2:  /* STM32F103C8T6 BluePill */
    case 3:  /* STM32F407VGT6 Discovery */
      return BSP_SYSTEM_OK;
      
    default:
      return BSP_SYSTEM_VERSION_ERROR;
  }
}

static bsp_system_status_t bsp_system_power_config(bsp_system_config_t *config)
{
  if (config == NULL)
  {
    return BSP_SYSTEM_ERROR;
  }
  
  /* Configure the main internal regulator output voltage */
  __HAL_RCC_PWR_CLK_ENABLE();
  
#if (BSP_SYSTEM_VERSION == 1) || (BSP_SYSTEM_VERSION == 3)
  /* STM32F4xx */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  
#elif (BSP_SYSTEM_VERSION == 2)
  /* STM32F1xx - No voltage scaling needed */
#endif
  
  return BSP_SYSTEM_OK;
}

/* End of file -------------------------------------------------------- */

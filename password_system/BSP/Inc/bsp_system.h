/**
 * @file       bsp_system.h
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-19
 * @author     Tran Van Diep
 *             
 * @brief      BSP System - Version Control & Hardware System Configuration
 *             Board Support Package for system-level initialization and configuration
 * @note       Supports multiple hardware versions through conditional compilation
 *             Manages GPIO clocks, system clock, and common peripherals
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_SYSTEM_H
#define __BSP_SYSTEM_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>

/* Public defines ----------------------------------------------------- */
/**
 * @brief Hardware version control
 * Define BSP_SYSTEM_VERSION in main.h or build system to select hardware configuration
 */
#ifndef BSP_SYSTEM_VERSION
  #define BSP_SYSTEM_VERSION 1
#endif

/* Hardware version specific includes */
#if (BSP_SYSTEM_VERSION == 1) || (BSP_SYSTEM_VERSION == 3)
  #include "stm32f4xx_hal.h"
#elif (BSP_SYSTEM_VERSION == 2)
  #include "stm32f1xx_hal.h"
#else
  #error "Unsupported BSP_SYSTEM_VERSION. Supported versions: 1 (STM32F411CEU6), 2 (STM32F103C8T6)"
#endif

/* Hardware Configuration Version 1 - STM32F411CEU6 BlackPill */
#if (BSP_SYSTEM_VERSION == 1)
  #define BSP_SYSTEM_MCU_TYPE           "STM32F411CEU6"
  #define BSP_SYSTEM_BOARD_TYPE         "BlackPill"
  #define BSP_SYSTEM_HSE_VALUE          25000000U   /*!< External High Speed oscillator (HSE) value */
  #define BSP_SYSTEM_SYSCLK_FREQ        100000000U  /*!< System Clock frequency */
  #define BSP_SYSTEM_HCLK_FREQ          100000000U  /*!< AHB Clock frequency */
  #define BSP_SYSTEM_PCLK1_FREQ         50000000U   /*!< APB1 Clock frequency */
  #define BSP_SYSTEM_PCLK2_FREQ         100000000U  /*!< APB2 Clock frequency */
  
  /* PLL Configuration */
  #define BSP_SYSTEM_PLL_M              12          /*!< PLL M divider */
  #define BSP_SYSTEM_PLL_N              96          /*!< PLL N multiplier */
  #define BSP_SYSTEM_PLL_P              RCC_PLLP_DIV2  /*!< PLL P divider */
  #define BSP_SYSTEM_PLL_Q              4           /*!< PLL Q divider */
  
  /* Flash Latency */
  #define BSP_SYSTEM_FLASH_LATENCY      FLASH_LATENCY_3
  
  /* GPIO Ports enabled by default */
  #define BSP_SYSTEM_GPIO_PORTS_ENABLED (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN)
#endif

/* Hardware Configuration Version 2 - STM32F103C8T6 Blue Pill */
#if (BSP_SYSTEM_VERSION == 2)
  #define BSP_SYSTEM_MCU_TYPE           "STM32F103C8T6"
  #define BSP_SYSTEM_BOARD_TYPE         "BluePill"
  #define BSP_SYSTEM_HSE_VALUE          8000000U    /*!< External High Speed oscillator (HSE) value */
  #define BSP_SYSTEM_SYSCLK_FREQ        72000000U   /*!< System Clock frequency */
  #define BSP_SYSTEM_HCLK_FREQ          72000000U   /*!< AHB Clock frequency */
  #define BSP_SYSTEM_PCLK1_FREQ         36000000U   /*!< APB1 Clock frequency */
  #define BSP_SYSTEM_PCLK2_FREQ         72000000U   /*!< APB2 Clock frequency */
  
  /* PLL Configuration for F103 */
  #define BSP_SYSTEM_PLL_MUL            RCC_PLL_MUL9  /*!< PLL multiplier (8MHz * 9 = 72MHz) */
  
  /* Flash Latency for F103 */
  #define BSP_SYSTEM_FLASH_LATENCY      FLASH_LATENCY_2
  
  /* GPIO Ports enabled by default for F103 */
  #define BSP_SYSTEM_GPIO_PORTS_ENABLED (RCC_APB2ENR_IOPAEN | RCC_APB2ENR_IOPBEN | RCC_APB2ENR_IOPCEN)
#endif

/* Hardware Configuration Version 3 - STM32F4 Discovery Board */
#if (BSP_SYSTEM_VERSION == 3)
  #define BSP_SYSTEM_MCU_TYPE           "STM32F407VGT6"
  #define BSP_SYSTEM_BOARD_TYPE         "Discovery"
  #define BSP_SYSTEM_HSE_VALUE          8000000U    /*!< External High Speed oscillator (HSE) value */
  #define BSP_SYSTEM_SYSCLK_FREQ        168000000U  /*!< System Clock frequency */
  #define BSP_SYSTEM_HCLK_FREQ          168000000U  /*!< AHB Clock frequency */
  #define BSP_SYSTEM_PCLK1_FREQ         42000000U   /*!< APB1 Clock frequency */
  #define BSP_SYSTEM_PCLK2_FREQ         84000000U   /*!< APB2 Clock frequency */
  
  /* PLL Configuration */
  #define BSP_SYSTEM_PLL_M              4           /*!< PLL M divider */
  #define BSP_SYSTEM_PLL_N              168         /*!< PLL N multiplier */
  #define BSP_SYSTEM_PLL_P              RCC_PLLP_DIV2  /*!< PLL P divider */
  #define BSP_SYSTEM_PLL_Q              7           /*!< PLL Q divider */
  
  /* Flash Latency */
  #define BSP_SYSTEM_FLASH_LATENCY      FLASH_LATENCY_5
  
  /* GPIO Ports enabled by default */
  #define BSP_SYSTEM_GPIO_PORTS_ENABLED (RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN | RCC_AHB1ENR_GPIOCEN | RCC_AHB1ENR_GPIODEN | RCC_AHB1ENR_GPIOEEN)
#endif

/* System Configuration */
#define BSP_SYSTEM_TIMEOUT_MS         (5000U)      /*!< System initialization timeout */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief BSP System return status enumeration
 */
typedef enum 
{
  BSP_SYSTEM_OK,              /**< Operation successful */
  BSP_SYSTEM_ERROR,           /**< General error */
  BSP_SYSTEM_TIMEOUT,         /**< Initialization timeout */
  BSP_SYSTEM_CLOCK_ERROR,     /**< Clock configuration error */
  BSP_SYSTEM_VERSION_ERROR    /**< Unsupported version */
}
bsp_system_status_t;

/**
 * @brief BSP System configuration structure
 */
typedef struct
{
  uint8_t   version;              /**< Hardware version */
  uint32_t  sysclk_freq;          /**< System clock frequency */
  uint32_t  hclk_freq;            /**< AHB clock frequency */
  uint32_t  pclk1_freq;           /**< APB1 clock frequency */
  uint32_t  pclk2_freq;           /**< APB2 clock frequency */
  uint8_t   initialized;          /**< Initialization flag */
  const char *mcu_type;           /**< MCU type string */
  const char *board_type;         /**< Board type string */
}
bsp_system_config_t;

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */
/**
 * @brief  Initialize BSP System
 *         This function performs complete system initialization including:
 *         - Version detection and validation
 *         - System clock configuration
 *         - GPIO ports clock enable
 *         - Flash latency setting
 *         - Power configuration
 *
 * @param[out] config  Pointer to BSP system configuration structure to initialize
 *
 * @return  
 *  - BSP_SYSTEM_OK: System initialization successful
 *  - BSP_SYSTEM_ERROR: Invalid parameters
 *  - BSP_SYSTEM_CLOCK_ERROR: Clock configuration failed
 *  - BSP_SYSTEM_VERSION_ERROR: Unsupported hardware version
 * 
 * @note   This function should be called first in main() after HAL_Init()
 *         Replaces SystemClock_Config() and GPIO clock enables
 */
bsp_system_status_t bsp_system_init(bsp_system_config_t *config);

/**
 * @brief  Configure system clock according to hardware version
 *         This function configures PLL, system clock, and bus clocks
 *
 * @param[in]  config  Pointer to BSP system configuration structure
 *
 * @return  
 *  - BSP_SYSTEM_OK: Clock configuration successful
 *  - BSP_SYSTEM_ERROR: Invalid parameters
 *  - BSP_SYSTEM_CLOCK_ERROR: Clock configuration failed
 *
 * @note   This function is automatically called by bsp_system_init()
 *         Can be called separately for clock re-configuration
 */
bsp_system_status_t bsp_system_clock_config(bsp_system_config_t *config);

/**
 * @brief  Enable GPIO ports clocks according to hardware version
 *         This function enables clocks for all GPIO ports used by the system
 *
 * @param[in]  config  Pointer to BSP system configuration structure
 *
 * @return  
 *  - BSP_SYSTEM_OK: GPIO clocks enabled successfully
 *  - BSP_SYSTEM_ERROR: Invalid parameters
 *
 * @note   This function is automatically called by bsp_system_init()
 *         Can be called separately to enable additional GPIO ports
 */
bsp_system_status_t bsp_system_gpio_clocks_enable(bsp_system_config_t *config);

/**
 * @brief  Get system clock frequencies
 *         This function retrieves current system clock frequencies
 *
 * @param[in]   config     Pointer to BSP system configuration structure
 * @param[out]  sysclk     System clock frequency in Hz
 * @param[out]  hclk       AHB clock frequency in Hz
 * @param[out]  pclk1      APB1 clock frequency in Hz
 * @param[out]  pclk2      APB2 clock frequency in Hz
 *
 * @return  
 *  - BSP_SYSTEM_OK: Frequencies retrieved successfully
 *  - BSP_SYSTEM_ERROR: Invalid parameters
 */
bsp_system_status_t bsp_system_get_clocks(bsp_system_config_t *config,
                                          uint32_t *sysclk, 
                                          uint32_t *hclk, 
                                          uint32_t *pclk1, 
                                          uint32_t *pclk2);

/**
 * @brief  Get BSP system version information
 *
 * @param[in]  config  Pointer to BSP system configuration structure
 *
 * @return  Hardware version number (0 if invalid)
 */
uint8_t bsp_system_get_version(bsp_system_config_t *config);

/**
 * @brief  Get MCU type string
 *
 * @param[in]  config  Pointer to BSP system configuration structure
 *
 * @return  Pointer to MCU type string (NULL if invalid)
 */
const char* bsp_system_get_mcu_type(bsp_system_config_t *config);

/**
 * @brief  Get board type string
 *
 * @param[in]  config  Pointer to BSP system configuration structure
 *
 * @return  Pointer to board type string (NULL if invalid)
 */
const char* bsp_system_get_board_type(bsp_system_config_t *config);

/**
 * @brief  System reset function
 *         This function performs a system-wide reset
 *
 * @note   This function does not return
 */
void bsp_system_reset(void);

/**
 * @brief  Enter low power mode
 *         This function configures the system for low power operation
 *
 * @param[in]  config  Pointer to BSP system configuration structure
 *
 * @return  
 *  - BSP_SYSTEM_OK: Low power mode configured successfully
 *  - BSP_SYSTEM_ERROR: Invalid parameters or configuration failed
 */
bsp_system_status_t bsp_system_enter_low_power(bsp_system_config_t *config);

#endif // __BSP_SYSTEM_H

/* End of file -------------------------------------------------------- */

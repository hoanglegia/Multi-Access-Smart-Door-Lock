/**
 * @file       bsp_system_clock.h
 * @copyright  Copyright (c) 2025
 * @license    
 * @version    1.0.0
 * @date       2025-27-9
 * @author     
 * @brief      Board support package for System Clock Configuration
 * @note       This BSP provides an abstraction layer for the system clock setup,
 *             handling hardware-specific initializations and
 *             exposing simplified clock configuration functions.
 * @example    Refer to application layer for usage examples.
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __BSP_SYSTEM_CLOCK_H
#define __BSP_SYSTEM_CLOCK_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "stm32f1xx_hal.h" 

#define BSP_VERSION_1
#ifdef BSP_VERSION_1

/* Public defines ----------------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------------- */
/* Public macros ------------------------------------------------------------ */
/* Public variables --------------------------------------------------------- */
/* Public APIs -------------------------------------------------------------- */
/**
 * @brief         Error handler function prototype.
 *                This function is called when a critical error occurs during
 *                system clock configuration.
 * @param[in]     None
 * @return        None
 * @note          The implementation of this function should handle the error appropriately,
 *                such as logging the error, entering a safe state, or resetting the system.
 */
void bsp_error_handler(void);
/**
 * @brief         Initializes the system clock to a predefined configuration.
 *                This function sets up the main system clock source, PLL settings,
 *                and bus prescalers to achieve the desired system frequency.
 *
 * @param[in]     None
 *
 * @return        None
 *
 * @note          This function should be called at the beginning of the program
 *                before any peripheral initialization that depends on the system clock.
 * @attention     The specific clock configuration parameters (e.g., PLL multipliers, dividers)
 *                are defined internally within the BSP. Ensure they match your hardware design.
 */
void bsp_system_clock_init(void);

/* -------------------------------------------------------------------------- */

#endif /* BSP_VERSION_1 */

#ifdef BSP_VERSION_2

/* Public defines ----------------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------------- */
/* Public macros ------------------------------------------------------------ */
/* Public variables --------------------------------------------------------- */
/* Public APIs -------------------------------------------------------------- */
/* -------------------------------------------------------------------------- */

#endif /* BSP_VERSION_2 */


#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __BSP_ST7735_H */

/* End of file -------------------------------------------------------------- */
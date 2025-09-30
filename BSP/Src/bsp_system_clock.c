/**
 * @file       bsp_system_clock.c
 * @copyright  Copyright (c) 2025
 * @license    
 * @version    1.0.0
 * @date       2025-27-9
 * @author     
 * @brief      Board support package for System Clock Configuration
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include "bsp_system_clock.h"

#ifdef BSP_VERSION_1
/* Public defines ----------------------------------------------------------- */
/* Public enumerate/structure ----------------------------------------------- */
/* Public macros ------------------------------------------------------------ */
/* Public variables --------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */
/* Public implementations --------------------------------------------------- */
void bsp_error_handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

void bsp_system_clock_init(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    bsp_error_handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    bsp_error_handler();
  }
}

/* -------------------------------------------------------------------------- */

#endif /* BSP_VERSION_1 */

#ifdef BSP_VERSION_2
/* Public defines ----------------------------------------------------------- */

/* Public enumerate/structure ----------------------------------------------- */
/* Public macros ------------------------------------------------------------ */

/* Public variables --------------------------------------------------------- */
/* Private variables -------------------------------------------------------- */

/* Public implementations --------------------------------------------------- */
/* -------------------------------------------------------------------------- */

#endif /* BSP_VERSION_2 */


#ifdef __cplusplus
} /* extern "C" { */
#endif


/* End of file -------------------------------------------------------------- */
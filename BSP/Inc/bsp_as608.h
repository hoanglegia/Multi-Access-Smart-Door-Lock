/**
 * @file       bsp_as608.h
 * @copyright  Copyright (C) 2025 YourCompany. All rights reserved.
 * @license    This project is released under the MIT License.
 * @version    1.0.0
 * @date       2025-09-30
 * @author     Your Name
 * @brief      BSP for AS608 Fingerprint Sensor - Hardware Abstraction Layer
 * @note       BSP layer acts as bridge between Application and Driver
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_AS608_H
#define __BSP_AS608_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal.h"
#include "driver_as608.h"
/* Public defines ----------------------------------------------------- */
/**
 * @brief Hardware version control
 * Define BSP_AS608_VERSION in main.h or build system to select hardware configuration
 */
#ifndef BSP_AS608_VERSION
  #define BSP_AS608_VERSION 1
#endif
/* Public defines ----------------------------------------------------- */
/* Hardware Configuration - Current board STM32F103 */
#if (BSP_AS608_VERSION == 1)  // Define BSP_AS608_VERSION in main.h or build system
  #define BSP_AS608_UART_INSTANCE         USART2
  #define BSP_AS608_TX_PIN                GPIO_PIN_2
  #define BSP_AS608_TX_PORT               GPIOA
  #define BSP_AS608_RX_PIN                GPIO_PIN_3
  #define BSP_AS608_RX_PORT               GPIOA
  #define BSP_AS608_BAUD_RATE             57600U
/* Hardware version 2 - Define additional pins or settings here */
#elif (BSP_AS608_VERSION == 2)
  #define BSP_AS608_UART_INSTANCE         USART1
  #define BSP_AS608_TX_PIN                GPIO_PIN_6
  #define BSP_AS608_TX_PORT               GPIOB
  #define BSP_AS608_RX_PIN                GPIO_PIN_7
  #define BSP_AS608_RX_PORT               GPIOB
  #define BSP_AS608_BAUD_RATE             57600U
#endif
/* BSP Configuration */
#define BSP_AS608_COMM_TIMEOUT_MS       (5000U)   /*!< Communication timeout */
#define BSP_AS608_MAX_FINGERPRINTS      (100U)    /*!< Maximum fingerprints */


/* STM32F103 Clock Enable Macros */
#define BSP_AS608_USART2_CLK_ENABLE()   __HAL_RCC_USART2_CLK_ENABLE()
#define BSP_AS608_USART1_CLK_ENABLE()   __HAL_RCC_USART1_CLK_ENABLE()
#define BSP_AS608_GPIOA_CLK_ENABLE()    __HAL_RCC_GPIOA_CLK_ENABLE()
#define BSP_AS608_GPIOB_CLK_ENABLE()    __HAL_RCC_GPIOB_CLK_ENABLE()

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief BSP AS608 status enumeration
 */
typedef enum 
{
  BSP_AS608_OK = 0,           /**< Operation successful */
  BSP_AS608_ERROR,            /**< General error */
  BSP_AS608_TIMEOUT,          /**< Communication timeout */
  BSP_AS608_HW_ERROR,         /**< Hardware initialization error */
  BSP_AS608_NO_FINGER,        /**< No finger detected */
  BSP_AS608_BAD_IMAGE,        /**< Bad image quality */
  BSP_AS608_NOT_MATCH,        /**< Fingerprints don't match */
  BSP_AS608_NOT_FOUND,        /**< Fingerprint not found in database */
  BSP_AS608_INVALID_ID,       /**< Invalid fingerprint ID */
  BSP_AS608_DATABASE_FULL     /**< Database is full */
}
bsp_as608_status_t;

/**
 * @brief BSP AS608 handle structure
 * @note Simple structure to hold driver config and UART handle pointer
 */
typedef struct
{
  as608_config_struct driver_config;  /**< Driver configuration */
  UART_HandleTypeDef *huart;          /**< UART handle pointer */
  uint16_t magic;                     /**< Magic number for validation */
  uint8_t initialized;                /**< Initialization flag */
}
bsp_as608_handle_t;

/* Exported constants --------------------------------------------------------*/

/* Exported macros -----------------------------------------------------------*/
/**
 * @brief Check if finger ID is valid
 */
#define BSP_AS608_IS_VALID_ID(id)     ((id) > 0 && (id) <= BSP_AS608_MAX_FINGERPRINTS)

/* Exported variables --------------------------------------------------------*/

/* Exported functions prototypes ---------------------------------------------*/

/**
 * @brief  Initialize BSP AS608 system
 * @param  handle: Pointer to BSP AS608 handle
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_init(bsp_as608_handle_t *handle);

/**
 * @brief  Deinitialize BSP AS608 system
 * @param  handle: Pointer to BSP AS608 handle
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_deinit(bsp_as608_handle_t *handle);

/**
 * @brief  Get image from sensor (wrapper for driver function)
 * @param  handle: Pointer to BSP AS608 handle
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_get_image(bsp_as608_handle_t *handle);

/**
 * @brief  Create character file (wrapper for driver function)
 * @param  handle: Pointer to BSP AS608 handle
 * @param  buffer_id: Buffer ID (AS608_BUFFER_1 or AS608_BUFFER_2)
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_create_char_file(bsp_as608_handle_t *handle, 
                                              uint8_t buffer_id);

/**
 * @brief  Create template (wrapper for driver function)
 * @param  handle: Pointer to BSP AS608 handle
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_create_template(bsp_as608_handle_t *handle);

/**
 * @brief  Store template (wrapper for driver function)
 * @param  handle: Pointer to BSP AS608 handle
 * @param  buffer_id: Buffer ID
 * @param  finger_id: Fingerprint ID to store
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_store_template(bsp_as608_handle_t *handle,
                                            uint8_t buffer_id,
                                            uint16_t finger_id);

/**
 * @brief  Search fingerprint (wrapper for driver function)
 * @param  handle: Pointer to BSP AS608 handle
 * @param  buffer_id: Buffer ID to search from
 * @param  start_id: Start ID for search
 * @param  end_id: End ID for search
 * @param  found_id: Pointer to store found ID
 * @param  match_score: Pointer to store match score
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_search_fingerprint(bsp_as608_handle_t *handle,
                                                uint8_t buffer_id,
                                                uint16_t start_id,
                                                uint16_t end_id,
                                                uint16_t *found_id,
                                                uint16_t *match_score);
                                                
/**
 * @brief  Delete template (wrapper for driver function)
 * @param  handle: Pointer to BSP AS608 handle
 * @param  finger_id: Fingerprint ID to delete
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_delete_template(bsp_as608_handle_t *handle, 
                                             uint16_t finger_id);

/**
 * @brief  Delete all templates (wrapper for driver function)
 * @param  handle: Pointer to BSP AS608 handle
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_delete_all_templates(bsp_as608_handle_t *handle);

/**
 * @brief  Get template count (wrapper for driver function)
 * @param  handle: Pointer to BSP AS608 handle
 * @param  count: Pointer to store template count
 * @retval BSP status
 */
bsp_as608_status_t bsp_as608_get_template_count(bsp_as608_handle_t *handle, 
                                                uint16_t *count);

/**
 * @brief  Get error string for BSP status code
 * @param  status: BSP status code
 * @retval Error description string
 */
/*
const char* bsp_as608_get_error_string(bsp_as608_status_t status);
*/
#ifdef __cplusplus
}
#endif

#endif /* __BSP_AS608_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/**
 * @file       bsp_as608.c
 * @copyright  Copyright (C) 2025 YourCompany. All rights reserved.
 * @license    This project is released under the MIT License.
 * @version    1.0.0
 * @date       2025-09-30
 * @author     Your Name
 * @brief      BSP AS608 Fingerprint Sensor Implementation
 * @note       BSP layer acts as bridge between Application and Driver
 */

/* Includes ----------------------------------------------------------- */
#include "bsp_as608.h"
#include "main.h"
#include <stdio.h>
#include <string.h>

/* Private defines ---------------------------------------------------- */
#define BSP_AS608_INIT_MAGIC        (0xA5A5)    /*!< Initialization magic number */

/* Private enumerate/structure ---------------------------------------- */

/* Private macros ----------------------------------------------------- */

/* Public variables --------------------------------------------------- */

/* Private variables -------------------------------------------------- */
static as608_response_struct g_as608_response;            /*!< Driver response */
static UART_HandleTypeDef g_huart_as608;                  /*!< Static UART handle */

/* Private function prototypes ---------------------------------------- */
/**
 * @brief  Convert driver status to BSP status
 * @param  driver_status: Driver status code
 * @param  confirmation_code: AS608 confirmation code
 * @retval BSP status
 */
static bsp_as608_status_t convert_driver_status(as608_status_enum driver_status, 
                                                uint8_t confirmation_code);

/* Function definitions ----------------------------------------------- */

/**
 * @brief  Initialize BSP AS608 system
 */
bsp_as608_status_t bsp_as608_init(bsp_as608_handle_t *handle)
{
  if (handle == NULL)
  {
    return BSP_AS608_ERROR;
  }

  /* Clear handle structure */
  memset(handle, 0, sizeof(bsp_as608_handle_t));

  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* Enable GPIO clocks */
  __HAL_RCC_GPIOA_CLK_ENABLE();

  /* Enable UART clock */
  __HAL_RCC_USART2_CLK_ENABLE();

  /* Configure GPIO pins for STM32F103 - TX as alternate push-pull */
  GPIO_InitStruct.Pin = BSP_AS608_TX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  HAL_GPIO_Init(BSP_AS608_TX_PORT, &GPIO_InitStruct);

  /* Configure RX pin as input floating */
  GPIO_InitStruct.Pin = BSP_AS608_RX_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BSP_AS608_RX_PORT, &GPIO_InitStruct);

  /* Configure UART using static handle */
  g_huart_as608.Instance = BSP_AS608_UART_INSTANCE;
  g_huart_as608.Init.BaudRate = BSP_AS608_BAUD_RATE;
  g_huart_as608.Init.WordLength = UART_WORDLENGTH_8B;
  g_huart_as608.Init.StopBits = UART_STOPBITS_1;
  g_huart_as608.Init.Parity = UART_PARITY_NONE;
  g_huart_as608.Init.Mode = UART_MODE_TX_RX;
  g_huart_as608.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  g_huart_as608.Init.OverSampling = UART_OVERSAMPLING_16;

  if (HAL_UART_Init(&g_huart_as608) != HAL_OK)
  {
    return BSP_AS608_HW_ERROR;
  }

  /* Set UART handle pointer */
  handle->huart = &g_huart_as608;

  /* Configure driver */
  handle->driver_config.uart_handle = handle->huart;
  handle->driver_config.timeout_ms = BSP_AS608_COMM_TIMEOUT_MS;

  /* Initialize driver */
  as608_status_enum status = as608_init(&handle->driver_config);
  if (status != AS608_STATUS_OK)
  {
    HAL_UART_DeInit(handle->huart);
    return BSP_AS608_ERROR;
  }

  /* Set magic number and initialization flag */
  handle->magic = BSP_AS608_INIT_MAGIC;
  handle->initialized = 1;

  return BSP_AS608_OK;
}

/**
 * @brief  Deinitialize BSP AS608 system
 */
bsp_as608_status_t bsp_as608_deinit(bsp_as608_handle_t *handle)
{
  if (handle == NULL)
  {
    return BSP_AS608_ERROR;
  }

  /* Deinitialize UART */
  if (handle->huart != NULL)
  {
    HAL_UART_DeInit(handle->huart);
  }

  /* Deinitialize GPIO */
  HAL_GPIO_DeInit(BSP_AS608_TX_PORT, BSP_AS608_TX_PIN);
  HAL_GPIO_DeInit(BSP_AS608_RX_PORT, BSP_AS608_RX_PIN);

  /* Clear handle */
  handle->huart = NULL;
  handle->magic = 0;
  handle->initialized = 0;

  return BSP_AS608_OK;
}

/**
 * @brief  Get image from sensor (wrapper for driver function)
 */
bsp_as608_status_t bsp_as608_get_image(bsp_as608_handle_t *handle)
{
  if (handle == NULL || handle->magic != BSP_AS608_INIT_MAGIC)
  {
    return BSP_AS608_ERROR;
  }

  as608_status_enum status = as608_get_image(&handle->driver_config, &g_as608_response);
  
  return convert_driver_status(status, g_as608_response.confirmation_code);
}

/**
 * @brief  Create character file (wrapper for driver function)
 */
bsp_as608_status_t bsp_as608_create_char_file(bsp_as608_handle_t *handle, 
                                              uint8_t buffer_id)
{
  if (handle == NULL || handle->magic != BSP_AS608_INIT_MAGIC)
  {
    return BSP_AS608_ERROR;
  }

  as608_status_enum status = as608_create_char_file(&handle->driver_config, (as608_buffer_id_enum)buffer_id, &g_as608_response);
  
  return convert_driver_status(status, g_as608_response.confirmation_code);
}

/**
 * @brief  Create template (wrapper for driver function)
 */
bsp_as608_status_t bsp_as608_create_template(bsp_as608_handle_t *handle)
{
  if (handle == NULL || handle->magic != BSP_AS608_INIT_MAGIC)
  {
    return BSP_AS608_ERROR;
  }

  as608_status_enum status = as608_create_template(&handle->driver_config, &g_as608_response);
  
  return convert_driver_status(status, g_as608_response.confirmation_code);
}

/**
 * @brief  Store template (wrapper for driver function)
 */
bsp_as608_status_t bsp_as608_store_template(bsp_as608_handle_t *handle,
                                            uint8_t buffer_id,
                                            uint16_t finger_id)
{
  if (handle == NULL || handle->magic != BSP_AS608_INIT_MAGIC)
  {
    return BSP_AS608_ERROR;
  }

  if (!BSP_AS608_IS_VALID_ID(finger_id))
  {
    return BSP_AS608_INVALID_ID;
  }

  as608_status_enum status = as608_store_template(&handle->driver_config, (as608_buffer_id_enum)buffer_id, finger_id, &g_as608_response);
  
  return convert_driver_status(status, g_as608_response.confirmation_code);
}

/**
 * @brief  Search fingerprint (wrapper for driver function)
 */
bsp_as608_status_t bsp_as608_search_fingerprint(bsp_as608_handle_t *handle,
                                                uint8_t buffer_id,
                                                uint16_t start_id,
                                                uint16_t end_id,
                                                uint16_t *found_id,
                                                uint16_t *match_score)
{
  if (handle == NULL || handle->magic != BSP_AS608_INIT_MAGIC || 
      found_id == NULL || match_score == NULL)
  {
    return BSP_AS608_ERROR;
  }

  // Convert BSP parameters to driver parameters
  uint16_t page_count = end_id - start_id + 1;
  
  as608_status_enum status = as608_search_fingerprint(&handle->driver_config, (as608_buffer_id_enum)buffer_id, 
                                                      start_id, page_count, &g_as608_response);
  
  if (status == AS608_STATUS_OK && g_as608_response.confirmation_code == AS608_RESP_OK)
  {
    *found_id = g_as608_response.found_finger_id;
    *match_score = g_as608_response.match_score;
  }

  return convert_driver_status(status, g_as608_response.confirmation_code);
}
/**
 * @brief  Delete template (wrapper for driver function)
 */
bsp_as608_status_t bsp_as608_delete_template(bsp_as608_handle_t *handle, 
                                             uint16_t finger_id)
{
  if (handle == NULL || handle->magic != BSP_AS608_INIT_MAGIC)
  {
    return BSP_AS608_ERROR;
  }

  if (!BSP_AS608_IS_VALID_ID(finger_id))
  {
    return BSP_AS608_INVALID_ID;
  }

  as608_status_enum status = as608_delete_template(&handle->driver_config, finger_id, &g_as608_response);
  
  return convert_driver_status(status, g_as608_response.confirmation_code);
}

/**
 * @brief  Delete all templates (wrapper for driver function)
 */
bsp_as608_status_t bsp_as608_delete_all_templates(bsp_as608_handle_t *handle)
{
  if (handle == NULL || handle->magic != BSP_AS608_INIT_MAGIC)
  {
    return BSP_AS608_ERROR;
  }

  as608_status_enum status = as608_delete_all_templates(&handle->driver_config, &g_as608_response);
  
  return convert_driver_status(status, g_as608_response.confirmation_code);
}

/**
 * @brief  Get template count (wrapper for driver function)
 */
bsp_as608_status_t bsp_as608_get_template_count(bsp_as608_handle_t *handle, 
                                                uint16_t *count)
{
  if (handle == NULL || handle->magic != BSP_AS608_INIT_MAGIC || count == NULL)
  {
    return BSP_AS608_ERROR;
  }

  as608_status_enum status = as608_get_template_count(&handle->driver_config, &g_as608_response);
  
  if (status == AS608_STATUS_OK && g_as608_response.confirmation_code == AS608_RESP_OK)
  {
    *count = g_as608_response.template_count;
  }

  return convert_driver_status(status, g_as608_response.confirmation_code);
}

/**
 * @brief  Get error string for BSP status code
 */
/*const char* bsp_as608_get_error_string(bsp_as608_status_t status)
{
  switch (status)
  {
    case BSP_AS608_OK:           return "Success";
    case BSP_AS608_ERROR:        return "General error";
    case BSP_AS608_TIMEOUT:      return "Communication timeout";
    case BSP_AS608_HW_ERROR:     return "Hardware initialization error";
    case BSP_AS608_NO_FINGER:    return "No finger detected";
    case BSP_AS608_BAD_IMAGE:    return "Bad image quality";
    case BSP_AS608_NOT_MATCH:    return "Fingerprints don't match";
    case BSP_AS608_NOT_FOUND:    return "Fingerprint not found in database";
    case BSP_AS608_INVALID_ID:   return "Invalid fingerprint ID";
    case BSP_AS608_DATABASE_FULL: return "Database is full";
    default:                     return "Unknown error";
  }
}*/

/* Private function definitions --------------------------------------- */

/**
 * @brief  Convert driver status to BSP status
 */
static bsp_as608_status_t convert_driver_status(as608_status_enum driver_status, 
                                                uint8_t confirmation_code)
{
  if (driver_status != AS608_STATUS_OK)
  {
    return BSP_AS608_TIMEOUT;
  }

  switch (confirmation_code)
  {
    case AS608_RESP_OK:
      return BSP_AS608_OK;
    case AS608_RESP_NO_FINGER:
      return BSP_AS608_NO_FINGER;
    case AS608_RESP_IMAGE_FAIL:
    case AS608_RESP_IMAGE_TOO_DRY:
    case AS608_RESP_IMAGE_TOO_WET:
    case AS608_RESP_IMAGE_DISORDER:
    case AS608_RESP_FEATURE_FAIL:
      return BSP_AS608_BAD_IMAGE;
    case AS608_RESP_NOT_FOUND:
      return BSP_AS608_NOT_FOUND;
    case AS608_RESP_NO_MATCH:
    case AS608_RESP_COMBINE_FAIL:
      return BSP_AS608_NOT_MATCH;
    case AS608_RESP_ADDRESS_OVER:
      return BSP_AS608_DATABASE_FULL;
    case AS608_RESP_READ_ERROR:
    case AS608_RESP_UPLOAD_FAIL:
    case AS608_RESP_DELETE_FAIL:
    case AS608_RESP_CONNECTION_FAIL:
      return BSP_AS608_ERROR;
    case AS608_RESP_TIMEOUT:
      return BSP_AS608_TIMEOUT;
    default:
      return BSP_AS608_ERROR;
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : as608.c
  * @brief          : AS608 Fingerprint Sensor Device Driver Implementation
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "driver_as608.h"
#include "stm32f1xx_hal.h"  /* HAL specific include */
#include "stdint.h"
#include "stdbool.h"
#include "string.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/**
  * @brief AS608 internal constants
  */
#define AS608_CHECKSUM_SIZE           (2)
#define AS608_MIN_PACKET_SIZE         (8)
#define AS608_RX_BYTE_TIMEOUT_MS      (100)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/**
  * @brief AS608 packet header constant definition
  */
const uint8_t AS608_PACKET_HEADER[AS608_PACKET_HEADER_SIZE] = {0xEF, 0x01, 0xFF, 0xFF, 0xFF, 0xFF};

/**
  * @brief Internal receive buffer
  */
static uint8_t g_rx_buffer[AS608_MAX_RESPONSE_LENGTH];

/**
  * @brief Driver initialization flag
  */
static bool g_driver_initialized = false;

/* Private function prototypes -----------------------------------------------*/

/**
  * @brief Calculate packet checksum
  * @param data: Pointer to data buffer
  * @param length: Data length
  * @retval Calculated checksum
  */
static uint16_t calculate_checksum(const uint8_t *data, uint8_t length);

/**
  * @brief Validate AS608 configuration structure
  * @param config: Pointer to configuration structure
  * @retval true if valid, false otherwise
  */
static bool validate_config(const as608_config_struct *config);

/**
  * @brief Send raw data via UART
  * @param config: Pointer to configuration structure
  * @param data: Pointer to data buffer
  * @param length: Data length
  * @retval AS608 driver status
  */
static as608_status_enum send_uart_data(const as608_config_struct *config,
                                        const uint8_t *data,
                                        uint8_t length);

/**
  * @brief Receive raw data via UART
  * @param config: Pointer to configuration structure
  * @param buffer: Pointer to receive buffer
  * @param expected_length: Expected data length
  * @param actual_length: Pointer to store actual received length
  * @retval AS608 driver status
  */
static as608_status_enum receive_uart_data(const as608_config_struct *config,
                                           uint8_t *buffer,
                                           uint8_t expected_length,
                                           uint8_t *actual_length);

/* Private user code ---------------------------------------------------------*/

/* Exported functions --------------------------------------------------------*/

as608_status_enum as608_init(as608_config_struct *config)
{
  /* Validate input parameters */
  if (config == NULL)
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if (config->uart_handle == NULL)
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Set default configuration values if not specified */
  if (config->timeout_ms == 0)
  {
    config->timeout_ms = AS608_DEFAULT_TIMEOUT_MS;
  }

  /* Set default device address */
  config->device_address[0] = 0xFF;
  config->device_address[1] = 0xFF;
  config->device_address[2] = 0xFF;
  config->device_address[3] = 0xFF;

  /* Set default debug mode */
  //config->debug_enable = false;

  /* Set driver as initialized */
  g_driver_initialized = true;

  return AS608_STATUS_OK;
}

as608_status_enum as608_deinit(as608_config_struct *config)
{
  /* Validate configuration */
  if (!validate_config(config))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Clear initialization flag */
  g_driver_initialized = false;

  return AS608_STATUS_OK;
}

as608_status_enum as608_send_header(const as608_config_struct *config)
{
  /* Validate configuration */
  if (!validate_config(config))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if (!g_driver_initialized)
  {
    return AS608_STATUS_NOT_INITIALIZED;
  }

  /* Send packet header */
  return send_uart_data(config, AS608_PACKET_HEADER, AS608_PACKET_HEADER_SIZE);
}

as608_status_enum as608_send_command(const as608_config_struct *config,
                                     const uint8_t *command_data,
                                     uint8_t command_length)
{
  /* Validate parameters */
  if (!validate_config(config) || (command_data == NULL) || (command_length == 0))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if (!g_driver_initialized)
  {
    return AS608_STATUS_NOT_INITIALIZED;
  }

  /* Send command data */
  return send_uart_data(config, command_data, command_length);
}

as608_status_enum as608_receive_response(const as608_config_struct *config,
                                         as608_response_struct *response,
                                         uint8_t expected_length)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL) || (expected_length == 0))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if (!g_driver_initialized)
  {
    return AS608_STATUS_NOT_INITIALIZED;
  }

  /* Clear response structure */
  memset(response, 0, sizeof(as608_response_struct));

  /* Receive response data */
  uint8_t actual_length = 0;
  as608_status_enum status = receive_uart_data(config,
                                               g_rx_buffer,
                                               expected_length,
                                               &actual_length);

  if (status != AS608_STATUS_OK)
  {
    if (actual_length == 0)
    {
      response->confirmation_code = AS608_RESP_CONNECTION_FAIL;
    }
    else
    {
      response->confirmation_code = AS608_RESP_TIMEOUT;
    }
    return status;
  }

  /* Parse response */
  response->confirmation_code = g_rx_buffer[9];
  response->response_length = actual_length;

  /* Copy response data */
  memcpy(response->response_data, g_rx_buffer, actual_length);

  /* Extract additional information for search results */
  if ((response->confirmation_code == AS608_RESP_OK) && (actual_length >= 16))
  {
    response->found_finger_id = ((uint16_t)g_rx_buffer[10] << 8) | g_rx_buffer[11];
    response->match_score = ((uint16_t)g_rx_buffer[12] << 8) | g_rx_buffer[13];
  }

  /* Extract template count information */
  if ((response->confirmation_code == AS608_RESP_OK) && (actual_length >= 14))
  {
    response->template_count = ((uint16_t)g_rx_buffer[10] << 8) | g_rx_buffer[11];
  }
  return AS608_STATUS_OK;
}

as608_status_enum as608_get_image(const as608_config_struct *config,
                                  as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet */
  uint16_t checksum = 0x01 + 0x00 + 0x03 + AS608_CMD_GET_IMAGE;
  uint8_t command_data[] = {
    0x01, 0x00, 0x03, AS608_CMD_GET_IMAGE,
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response */
  return as608_receive_response(config, response, 12);
}

as608_status_enum as608_create_char_file(const as608_config_struct *config,
                                         as608_buffer_id_enum buffer_id,
                                         as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if ((buffer_id != AS608_BUFFER_1) && (buffer_id != AS608_BUFFER_2))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet */
  uint16_t checksum = 0x01 + 0x00 + 0x04 + AS608_CMD_CREATE_CHAR + buffer_id;
  uint8_t command_data[] = {
    0x01, 0x00, 0x04, AS608_CMD_CREATE_CHAR, (uint8_t)buffer_id,
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response */
  return as608_receive_response(config, response, 12);
}

/* Create template from CharBuffer1 & CharBuffer2 */
as608_status_enum as608_create_template(const as608_config_struct *config,
                                        as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet - Combine CharBuffer1 & CharBuffer2 into template */
  uint16_t checksum = 0x01 + 0x00 + 0x03 + AS608_CMD_CREATE_TEMPLATE;
  uint8_t command_data[] = {
    0x01, 0x00, 0x03, AS608_CMD_CREATE_TEMPLATE,
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command - Combine buffer 1 and buffer 2 into template */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response - Template is created and stored in Buffer 1 */
  return as608_receive_response(config, response, 12);
}

as608_status_enum as608_store_template(const as608_config_struct *config,
                                       as608_buffer_id_enum buffer_id,
                                       uint16_t location_id,
                                       as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if (location_id > AS608_MAX_FINGER_ID)
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet */
  uint16_t checksum = 0x01 + 0x00 + 0x06 + AS608_CMD_STORE_TEMPLATE + 
                      buffer_id + (location_id >> 8) + (location_id & 0xFF);
  
  uint8_t command_data[] = {
    0x01, 0x00, 0x06, AS608_CMD_STORE_TEMPLATE, (uint8_t)buffer_id,
    (uint8_t)(location_id >> 8), (uint8_t)(location_id & 0xFF),
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response */
  return as608_receive_response(config, response, 12);
}

as608_status_enum as608_search_fingerprint(const as608_config_struct *config,
                                          as608_buffer_id_enum buffer_id,
                                          uint16_t start_id,
                                          uint16_t count,
                                          as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if ((buffer_id != AS608_BUFFER_1) && (buffer_id != AS608_BUFFER_2))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if ((start_id > AS608_MAX_FINGER_ID) || (count == 0) || 
      (start_id + count > AS608_MAX_FINGER_ID))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet */
  uint16_t checksum = 0x01 + 0x00 + 0x08 + AS608_CMD_SEARCH_FINGER + 
                      buffer_id + (start_id >> 8) + (start_id & 0xFF) + 
                      (count >> 8) + (count & 0xFF);
  
  uint8_t command_data[] = {
    0x01, 0x00, 0x08, AS608_CMD_SEARCH_FINGER, (uint8_t)buffer_id,
    (uint8_t)(start_id >> 8), (uint8_t)(start_id & 0xFF),
    (uint8_t)(count >> 8), (uint8_t)(count & 0xFF),
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response */
  return as608_receive_response(config, response, 16);
}

as608_status_enum as608_delete_all_templates(const as608_config_struct *config,
                                             as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet */
  uint16_t checksum = 0x01 + 0x00 + 0x03 + AS608_CMD_DELETE_ALL;
  uint8_t command_data[] = {
    0x01, 0x00, 0x03, AS608_CMD_DELETE_ALL,
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response */
  return as608_receive_response(config, response, 12);
}

as608_status_enum as608_delete_template(const as608_config_struct *config,
                                        uint16_t location_id,
                                        as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if (location_id > AS608_MAX_FINGER_ID)
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet - delete single fingerprint at location_id */
  uint16_t checksum = 0x01 + 0x00 + 0x07 + AS608_CMD_DELETE_CHAR + 
                      (location_id >> 8) + (location_id & 0xFF) + 
                      0x00 + 0x01;  // Delete 1 template (count = 1)
  
  uint8_t command_data[] = {
    0x01, 0x00, 0x07, AS608_CMD_DELETE_CHAR,
    (uint8_t)(location_id >> 8), (uint8_t)(location_id & 0xFF),
    0x00, 0x01,  // Delete only 1 fingerprint (count = 1)
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response */
  return as608_receive_response(config, response, 12);
}

as608_status_enum as608_get_template_count(const as608_config_struct *config,
                                           as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet */
  uint16_t checksum = 0x01 + 0x00 + 0x03 + AS608_CMD_GET_TEMPLATE_COUNT;
  uint8_t command_data[] = {
    0x01, 0x00, 0x03, AS608_CMD_GET_TEMPLATE_COUNT,
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response */
  return as608_receive_response(config, response, 14);
}

as608_status_enum as608_read_template(const as608_config_struct *config,
                                      as608_buffer_id_enum buffer_id,
                                      uint16_t location_id,
                                      as608_response_struct *response)
{
  /* Validate parameters */
  if (!validate_config(config) || (response == NULL))
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  if (location_id > AS608_MAX_FINGER_ID)
  {
    return AS608_STATUS_INVALID_PARAM;
  }

  /* Send packet header */
  as608_status_enum status = as608_send_header(config);
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Create command packet */
  uint16_t checksum = 0x01 + 0x00 + 0x06 + AS608_CMD_READ_TEMPLATE + 
                      buffer_id + (location_id >> 8) + (location_id & 0xFF);
  
  uint8_t command_data[] = {
    0x01, 0x00, 0x06, AS608_CMD_READ_TEMPLATE, (uint8_t)buffer_id,
    (uint8_t)(location_id >> 8), (uint8_t)(location_id & 0xFF),
    (uint8_t)(checksum >> 8), (uint8_t)(checksum & 0xFF)
  };

  /* Send command */
  status = as608_send_command(config, command_data, sizeof(command_data));
  if (status != AS608_STATUS_OK)
  {
    return status;
  }

  /* Receive response */
  return as608_receive_response(config, response, 12);
}

/* Private functions ---------------------------------------------------------*/

static uint16_t calculate_checksum(const uint8_t *data, uint8_t length)
{
  uint16_t checksum = 0;
  
  for (uint8_t i = 0; i < length; i++)
  {
    checksum += data[i];
  }
  
  return checksum;
}

static bool validate_config(const as608_config_struct *config)
{
  return (config != NULL) && (config->uart_handle != NULL);
}

static as608_status_enum send_uart_data(const as608_config_struct *config,
                                        const uint8_t *data,
                                        uint8_t length)
{
  HAL_StatusTypeDef hal_status = HAL_UART_Transmit(config->uart_handle,
                                                   (uint8_t *)data,
                                                   length,
                                                   config->timeout_ms);
  
  return (hal_status == HAL_OK) ? AS608_STATUS_OK : AS608_STATUS_ERROR;
}

static as608_status_enum receive_uart_data(const as608_config_struct *config,
                                           uint8_t *buffer,
                                           uint8_t expected_length,
                                           uint8_t *actual_length)
{
  /* Clear buffer */
  memset(buffer, 0xFF, AS608_MAX_RESPONSE_LENGTH);
  
  uint8_t bytes_received = 0;
  uint8_t single_byte[1];
  uint32_t start_time = HAL_GetTick();

  /* Receive data byte by byte with timeout */
  while ((HAL_GetTick() - start_time < config->timeout_ms) && 
         (bytes_received < expected_length))
  {
    if (HAL_UART_Receive(config->uart_handle, 
                         single_byte, 
                         1, 
                         AS608_RX_BYTE_TIMEOUT_MS) == HAL_OK)
    {
      buffer[bytes_received] = single_byte[0];
      bytes_received++;
    }
  }

  /* Set actual received length */
  if (actual_length != NULL)
  {
    *actual_length = bytes_received;
  }
  
  /* Check reception result */
  if (bytes_received == 0)
  {
    return AS608_STATUS_ERROR;
  }
  else if (bytes_received < expected_length)
  {
    return AS608_STATUS_TIMEOUT;
  }
  else
  {
    return AS608_STATUS_OK;
  }
}
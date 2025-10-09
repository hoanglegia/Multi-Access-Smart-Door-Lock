/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : as608.h
  * @brief          : AS608 Fingerprint Sensor Device Driver Header
  ******************************************************************************
  * @attention
  *
  * AS608 fingerprint sensor low-level device driver
  * Compatible with R307, R551, ZFM-20 series
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AS608_H
#define __AS608_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Exported types ------------------------------------------------------------*/

/* Forward declaration for HAL UART handle */
typedef struct __UART_HandleTypeDef UART_HandleTypeDef;

/**
  * @brief AS608 command definitions
  */
#define AS608_CMD_GET_IMAGE           (0x01)
#define AS608_CMD_CREATE_CHAR         (0x02)
#define AS608_CMD_SEARCH_FINGER       (0x04)
#define AS608_CMD_CREATE_TEMPLATE     (0x05)
#define AS608_CMD_STORE_TEMPLATE      (0x06)
#define AS608_CMD_READ_TEMPLATE       (0x07)
#define AS608_CMD_DELETE_CHAR         (0x0C)
#define AS608_CMD_DELETE_ALL          (0x0D)
#define AS608_CMD_GET_TEMPLATE_COUNT  (0x1D)

/**
  * @brief AS608 response code definitions
  */
#define AS608_RESP_OK                 (0x00)
#define AS608_RESP_ERROR              (0x01)
#define AS608_RESP_NO_FINGER          (0x02)
#define AS608_RESP_IMAGE_FAIL         (0x03)
#define AS608_RESP_IMAGE_TOO_DRY      (0x04)
#define AS608_RESP_IMAGE_TOO_WET      (0x05)
#define AS608_RESP_IMAGE_DISORDER     (0x06)
#define AS608_RESP_FEATURE_FAIL       (0x07)
#define AS608_RESP_NO_MATCH           (0x08)
#define AS608_RESP_NOT_FOUND          (0x09)
#define AS608_RESP_COMBINE_FAIL       (0x0A)
#define AS608_RESP_ADDRESS_OVER       (0x0B)
#define AS608_RESP_READ_ERROR         (0x0C)
#define AS608_RESP_UPLOAD_FAIL        (0x0D)
#define AS608_RESP_DELETE_FAIL        (0x10)
#define AS608_RESP_EMPTY              (0x11)
#define AS608_RESP_TIMEOUT            (0xFE)
#define AS608_RESP_CONNECTION_FAIL    (0xFF)

/**
  * @brief AS608 configuration constants
  */
#define AS608_DEFAULT_TIMEOUT_MS      (3000)
#define AS608_MAX_FINGER_ID           (999)
#define AS608_MAX_RESPONSE_LENGTH     (32)
#define AS608_PACKET_HEADER_SIZE      (6)

/**
  * @brief AS608 buffer ID enumeration
  */
typedef enum
{
  AS608_BUFFER_1 = 1,
  AS608_BUFFER_2 = 2
} as608_buffer_id_enum;

/**
  * @brief AS608 driver status enumeration
  */
typedef enum
{
  AS608_STATUS_OK = 0,
  AS608_STATUS_ERROR,
  AS608_STATUS_TIMEOUT,
  AS608_STATUS_INVALID_PARAM,
  AS608_STATUS_NOT_INITIALIZED
} as608_status_enum;

/**
  * @brief AS608 hardware configuration structure
  */
typedef struct
{
  UART_HandleTypeDef *uart_handle;      ///< UART handle for communication
  uint16_t timeout_ms;                  ///< Communication timeout in milliseconds
  uint8_t device_address[4];            ///< Device address array
  //bool debug_enable;                    ///< Debug output enable flag
} as608_config_struct;

/**
  * @brief AS608 response data structure
  */
typedef struct
{
  uint8_t confirmation_code;            ///< Response confirmation code
  uint8_t response_data[AS608_MAX_RESPONSE_LENGTH]; ///< Response data buffer
  uint8_t response_length;              ///< Response data length
  uint16_t found_finger_id;             ///< Found finger ID (search result)
  uint16_t match_score;                 ///< Match confidence score
  uint16_t template_count;              ///< Total template count
} as608_response_struct;

/* Exported constants --------------------------------------------------------*/

/**
  * @brief AS608 packet header constant
  */
extern const uint8_t AS608_PACKET_HEADER[AS608_PACKET_HEADER_SIZE];

/* Exported macro ------------------------------------------------------------*/

/**
  * @brief Check if response code indicates success
  */
#define AS608_IS_SUCCESS(code)    ((code) == AS608_RESP_OK)

/**
  * @brief Check if response code indicates error
  */
#define AS608_IS_ERROR(code)      ((code) != AS608_RESP_OK)

/* Exported functions prototypes ---------------------------------------------*/

/**
  * @brief Initialize AS608 driver
  * @param config: Pointer to AS608 configuration structure
  * @retval AS608 driver status
  */
as608_status_enum as608_init(as608_config_struct *config);

/**
  * @brief Deinitialize AS608 driver
  * @param config: Pointer to AS608 configuration structure
  * @retval AS608 driver status
  */
as608_status_enum as608_deinit(as608_config_struct *config);

/**
  * @brief Send packet header to AS608
  * @param config: Pointer to AS608 configuration structure
  * @retval AS608 driver status
  */
as608_status_enum as608_send_header(const as608_config_struct *config);

/**
  * @brief Send command packet to AS608
  * @param config: Pointer to AS608 configuration structure
  * @param command_data: Pointer to command data buffer
  * @param command_length: Command data length
  * @retval AS608 driver status
  */
as608_status_enum as608_send_command(const as608_config_struct *config,
                                     const uint8_t *command_data,
                                     uint8_t command_length);

/**
  * @brief Receive response from AS608
  * @param config: Pointer to AS608 configuration structure
  * @param response: Pointer to response structure
  * @param expected_length: Expected response length
  * @retval AS608 driver status
  */
as608_status_enum as608_receive_response(const as608_config_struct *config,
                                         as608_response_struct *response,
                                         uint8_t expected_length);

/**
  * @brief Get fingerprint image from sensor
  * @param config: Pointer to AS608 configuration structure
  * @param response: Pointer to response structure
  * @retval AS608 driver status
  */
as608_status_enum as608_get_image(const as608_config_struct *config,
                                  as608_response_struct *response);

/**
  * @brief Create character file from fingerprint image
  * @param config: Pointer to AS608 configuration structure
  * @param buffer_id: Buffer ID (AS608_BUFFER_1 or AS608_BUFFER_2)
  * @param response: Pointer to response structure
  * @retval AS608 driver status
  */
as608_status_enum as608_create_char_file(const as608_config_struct *config,
                                         as608_buffer_id_enum buffer_id,
                                         as608_response_struct *response);

/**
  * @brief Create template by combining CharBuffer1 and CharBuffer2
  * @param config: Pointer to AS608 configuration structure
  * @param response: Pointer to response structure
  * @retval AS608 driver status
  * @note Template is created from features stored in CharBuffer1 and CharBuffer2,
  *       then stored back into CharBuffer1
  */
as608_status_enum as608_create_template(const as608_config_struct *config,
                                        as608_response_struct *response);

/**
  * @brief Store template to flash memory
  * @param config: Pointer to AS608 configuration structure
  * @param buffer_id: Buffer ID containing template
  * @param location_id: Storage location ID
  * @param response: Pointer to response structure
  * @retval AS608 driver status
  */
as608_status_enum as608_store_template(const as608_config_struct *config,
                                       as608_buffer_id_enum buffer_id,
                                       uint16_t location_id,
                                       as608_response_struct *response);

/**
  * @brief Search fingerprint in database
  * @param config: Pointer to AS608 configuration structure
  * @param buffer_id: Buffer ID containing fingerprint features to search
  * @param start_page: Start fingerprint ID for search (typically 0)
  * @param page_count: Number of fingerprints to search through
  * @param response: Pointer to response structure (contains found_finger_id and match_score)
  * @retval AS608 driver status
  */
as608_status_enum as608_search_fingerprint(const as608_config_struct *config,
                                           as608_buffer_id_enum buffer_id,
                                           uint16_t start_page,
                                           uint16_t page_count,
                                           as608_response_struct *response);

/**
  * @brief Delete single fingerprint template
  * @param config: Pointer to AS608 configuration structure
  * @param location_id: Template location ID to delete
  * @param response: Pointer to response structure
  * @retval AS608 driver status
  */
as608_status_enum as608_delete_template(const as608_config_struct *config,
                                        uint16_t location_id,
                                        as608_response_struct *response);

/**
  * @brief Delete all fingerprint templates
  * @param config: Pointer to AS608 configuration structure
  * @param response: Pointer to response structure
  * @retval AS608 driver status
  */
as608_status_enum as608_delete_all_templates(const as608_config_struct *config,
                                             as608_response_struct *response);

/**
  * @brief Get template count
  * @param config: Pointer to AS608 configuration structure
  * @param response: Pointer to response structure
  * @retval AS608 driver status
  */
as608_status_enum as608_get_template_count(const as608_config_struct *config,
                                           as608_response_struct *response);

/**
  * @brief Read template from flash memory
  * @param config: Pointer to AS608 configuration structure
  * @param buffer_id: Buffer ID to store read template
  * @param location_id: Template location ID to read
  * @param response: Pointer to response structure
  * @retval AS608 driver status
  */
as608_status_enum as608_read_template(const as608_config_struct *config,
                                      as608_buffer_id_enum buffer_id,
                                      uint16_t location_id,
                                      as608_response_struct *response);

/* Private defines -----------------------------------------------------------*/
/* Private macros ------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif /* __AS608_H */
/**
 * @file       driver_rc522.h
 * @copyright  Copyright (C) 2025
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025
 * @author     Bao Quoc
 *
 * @brief      Generic, hardware-independent driver for MFRC522 RFID module.
 *
 * @note       Using STM32F1 HAL (SPI + GPIO)
 * @example    main.c
 *             Example of reading card UID using MFRC522_Check()
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __DRIVER_RC522_H
#define __DRIVER_RC522_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>

/* Public defines ----------------------------------------------------- */
/**
 * @brief Size of the card UID in bytes.
 */
#define MFRC522_UID_SIZE    5

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Driver status enumeration.
 */
typedef enum
{
    RFID_STATUS_OK = 0,        /**< Operation successful */
    RFID_STATUS_ERROR,         /**< General communication error */
    RFID_STATUS_NO_TAG_ERR,    /**< No tag found in the field */
    RFID_STATUS_INVALID_PARAM, /**< Invalid parameter provided */
    RFID_STATUS_NOT_INIT,      /**< Driver not initialized */
} rfid_status_t;

/**
 * @brief Hardware I/O interface structure (to be provided by BSP).
 */
typedef struct
{
    void (*spi_transmit)(uint8_t*, uint16_t);
    void (*spi_receive)(uint8_t*, uint16_t);
    void (*spi_transmit_receive)(uint8_t*, uint8_t*, uint16_t);
    void (*cs_low)(void);
    void (*cs_high)(void);
    void (*reset_low)(void);
    void (*reset_high)(void);
    void (*delay_ms)(uint32_t);
} rfid_io_t;

/**
 * @brief Driver handle structure.
 */
typedef struct
{
    rfid_io_t* io;          /**< Pointer to the I/O interface functions */
    bool       is_init;     /**< Initialization flag */
} rfid_handle_t;

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */
/* Public APIs ----------------------------------------- */
/**
 * @brief Initializes the MFRC522 driver.
 *
 * @param[out] handle Pointer to the driver handle structure.
 * @param[in]  io     Pointer to the hardware I/O interface structure provided by BSP.
 * @return rfid_status_t Operation status.
 */
rfid_status_t rfid_init(rfid_handle_t* handle, rfid_io_t* io);

/**
 * @brief Checks for a new card in the field and reads its UID.
 *
 * @param[in]  handle Pointer to the driver handle.
 * @param[out] uid    Pointer to a buffer to store the card's UID (must be MFRC522_UID_SIZE).
 * @return rfid_status_t Operation status. Returns RFID_STATUS_OK if a card is found.
 */
rfid_status_t rfid_check_card(rfid_handle_t* handle, uint8_t* uid);

#endif // __DRIVER_RC522_H

/* End of file -------------------------------------------------------- */

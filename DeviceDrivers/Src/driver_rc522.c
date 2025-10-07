/**
 * @file       driver_rc522.c
 * @copyright  Copyright (C) 2025
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025
 * @author     Bao Quoc
 *
 * @brief      Implementation of the generic MFRC522 driver.
 *
 * @note       Tested on STM32F103C8T6 using CubeMX + HAL
 * @example    main.c
 *             Example of using mfrc522_check() to read card UID
 */

/* Includes ----------------------------------------------------------- */
#include "driver_rc522.h"
#include <stddef.h>

/* Private defines ---------------------------------------------------- */

// === MFRC522 Command Set (for CommandReg) ===
#define PCD_IDLE              0x00  /**< No action, cancel current command */
#define PCD_AUTHENT           0x0E  /**< Perform authentication */
#define PCD_RECEIVE           0x08  /**< Activate receiver */
#define PCD_TRANSMIT          0x04  /**< Transmit data from FIFO */
#define PCD_TRANSCEIVE        0x0C  /**< Transmit from FIFO and activate receiver */
#define PCD_RESETPHASE        0x0F  /**< Perform a software reset */
#define PCD_CALCCRC           0x03  /**< Activate CRC coprocessor */

// === PICC (Card) Command Set ===
#define PICC_REQIDL           0x26  /**< Request command for idle cards */
#define PICC_REQALL           0x52  /**< Request command for all cards */
#define PICC_ANTICOLL         0x93  /**< Anticollision command */
#define PICC_SElECTTAG        0x93  /**< Select card command */
#define PICC_AUTHENT1A        0x60  /**< Authenticate with Key A */
#define PICC_AUTHENT1B        0x61  /**< Authenticate with Key B */
#define PICC_READ             0x30  /**< Read block command */
#define PICC_WRITE            0xA0  /**< Write block command */
#define PICC_HALT             0x50  /**< Halt card command */

// IRQ bits
#define RxIRq           5
#define IdleIRq         4

// === MFRC522 Register Addresses ===
// Page 0: Command and Status
#define CommandReg            0x01  /**< Starts and stops command execution */
#define ComIEnReg             0x02  /**< Enable and disable interrupt request signals */
#define DivIrqReg             0x05  /**< Interrupt request bits */
#define ComIrqReg             0x04  /**< Interrupt request bits */
#define ErrorReg              0x06  /**< Error bits showing the error status of the last command */
#define Status1Reg            0x07  /**< Communication status bits */
#define Status2Reg            0x08  /**< Receiver and transmitter status bits */
#define FIFODataReg           0x09  /**< Input and output of 64 byte FIFO buffer */
#define FIFOLevelReg          0x0A  /**< Number of bytes stored in the FIFO buffer */
#define ControlReg            0x0C  /**< Miscellaneous control bits */
#define BitFramingReg         0x0D  /**< Adjustments for bit-oriented frames */
#define CollReg               0x0E  /**< Bit position of the first bit-collision detected */

// Page 1: Communication
#define ModeReg               0x11  /**< Defines general modes for transmitting and receiving */
#define TxModeReg             0x12  /**< Defines transmission data rate and framing */
#define RxModeReg             0x13  /**< Defines reception data rate and framing */
#define TxControlReg          0x14  /**< Controls the logical behavior of the antenna driver pins */
#define TxAutoReg             0x15  /**< Controls the setting of the transmission modulation */

// Page 2: Configuration
#define CRCResultRegM         0x21  /**< Shows the MSB and LSB values of the CRC calculation */
#define CRCResultRegL         0x22
#define TModeReg              0x2A  /**< Defines settings for the internal timer */
#define TPrescalerReg         0x2B  /**< Defines settings for the internal timer */
#define TReloadRegH           0x2C  /**< Defines the 16-bit timer reload value */
#define TReloadRegL           0x2D

// === Driver Configuration and Constants ===
#define MFRC522_MAX_LEN       16    /**< Maximum length of data in FIFO buffer */

// Default Initialization Values
#define MFRC522_TMODE_CFG       0x8D  /**< TModeReg: set TAuto=1; TPrescalerHi=0x0D */
#define MFRC522_TPRESCALER_CFG  0x3E  /**< TPrescalerReg: set prescaler low byte */
#define MFRC522_TRELOADL_CFG    30    /**< TReloadRegL: timer reload low byte */
#define MFRC522_TRELOADH_CFG    0     /**< TReloadRegH: timer reload high byte */
#define MFRC522_TXAUTO_CFG      0x40  /**< TxAutoReg: 100% ASK modulation */
#define MFRC522_MODE_CFG        0x3D  /**< ModeReg: CRC preset = 0x6363 */

// SPI Address/Command Formatting
#define MFRC522_SPI_WRITE_MASK      0x7E  /**< Address format for write: 0XXXXXX0 */
#define MFRC522_SPI_READ_FLAG       0x80  /**< Flag to indicate a read operation */

// Register Bit Masks
#define IRQ_CLEAR_ALL_MASK          0x7F  /**< Clears all interrupt request bits in ComIrqReg */
#define ERROR_COMM_MASK             0x1B  /**< Mask for communication errors in ErrorReg */

// PICC Command Parameters
#define PICC_ANTICOLL_SEL_CL1       0x20  /**< NVB byte for anticollision cascade level 1 */


/* Private enumerate/structure ---------------------------------------- */
/* Private macros ----------------------------------------------------- */
/* Public variables --------------------------------------------------- */
/* Private variables -------------------------------------------------- */
/* Private function prototypes ---------------------------------------- */

/**
 * @brief  Writes a byte to a specified register in the MFRC522.
 * @param[in]  handle Pointer to the driver handle.
 * @param[in]  addr   The register address.
 * @param[in]  val    The value to write.
 */
static void rfid_write_register(rfid_handle_t* handle, uint8_t addr, uint8_t val);

/**
 * @brief  Reads a byte from a specified register in the MFRC522.
 * @param[in]  handle Pointer to the driver handle.
 * @param[in]  addr   The register address.
 * @return     uint8_t The value read from the register.
 */
static uint8_t rfid_read_register(rfid_handle_t* handle, uint8_t addr);

/**
 * @brief  Sets a bit mask on a specified register.
 * @param[in]  handle Pointer to the driver handle.
 * @param[in]  reg    The register address.
 * @param[in]  mask   The bit mask to set (e.g., 0x80 to set bit 7).
 */
static void rfid_set_bit_mask(rfid_handle_t* handle, uint8_t reg, uint8_t mask);

/**
 * @brief  Clears a bit mask on a specified register.
 * @param[in]  handle Pointer to the driver handle.
 * @param[in]  reg    The register address.
 * @param[in]  mask   The bit mask to clear (e.g., 0x80 to clear bit 7).
 */
static void rfid_clear_bit_mask(rfid_handle_t* handle, uint8_t reg, uint8_t mask);

/**
 * @brief  Transfers data to a card and receives data back.
 * @param[in]  handle   Pointer to the driver handle.
 * @param[in]  command  The command to execute (e.g., PCD_TRANSCEIVE).
 * @param[in]  sendData Pointer to the data to send.
 * @param[in]  sendLen  Length of the data to send.
 * @param[out] backData Pointer to a buffer to store received data.
 * @param[out] backLen  Pointer to store the length of received data in bits.
 * @return     rfid_status_t Operation status.
 */
static rfid_status_t rfid_to_card(rfid_handle_t* handle, uint8_t command,
                                  uint8_t* sendData, uint8_t sendLen,
                                  uint8_t* backData, uint16_t* backLen);

/**
 * @brief  Sends a request command to find cards.
 * @param[in]  handle  Pointer to the driver handle.
 * @param[in]  reqMode Request mode (PICC_REQIDL for idle cards).
 * @param[out] tagType Pointer to a buffer to store the card type (ATQA).
 * @return     rfid_status_t Operation status.
 */
static rfid_status_t rfid_request(rfid_handle_t* handle, uint8_t reqMode, uint8_t* tagType);

/**
 * @brief  Handles anti-collision to select a single card and get its UID.
 * @param[in]  handle Pointer to the driver handle.
 * @param[out] serNum Pointer to a buffer to store the card's serial number (UID).
 * @return     rfid_status_t Operation status.
 */
static rfid_status_t rfid_anticoll(rfid_handle_t* handle, uint8_t* serNum);

/**
 * @brief  Turns on the antenna by setting the appropriate bits in TxControlReg.
 * @param[in]  handle Pointer to the driver handle.
 */
static void rfid_antenna_on(rfid_handle_t* handle);

/**
 * @brief  Performs a software reset of the MFRC522 chip.
 * @param[in]  handle Pointer to the driver handle.
 */
static void rfid_reset(rfid_handle_t* handle);

/* Public implementations --------------------------------------------------- */

rfid_status_t rfid_init(rfid_handle_t* handle, rfid_io_t* io)
{
    if (handle == NULL || io == NULL)
    {
        return RFID_STATUS_INVALID_PARAM;
    }

    handle->io = io;
    handle->is_init = false;

    // Perform hardware reset via BSP
    handle->io->reset_high();
    handle->io->delay_ms(1);
    handle->io->reset_low();
    handle->io->delay_ms(1);
    handle->io->reset_high();
    handle->io->delay_ms(10);

    // Perform software reset
    rfid_reset(handle);

    // Timer configuration (values from datasheet/common practice)
    rfid_write_register(handle, TModeReg, MFRC522_TMODE_CFG);
    rfid_write_register(handle, TPrescalerReg, MFRC522_TPRESCALER_CFG);
    rfid_write_register(handle, TReloadRegL, MFRC522_TRELOADL_CFG);
    rfid_write_register(handle, TReloadRegH, MFRC522_TRELOADH_CFG);

    // Other configurations
    rfid_write_register(handle, TxAutoReg, MFRC522_TXAUTO_CFG);  // 100% ASK
    rfid_write_register(handle, ModeReg, MFRC522_MODE_CFG);    // CRC preset 0x6363

    // Turn on antenna
    rfid_antenna_on(handle);

    handle->is_init = true;
    return RFID_STATUS_OK;
}

rfid_status_t rfid_check_card(rfid_handle_t* handle, uint8_t* uid)
{
    if ((handle == NULL) || (!handle->is_init) || (uid == NULL))
    {
        return RFID_STATUS_INVALID_PARAM;
    }

    rfid_status_t status;

    // Step 1: Find cards
    status = rfid_request(handle, PICC_REQIDL, uid);
    if (status != RFID_STATUS_OK)
    {
        return status;
    }

    // Step 2: Anti-collision, get UID
    status = rfid_anticoll(handle, uid);

    return status;
}

/* Private implementations -------------------------------------------------- */

void rfid_write_register(rfid_handle_t* handle, uint8_t addr, uint8_t val)
{
    handle->io->cs_low();
    addr = (addr << 1) & MFRC522_SPI_WRITE_MASK;
    handle->io->spi_transmit(&addr, 1);
    handle->io->spi_transmit(&val, 1);
    handle->io->cs_high();
}

uint8_t rfid_read_register(rfid_handle_t* handle, uint8_t addr)
{
    uint8_t val;
    handle->io->cs_low();
    addr = ((addr << 1) & MFRC522_SPI_WRITE_MASK) | MFRC522_SPI_READ_FLAG;
    handle->io->spi_transmit(&addr, 1);
    handle->io->spi_receive(&val, 1);
    handle->io->cs_high();
    return val;
}

static void rfid_set_bit_mask(rfid_handle_t* handle, uint8_t reg, uint8_t mask)
{
    rfid_write_register(handle, reg, rfid_read_register(handle, reg) | mask);
}

static void rfid_clear_bit_mask(rfid_handle_t* handle, uint8_t reg, uint8_t mask)
{
    rfid_write_register(handle, reg, rfid_read_register(handle, reg) & (~mask));
}

static void rfid_antenna_on(rfid_handle_t* handle)
{
    uint8_t temp = rfid_read_register(handle, TxControlReg);
    if (!(temp & PCD_CALCCRC))
    {
        rfid_set_bit_mask(handle, TxControlReg, PCD_CALCCRC);
    }
}

static void rfid_reset(rfid_handle_t* handle)
{
    rfid_write_register(handle, CommandReg, PCD_RESETPHASE);
}

// Complex communication function (simplified for clarity)
static rfid_status_t rfid_to_card(rfid_handle_t* handle, uint8_t command,
                                  uint8_t* sendData, uint8_t sendLen,
                                  uint8_t* backData, uint16_t* backLen)
{
    rfid_write_register(handle, CommandReg, PCD_IDLE);
    rfid_write_register(handle, ComIrqReg, IRQ_CLEAR_ALL_MASK); // Clear all IRQ bits
    rfid_write_register(handle, FIFOLevelReg, MFRC522_SPI_READ_FLAG); // Flush FIFO

    for (uint8_t i = 0; i < sendLen; i++)
    {
        rfid_write_register(handle, FIFODataReg, sendData[i]);
    }

    rfid_write_register(handle, CommandReg, command);
    if (command == PCD_TRANSCEIVE)
    {
        rfid_set_bit_mask(handle, BitFramingReg, MFRC522_SPI_READ_FLAG); // Start transmission
    }

    uint16_t timeout = 2000;
    uint8_t irq_val = 0;
    while (timeout--)
    {
        irq_val = rfid_read_register(handle, ComIrqReg);
        if (irq_val & PICC_READ) // RxIrq or IdleIrq
        {
            break;
        }
    }
    rfid_clear_bit_mask(handle, BitFramingReg, MFRC522_SPI_READ_FLAG);

    if (timeout == 0)
    {
        return RFID_STATUS_NO_TAG_ERR;
    }
    if (rfid_read_register(handle, ErrorReg) & ERROR_COMM_MASK)
    {
        return RFID_STATUS_ERROR;
    }
    uint8_t len = rfid_read_register(handle, FIFOLevelReg);
    for (uint8_t i = 0; i < len; i++)
    {
        backData[i] = rfid_read_register(handle, FIFODataReg);
    }
    *backLen = len;

    return RFID_STATUS_OK;
}

static rfid_status_t rfid_request(rfid_handle_t* handle, uint8_t reqMode, uint8_t* tagType)
{
    uint16_t len;
    rfid_write_register(handle, BitFramingReg, Status1Reg); // 7 bits of last byte are valid
    tagType[0] = reqMode;
    return rfid_to_card(handle, PCD_TRANSCEIVE, tagType, 1, tagType, &len);
}

static rfid_status_t rfid_anticoll(rfid_handle_t* handle, uint8_t* serNum)
{
    uint16_t len;
    rfid_write_register(handle, BitFramingReg, PCD_IDLE); // All bits are valid
    serNum[0] = PICC_ANTICOLL;
    serNum[1] = PICC_ANTICOLL_SEL_CL1;
    rfid_status_t status = rfid_to_card(handle, PCD_TRANSCEIVE, serNum, 2, serNum, &len);
    if (status == RFID_STATUS_OK)
    {
        // Simple check, a full implementation would check CRC
        if (len != 5) status = RFID_STATUS_ERROR;
    }
    return status;
}

/* End of file -------------------------------------------------------- */

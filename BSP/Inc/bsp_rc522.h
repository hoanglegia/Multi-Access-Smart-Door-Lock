/**
 * @file       bsp_rc522.h
 * @copyright  Copyright (C) 2025
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025
 * @author     Bao Quoc
 *
 * @brief      Board Support Package (BSP) for MFRC522 RFID reader
 *
 * @note       Using STM32F1 HAL (SPI + GPIO)
 * @example    main.c
 *             Example of reading card UID using MFRC522_Check()
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BSP_RC522_H
#define __BSP_RC522_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>
#include <stdbool.h>
#include "driver_rc522.h"

/* Public defines ----------------------------------------------------- */

/**
 * @brief Hardware version selection for different board layouts.
 *        Currently, only version 1 is defined.
 */
#ifndef BSP_RFID_VERSION
#define BSP_RFID_VERSION    1    /**< Default hardware version */
#endif

#if (BSP_RFID_VERSION == 1)

/* === SPI Peripheral Configuration === */
#define BSP_RFID_SPI_INSTANCE          SPI2                /**< SPI peripheral instance (SPI1, SPI2, etc.) */
#define BSP_RFID_SPI_HANDLE            hspi2               /**< HAL SPI handle defined in bsp_rfid_rc522.c */
#define BSP_RFID_SPI_CLK_ENABLE()      __HAL_RCC_SPI2_CLK_ENABLE()   /**< SPI clock enable macro */
#define BSP_RFID_SPI_CLK_DISABLE()     __HAL_RCC_SPI2_CLK_DISABLE()  /**< SPI clock disable macro */

/* === SPI GPIO Pins (SCK, MISO, MOSI) Configuration === */
#define BSP_RFID_SPI_GPIO_PORT         GPIOB               /**< GPIO port for SPI pins (PB13, PB14, PB15) */
#define BSP_RFID_SPI_SCK_PIN           GPIO_PIN_13          /**< SPI SCK pin */
#define BSP_RFID_SPI_MISO_PIN          GPIO_PIN_14          /**< SPI MISO pin */
#define BSP_RFID_SPI_MOSI_PIN          GPIO_PIN_15          /**< SPI MOSI pin */
#define BSP_RFID_SPI_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()  /**< GPIO clock enable macro for SPI port */
// Note: Alternate Function for SPI2 on these pins is typically AF0 for F1 series, but handled by HAL_SPI_MspInit.

/* === Chip Select (CS/SS/SDA) Pin Configuration === */
#define BSP_RFID_CS_PORT               GPIOB              	/**< GPIO port for CS pin */
#define BSP_RFID_CS_PIN                GPIO_PIN_12        	/**< GPIO pin for CS */
#define BSP_RFID_CS_GPIO_CLK_ENABLE()  __HAL_RCC_GPIOB_CLK_ENABLE()  /**< GPIO clock enable macro for CS port */

/* === Reset (RST) Pin Configuration === */
#define BSP_RFID_RST_PORT              GPIOA              	/**< GPIO port for RST pin */
#define BSP_RFID_RST_PIN               GPIO_PIN_8         	/**< GPIO pin for RST */
#define BSP_RFID_RST_GPIO_CLK_ENABLE() __HAL_RCC_GPIOA_CLK_ENABLE()  /**< GPIO clock enable macro for RST port */

/* === Interrupt (IRQ) Pin Configuration === */
#define BSP_RFID_IRQ_PORT              GPIOB				/**< GPIO port for TRQ pin */
#define BSP_RFID_IRQ_PIN               GPIO_PIN_0			/**< GPIO pin for IRQ */
#define BSP_RFID_IRQ_GPIO_CLK_ENABLE() __HAL_RCC_GPIOB_CLK_ENABLE()	/**< GPIO clock enable macro for IRQ port */
#define BSP_RFID_IRQn                  EXTI0_IRQn			/**< External interrupt number for RFID IRQ pin (PB0 -> EXTI0) */

#endif /* BSP_RFID_VERSION == 1 */

/* Public enumerate/structure ----------------------------------------- */
/* Public macros ------------------------------------------------------ */
/* Public variables --------------------------------------------------- */
/* Public APIs ----------------------------------------- */

/**
 * @brief Initializes the BSP for the RFID module, including hardware and driver.
 * @note Must be called once before using other BSP functions.
 * @return rfid_status_t Operation status.
 */
rfid_status_t bsp_rfid_init(void);

/**
 * @brief Checks for a new card and retrieves its UID.
 * @note This function handles debouncing and ensures the same card isn't reported repeatedly.
 * @param[out] uid Pointer to a buffer to store the card's UID.
 * @return bool - true if a new card has been detected, false otherwise.
 */
bool bsp_rfid_check_for_new_card(uint8_t* uid);

#endif // __BSP_RC522_H

/* End of file -------------------------------------------------------- */

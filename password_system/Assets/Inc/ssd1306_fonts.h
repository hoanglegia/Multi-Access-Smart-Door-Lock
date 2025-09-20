/**
 * @file       ssd1306_fonts.h
 * @copyright  Copyright (C) 2019 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-18
 * @author     Tran Van Diep
 *             
 * @brief      Font Assets for SSD1306 OLED Display
 *             Font bitmap data and structures for text display
 * @note       Contains various font sizes and styles for SSD1306
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __SSD1306_FONTS_H
#define __SSD1306_FONTS_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
/* Font 6x8 defines */
#define FONT_6X8_WIDTH              (6)    /*!< Character width in pixels */
#define FONT_6X8_HEIGHT             (8)    /*!< Character height in pixels */
#define FONT_6X8_FIRST_CHAR         (32)   /*!< First printable character (space) */
#define FONT_6X8_LAST_CHAR          (126)  /*!< Last printable character (~) */
#define FONT_6X8_CHAR_COUNT         (FONT_6X8_LAST_CHAR - FONT_6X8_FIRST_CHAR + 1) /*!< Total character count */

/* Public enumerate/structure ----------------------------------------- */
/**
 * @brief Font structure for character display
 */
typedef struct 
{
  uint8_t width;                       /**< Character width in pixels */
  uint8_t height;                      /**< Character height in pixels */
  const uint8_t* data;                 /**< Font bitmap data */
}
font_t;

/* Public variables --------------------------------------------------- */
extern const font_t font_6x8; /**< Default 6x8 font */

/* Public function prototypes ----------------------------------------- */

#endif // __SSD1306_FONTS_H

/* End of file -------------------------------------------------------- */

/**
 * @file       asset_st7735_font.h
 * @copyright  Modified version by Hoang Le, 2025.
 * @license    
 * @version    1.0.0
 * @date       2025-17-9
 * @author     Original: mokhwasomssi
 * @modified   Hoang Le (2025-09-17)
 * @brief      Font data for ST7735
 * @note       None
 * @example    None
 */

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __ST7735_FONT_H
#define __ST7735_FONT_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ----------------------------------------------------------------- */
#include <stdint.h>

/* Public defines ----------------------------------------------------------- */
#define FONT_7X10
/* Public enumerate/structure ----------------------------------------------- */
typedef enum
{
  FONT_6x8 = 0,
  FONT_7x10,
  FONT_11x18,
  FONT_16x26
} font_size_t;

typedef struct
{
  font_size_t size;       /**< Font size */
  const uint16_t *data;   /**< Pointer to font data array */
  uint8_t width;          /**< Character width in pixels */
  uint8_t height;         /**< Character height in pixels */
} font_t;

/* Public macros ------------------------------------------------------------ */
/* Public variables --------------------------------------------------------- */
#ifdef FONT_6X8
extern font_t font_6x8;
#endif

#ifdef FONT_7X10
extern font_t font_7x10;
#endif

#ifdef FONT_11X18
extern font_t font_11x18;
#endif

#ifdef FONT_16X26
extern font_t font_16x26;
#endif
/* Public APIs -------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
#ifdef __cplusplus
} /* extern "C" { */
#endif

#endif /* __ST7735_FONT_H */

/* End of file -------------------------------------------------------------- */

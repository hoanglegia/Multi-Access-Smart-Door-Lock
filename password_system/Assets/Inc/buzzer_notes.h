/**
 * @file       buzzer_notes.h
 * @copyright  Copyright (C) 2025 ITRVN. All rights reserved.
 * @license    This project is released under the Fiot License.
 * @version    1.0.0
 * @date       2025-09-18
 * @author     Tran Diep
 *             
 * @brief      Musical notes frequency definitions for buzzer
 *             This file contains frequency definitions for musical notes
 *             from octave 3 to octave 6, commonly used for buzzer applications
 * @note       All frequencies are defined in Hz (Hertz)
 */

/* Define to prevent recursive inclusion ------------------------------ */
#ifndef __BUZZER_NOTES_H
#define __BUZZER_NOTES_H

/* Includes ----------------------------------------------------------- */
#include <stdint.h>

/* Public defines ----------------------------------------------------- */
/* Third Octave (Lower frequencies) */
#define NOTE_C3     131  /*!< Do - C3 frequency in Hz */
#define NOTE_CS3    139  /*!< Do# - C#3 frequency in Hz */
#define NOTE_D3     147  /*!< Re - D3 frequency in Hz */
#define NOTE_DS3    156  /*!< Re# - D#3 frequency in Hz */
#define NOTE_E3     165  /*!< Mi - E3 frequency in Hz */
#define NOTE_F3     175  /*!< Fa - F3 frequency in Hz */
#define NOTE_FS3    185  /*!< Fa# - F#3 frequency in Hz */
#define NOTE_G3     196  /*!< Sol - G3 frequency in Hz */
#define NOTE_GS3    208  /*!< Sol# - G#3 frequency in Hz */
#define NOTE_A3     220  /*!< La - A3 frequency in Hz */
#define NOTE_AS3    233  /*!< La# - A#3 frequency in Hz */
#define NOTE_B3     247  /*!< Si - B3 frequency in Hz */

/* Fourth Octave (Medium frequencies) */
#define NOTE_C4     262  /*!< Do - C4 frequency in Hz */
#define NOTE_CS4    277  /*!< Do# - C#4 frequency in Hz */
#define NOTE_D4     294  /*!< Re - D4 frequency in Hz */
#define NOTE_DS4    311  /*!< Re# - D#4 frequency in Hz */
#define NOTE_E4     330  /*!< Mi - E4 frequency in Hz */
#define NOTE_F4     349  /*!< Fa - F4 frequency in Hz */
#define NOTE_FS4    370  /*!< Fa# - F#4 frequency in Hz */
#define NOTE_G4     392  /*!< Sol - G4 frequency in Hz */
#define NOTE_GS4    415  /*!< Sol# - G#4 frequency in Hz */
#define NOTE_A4     440  /*!< La - A4 frequency in Hz (Concert pitch) */
#define NOTE_AS4    466  /*!< La# - A#4 frequency in Hz */
#define NOTE_B4     493  /*!< Si - B4 frequency in Hz */

/* Fifth Octave (Higher frequencies) */
#define NOTE_C5     523  /*!< Do - C5 frequency in Hz */
#define NOTE_CS5    554  /*!< Do# - C#5 frequency in Hz */
#define NOTE_D5     587  /*!< Re - D5 frequency in Hz */
#define NOTE_DS5    622  /*!< Re# - D#5 frequency in Hz */
#define NOTE_E5     659  /*!< Mi - E5 frequency in Hz */
#define NOTE_F5     698  /*!< Fa - F5 frequency in Hz */
#define NOTE_FS5    740  /*!< Fa# - F#5 frequency in Hz */
#define NOTE_G5     784  /*!< Sol - G5 frequency in Hz */
#define NOTE_GS5    831  /*!< Sol# - G#5 frequency in Hz */
#define NOTE_A5     880  /*!< La - A5 frequency in Hz */
#define NOTE_AS5    932  /*!< La# - A#5 frequency in Hz */
#define NOTE_B5     988  /*!< Si - B5 frequency in Hz */

/* Sixth Octave (Very high frequencies) */
#define NOTE_C6     1047 /*!< Do - C6 frequency in Hz */
#define NOTE_CS6    1109 /*!< Do# - C#6 frequency in Hz */
#define NOTE_D6     1175 /*!< Re - D6 frequency in Hz */
#define NOTE_DS6    1245 /*!< Re# - D#6 frequency in Hz */
#define NOTE_E6     1319 /*!< Mi - E6 frequency in Hz */
#define NOTE_F6     1397 /*!< Fa - F6 frequency in Hz */
#define NOTE_FS6    1480 /*!< Fa# - F#6 frequency in Hz */
#define NOTE_G6     1568 /*!< Sol - G6 frequency in Hz */
#define NOTE_GS6    1661 /*!< Sol# - G#6 frequency in Hz */
#define NOTE_A6     1760 /*!< La - A6 frequency in Hz */
#define NOTE_AS6    1865 /*!< La# - A#6 frequency in Hz */
#define NOTE_B6     1976 /*!< Si - B6 frequency in Hz */

/* Special notes */
#define NOTE_REST   0    /*!< Rest/Silence - No sound */

/* Common durations in milliseconds */
#define DURATION_WHOLE      2000 /*!< Whole note duration */
#define DURATION_HALF       1000 /*!< Half note duration */
#define DURATION_QUARTER    500  /*!< Quarter note duration */
#define DURATION_EIGHTH     250  /*!< Eighth note duration */
#define DURATION_SIXTEENTH  125  /*!< Sixteenth note duration */

/* Common effect frequencies */
#define FREQ_ALARM_HIGH     800  /*!< High frequency for alarm sound */
#define FREQ_ALARM_LOW      400  /*!< Low frequency for alarm sound */
#define FREQ_WARNING        700  /*!< Warning sound frequency */
#define FREQ_BEEP           1000 /*!< Standard beep frequency */
#define FREQ_ERROR          300  /*!< Error sound frequency */
#define FREQ_SUCCESS        600  /*!< Success sound frequency */

/* Human audible frequency range */
#define HUMAN_MIN_FREQ      20   /*!< Minimum audible frequency (Hz) */
#define HUMAN_MAX_FREQ      20000 /*!< Maximum audible frequency (Hz) */

/* Buzzer optimal frequency range */
#define BUZZER_MIN_FREQ     100  /*!< Minimum recommended buzzer frequency */
#define BUZZER_MAX_FREQ     5000 /*!< Maximum recommended buzzer frequency */

/* Public enumerate/structure ----------------------------------------- */

/* Public macros ------------------------------------------------------ */

/* Public variables --------------------------------------------------- */

/* Public function prototypes ----------------------------------------- */

#endif // __BUZZER_NOTES_H

/* End of file -------------------------------------------------------- */

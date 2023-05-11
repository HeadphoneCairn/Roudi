/*
 * Roudi, a ROUter for miDI messages for Blokas Midiboy
 * Copyright (C) 2019-2023 Headphone Cairn Software
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2 of the License.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "Roudi.h"   // for #define ENABLE_DEBUG
#include "Screen.h"

#ifdef ENABLE_DEBUG  // --------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <ArduboyTones.h>

namespace Debug
{
  // =============================================================================
  //                                P R I N T
  // =============================================================================

  void Print(const char * format, ...)
  {
    char buffer[64]; // Can overflow of course
    va_list args;
    va_start (args, format);
    vsprintf (buffer, format, args);
    va_end (args);
    Screen::Print(Screen::CanvasComplete, 0, 0, buffer, Screen::LineClear, Screen::inversion_none);
  }

  // =============================================================================
  //                                B E E P
  // =============================================================================

  bool enableSounds()
  {
    return true;
  }

  ArduboyTones make_a_sound(enableSounds);

  const uint16_t bing[] PROGMEM = {
    NOTE_C5  + TONE_HIGH_VOLUME, 250, 
    TONES_END
  };

  void Beep()
  {
    make_a_sound.tones(bing);
  }

  const uint16_t binglow[] PROGMEM = {
    NOTE_C3  + TONE_HIGH_VOLUME, 250, 
    TONES_END
  };

  void BeepLow()
  {
    make_a_sound.tones(binglow);
  }

  const uint16_t binghigh[] PROGMEM = {
    NOTE_C7  + TONE_HIGH_VOLUME, 250, 
    TONES_END
  };

  void BeepHigh()
  {
    make_a_sound.tones(binghigh);
  }


}

#else // -----------------------------------------------------------------------

namespace Debug
{
  void Print(const char * format, ...) {}
  void Beep() {}  
  void BeepLow() {}  
  void BeepHigh() {}
}

#endif // ----------------------------------------------------------------------
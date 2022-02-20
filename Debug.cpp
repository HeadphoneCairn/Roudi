#include "Screen.h"

#define ENABLE_DEBUG // saves about 1000 bytes of programn space
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
#pragma once

namespace Debug
{
  void Print(const char * format, ...);   // requires 220 bytes extra program space
  void Beep();                            // requires 737 bytes extra program space
  void BeepLow();
  void BeepHigh();
}
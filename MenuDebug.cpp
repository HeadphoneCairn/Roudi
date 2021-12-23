#include "MenuDebug.h"


#include "Data.h"
#include "MemoryFree.h"
#include "MenuSingle.h"
#include "MenuSplit.h"
#include "MenuSettings.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

PSTRING(PSTR_menu_debug, " DEBUG "); 

PSTRING(PSTR_debug_memory, "free memory: %d");
PSTRING(PSTR_debug_sizeof, "size menus: %d %d %d %d");


MenuDebug::MenuDebug(): Menu()
{
  SetNumberOfLines(7);
}

const char* MenuDebug::GetTitle()
{
  return GetPString(PSTR_menu_debug);
}

void MenuDebug::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  if (line == 0) {
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_debug_memory), freeMemory());
  } else if (line == 1) {
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_debug_sizeof), sizeof(MenuSingle), sizeof(MenuSplit), sizeof(MenuSettings), sizeof(MenuDebug));
  } else {
    Screen::buffer[0] = 0;
  }
  text = Screen::buffer;
  inversion = {Screen::InvertAll, 0, 0};
}

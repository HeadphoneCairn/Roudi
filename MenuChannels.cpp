#include "MenuChannels.h"

#include "Debug.h"
#include "Data.h"
#include "Menus.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace
{
  uint8_t g_selected_line = 0;
  uint8_t g_first_line = 0; 
}

PSTRING(PSTR_menu_channels, " OUTPUT CHANNELS "); 

PSTRING(PSTR_channel_format, "ch%02d: %s");


MenuChannels::MenuChannels(): Menu()
{
  SetNumberOfLines(EE::GetNumberOfChannels() + 1, g_selected_line, g_first_line);
}

const char* MenuChannels::GetTitle()
{
  return GetPString(PSTR_menu_channels);
}

void MenuChannels::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  if (line < EE::GetNumberOfChannels()) {
    const char* channel_name = EE::GetChannelName(line);
    sprintf(Screen::buffer, GetPString(PSTR_channel_format), line + 1, channel_name);
    text = Screen::buffer;
    inversion = { Screen::InvertGiven, 6, 6 + strlen(channel_name) - 1 };
  } else {
    text = GetPStringAccept();
    inversion = Screen::inversion_all;
  }
}

bool MenuChannels::OnLine(LineAction action, uint8_t line)
{
  if (action == SELECTED)
    return false;

  if (line < EE::GetNumberOfChannels()) {
    Menus::SetNextMenu(MENU_NAME_CHANNEL, line);
    return false;    
  } else {
    Menus::SetNextMenu(MENU_SETTINGS);
    return false;
  }
}

void MenuChannels::OnStop(uint8_t selected_line, uint8_t first_line)
{
  if (selected_line < EE::GetNumberOfChannels()) {
    g_selected_line = selected_line;
    g_first_line = first_line;
  } else // Accept
    g_selected_line = g_first_line = 0;
}
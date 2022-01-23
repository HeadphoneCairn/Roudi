#include "PageChannels.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

PSTRING(PSTR_page_channels, " OUTPUT CHANNELS "); 
PSTRING(PSTR_accept, "> Accept");

PSTRING(PSTR_channel_format, "ch%02d: %s");


PageChannels::PageChannels(): Page()
{
}

void PageChannels::OnStart()
{
  SetNumberOfLines(NumberOfChannels + 1); // TODO: set first line and seletction to zero if coming from settings dialog 
}

void PageChannels::OnStop(uint8_t selected_line, uint8_t first_line)
{

//  if (selected_line = NumberOfChannels)
//    g_selected_line = g_first_line = 0;

}

const char* PageChannels::GetTitle()
{
  return GetPString(PSTR_page_channels);
}

Page::LineResult PageChannels::Line(LineFunction func, uint8_t line, uint8_t field)
{
  const char* text = nullptr;
  Screen::Inversion inversion = Screen::inversion_all;

  if (func == GET_TEXT) {
    if (line < NumberOfChannels) {
      text = Screen::buffer;
      const char* channel_name = EE::GetChannelName(line);
      sprintf(Screen::buffer, GetPString(PSTR_channel_format), line + 1, channel_name);
      inversion = { Screen::InvertGiven, 6, static_cast<uint8_t>(6 + strlen(channel_name) - 1)};
    } else {
      text = GetPString(PSTR_accept);
    }  
  } else if (func == DO_LEFT || func == DO_RIGHT) {
    if (line < NumberOfChannels) {
      Debug::Beep();
      //Menus::SetNextMenu(MENU_NAME_CHANNEL, line);
    } else {
      Debug::BeepLow();
      //Menus::SetNextMenu(MENU_SETTINGS);
    }
  }
  
  return {1, text, inversion, false};
}

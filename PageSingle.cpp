#include "PageSingle.h"

#include "Debug.h"
#include "Data.h"
#include "Roudi.h"

namespace
{
  uint8_t g_first_line = 0; 
  uint8_t g_selected_line = 0;
}

PSTRING(PSTR_page_single, " SINGLE "); 

PageSingle::PageSingle(): Page()
{
}

void PageSingle::OnStart()
{
  SetNumberOfLines(17, g_selected_line, g_first_line);
  SetMidiConfiguration(g_selected_line);
}

const char* PageSingle::GetTitle()
{
  return GetPString(PSTR_page_single);
}

Page::LineResult PageSingle::Line(LineFunction func, uint8_t line, uint8_t field)
{
  const char* text = nullptr;
  if (func == GET_TEXT) {
    text = (line < 16) ? GetChannelNameBrol(line) : GetPStringNone();
  } else if (func == DO_SELECTED) {
    SetMidiConfiguration(line);
  }
  
  return {1, text, Screen::inversion_all, false};
}


void PageSingle::OnStop(uint8_t selected_line, uint8_t first_line)
{
  g_first_line = first_line;
  //g_selected_channel = selected_line < m_number_of_channels ? EE::ChannelIndexToChannelValue(selected_line) : selected_line;
}

void PageSingle::SetMidiConfiguration(uint8_t selected_line)
{
  g_next_midi_config.config.SetDefaults();
  if (selected_line < 16) {
    g_next_midi_config.config.m_nbr_output_channels = 1;
    g_next_midi_config.config.m_output_channel[0].m_channel = EE::ChannelIndexToChannelValue(selected_line);
    Debug::Beep();
  } else {
    g_next_midi_config.config.m_nbr_output_channels = 0;
  }
  g_next_midi_config.go = true;
}

#include "MenuSingle.h"

#include "Debug.h"
#include "Data.h"
#include "Roudi.h"

namespace
{
  uint8_t g_first_line = 0; 
  uint8_t g_selected_channel = EE::ChannelIndexToChannelValue(0);
}

PSTRING(PSTR_menu_single, " SINGLE "); 

MenuSingle::MenuSingle(): Menu(), m_number_of_channels(EE::GetNumberOfChannels())
{
  const uint8_t selected_line = g_selected_channel < m_number_of_channels ? EE::ChannelValueToChannelIndex(g_selected_channel) : g_selected_channel;
  SetNumberOfLines(m_number_of_channels + 1, selected_line, g_first_line);
  SetMidiConfiguration(selected_line);
}

const char* MenuSingle::GetTitle()
{
  return GetPString(PSTR_menu_single);
}

void MenuSingle::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  inversion = Screen::inversion_all;
  if (line < m_number_of_channels) {
    text = EE::GetChannelNameFormatted(line);
  } else {
    text = GetPStringNone();
  }
}

bool MenuSingle::OnLine(LineAction action, uint8_t line)
{
  if (action == SELECTED) {
    SetMidiConfiguration(line);
    return false;
  }
  // TODO If left or right go into midi monitor
  return false;
}

void MenuSingle::OnStop(uint8_t selected_line, uint8_t first_line)
{
  g_first_line = first_line;
  g_selected_channel = selected_line < m_number_of_channels ? EE::ChannelIndexToChannelValue(selected_line) : selected_line;
}

void MenuSingle::SetMidiConfiguration(uint8_t selected_line)
{
  g_next_midi_config.config.SetDefaults();
  if (selected_line < m_number_of_channels) {
    g_next_midi_config.config.m_nbr_output_channels = 1;
    g_next_midi_config.config.m_output_channel[0].m_channel = EE::ChannelIndexToChannelValue(selected_line);
  } else {
    g_next_midi_config.config.m_nbr_output_channels = 0;
  }
  g_next_midi_config.go = true;
}

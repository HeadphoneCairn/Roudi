#include "MenuLayer.h"

#include "Data.h"
#include "Roudi.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

namespace 
{
  bool g_first_run = true;
  uint8_t g_selected_line = 0;
  uint8_t g_first_line = 0; 

  struct LayerValues g_values;
  void gValues_ChannelIndexToChannelValue()
  {
    g_values.a_channel = EE::ChannelIndexToChannelValue(g_values.a_channel);
    g_values.b_channel = EE::ChannelIndexToChannelValue(g_values.b_channel);
  }
  void gValues_ChannelValueToChannelIndex()
  {
    g_values.a_channel = EE::ChannelValueToChannelIndex(g_values.a_channel);
    g_values.b_channel = EE::ChannelValueToChannelIndex(g_values.b_channel);
  }
}

PSTRING(PSTR_menu_layer, " LAYER "); 

PSTRING(PSTR_a,        "A: ");
PSTRING(PSTR_b,        "B: ");
PSTRING(PSTR_velocity, "   when velocity > ");

PSTRING(PSTR_percentage_format, "%d%%");

const char* GetPercentage(uint8_t selected_value)
{
  sprintf(data_scratch, GetPString(PSTR_percentage_format), selected_value*10);
  return data_scratch;
}

void line_a_channel(ParsPars& pars)
  {
  pars.types = TypePString|TypeFunction;
  pars.name = (void*) PSTR_a;
  pars.number_of_values = EE::GetNumberOfChannels(); 
  pars.values = (void*) EE::GetChannelNameFormatted; 
  }

void line_velocity(ParsPars& pars)
  {
  pars.types = TypePString|TypeFunction; 
  pars.name = (void*) PSTR_velocity;
  pars.number_of_values = 10;
  pars.values = (void*) GetPercentage;
  }

void line_b_channel(ParsPars& pars)
  {
  pars.types = TypePString|TypeFunction;
  pars.name = (void*) PSTR_b;
  pars.number_of_values = EE::GetNumberOfChannels(); 
  pars.values = (void*) EE::GetChannelNameFormatted; 
  }


/*
·A: 01. Piano          ·
·   always on          ·
·B: 12. iPad           ·
·   when velocity > 50 ·
·
*/

MenuLayer::MenuLayer(): 
  Menu()
{
  if (g_first_run) {
    g_first_run = false;
    EE::GetLayer(0, g_values);
  }
  gValues_ChannelValueToChannelIndex();
  m_lines[0].Init(line_a_channel,  &g_values.a_channel);
  m_lines[1].Init(line_velocity,   &g_values.a_velocity);
  m_lines[2].Init(line_b_channel,  &g_values.b_channel);
  m_lines[3].Init(line_velocity,   &g_values.b_velocity);
  SetNumberOfLines(m_number_of_combilines + 8, g_selected_line, g_first_line);
}

const char* MenuLayer::GetTitle()
{
  return GetPString(PSTR_menu_layer);
}


void MenuLayer::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  if (line < m_number_of_combilines) {
    text = m_lines[line].GetText();
    inversion = m_lines[line].GetInversion();
  } else if (line >= m_number_of_combilines + 1 && line <= m_number_of_combilines + 3) {
    text = GetPStringLoadPreset(line - m_number_of_combilines);
    inversion = {Screen::InvertAll, 0, 0};
  } else if (line >= m_number_of_combilines + 4 && line <= m_number_of_combilines + 6) {
    text = GetPStringSavePreset(line - (m_number_of_combilines + 3));
    inversion = {Screen::InvertAll, 0, 0};
  } else if (line == m_number_of_combilines + 7) {
    text = GetPStringMonitor();
    inversion = {Screen::InvertAll, 0, 0};
  } else {
    text = GetPStringEmpty();
    inversion = {Screen::InvertGiven, 0, 0};
  }
}

bool MenuLayer::OnLine(LineAction action, uint8_t line)
{
  bool redraw = ActualOnLine(action, line);
  if (redraw) // A setting has changed, so we update the midi configuration
    SetMidiConfiguration();
  return redraw;
}

bool MenuLayer::ActualOnLine(LineAction action, uint8_t line)
{
  if (action == SELECTED)
    return false;
  
  if (line < m_number_of_combilines) {
    return action == LEFT ? m_lines[line].OnLeft() : m_lines[line].OnRight();
  } else if (line >= m_number_of_combilines + 1 && line <= m_number_of_combilines + 3) {
    EE::GetLayer(line - m_number_of_combilines, g_values);
    gValues_ChannelValueToChannelIndex();
    return true;
  } else if (line >= m_number_of_combilines + 4 && line <= m_number_of_combilines + 6) {
    gValues_ChannelIndexToChannelValue();
    EE::SetLayer(line - (m_number_of_combilines + 3), g_values);
    gValues_ChannelValueToChannelIndex();
    return false;
  } else
    return false;
}

void MenuLayer::OnStop(uint8_t selected_line, uint8_t first_line)
{
  gValues_ChannelIndexToChannelValue();
  g_selected_line = selected_line;
  g_first_line = first_line;
}

void MenuLayer::SetMidiConfiguration()
{
  g_next_midi_config.config.SetDefaults();
//g_next_midi_config.config.m_input_channel = 0; // TODO ??????
  g_next_midi_config.config.m_nbr_output_channels = 2;
  g_next_midi_config.config.m_output_channel[0].m_channel = EE::ChannelIndexToChannelValue(g_values.a_channel);
  g_next_midi_config.config.m_output_channel[0].m_minimum_velocity = g_values.a_velocity * 13;
  g_next_midi_config.config.m_output_channel[1].m_channel = EE::ChannelIndexToChannelValue(g_values.b_channel);
  g_next_midi_config.config.m_output_channel[1].m_minimum_velocity = g_values.b_velocity * 13;
  g_next_midi_config.go = true;
}

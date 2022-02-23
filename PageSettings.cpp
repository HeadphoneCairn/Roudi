#include "PageSettings.h"

#include "Data.h"
#include "Debug.h"
#include "MidiFilter.h"
#include "MidiProcessing.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace {

  PSTRING(PSTR_page_settings,   " SETTINGS ");

  PSTRING(PSTR_input_channel,   "Input channel");
  PSTRING(PSTR_block_other,     "Block other channels");
  PSTRING(PSTR_velocity_curve,  "Velocity curve");
  PSTRING(PSTR_brightness,      "Screen brightness");

  PSTRING(PSTR_ellipsis_0,        "...");
  PTABLE(PTAB_ellipsis, PSTR_ellipsis_0);

  PSTRING(PSTR_block_other_0, "no");
  PSTRING(PSTR_block_other_1, "yes");

  PSTRING(PSTR_velocity_0, "soft");
  PSTRING(PSTR_velocity_1, "medium");
  PSTRING(PSTR_velocity_2, "hard");
  PTABLE(PTAB_velocity, PSTR_velocity_0, PSTR_velocity_1, PSTR_velocity_2);
  const char* GetVelocityCurve(uint8_t i_value, uint8_t& o_number_of_values)
  {
    return GetPTable(i_value, o_number_of_values, PTAB_velocity, PTAB_velocity_size);
  }



  const char* GetBrightness(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = 10;
    return GetNumberPlusOne(i_value);
  }

  const char* GetInputChannel(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = NumberOfChannels;
    return GetNumberPlusOne(i_value);
  }


  PSTRING(PSTR_monitor_fltr_0, "block");
  PSTRING(PSTR_monitor_fltr_1, "pass");

}

uint8_t PageSettings::m_selected_line = 0;
uint8_t PageSettings::m_first_line = 0;

PageSettings::PageSettings(): 
  Page()
  {}

void PageSettings::OnStart(uint8_t)
{
  SettingsValues& values = EE::SettingsRW();
  SetNumberOfLines(16, m_selected_line, 0, m_first_line);
}

void PageSettings::OnStop() 
{  
  EE::SetSettings();
  m_selected_line = GetSelectedLine();
  m_first_line = GetFirstLine();
}

const char* PageSettings::GetTitle()
{
  return GetPString(PSTR_page_settings);
}

Page::LineResult PageSettings::Line(LineFunction func, uint8_t line, uint8_t field)
{
  SettingsValues& settings = EE::SettingsRW();
  switch (line)
  {
    case  0: return LineInputChannel(func);
    case  1: return BoolLine(func, PSTR_block_other, settings.block_other, PSTR_block_other_0, PSTR_block_other_1);
    case  2: return SingleLine(func, PSTR_velocity_curve, settings.velocity_curve, GetVelocityCurve);
    case  3: return SingleLine(func, PSTR_brightness, settings.brightness, GetBrightness);
    case  4: return TextLine(func, PSTR_filter_messages);
    case  5: {
             Page::LineResult result = LineFilter(func, PSTR_filter_note_on_off, settings.filter.note_off);
             settings.filter.note_on = settings.filter.note_off; // on and off should be filter together
             return result; 
             }
    case  6: return LineFilter(func, PSTR_filter_pitch_bend,        settings.filter.pitch_bend       );
    case  7: return LineFilter(func, PSTR_filter_channel_pressure,  settings.filter.channel_pressure );
    case  8: return LineFilter(func, PSTR_filter_key_pressure,      settings.filter.key_pressure     );
    case  9: return LineFilter(func, PSTR_filter_program_change,    settings.filter.program_change   );
    case 10: return LineFilter(func, PSTR_filter_control_change,    settings.filter.control_change   );
    case 11: return LineFilter(func, PSTR_filter_time_sync,         settings.filter.time_sync        );
    case 12: return LineFilter(func, PSTR_filter_transport,         settings.filter.transport        ); 
    case 13: return LineFilter(func, PSTR_filter_system_exclusive,  settings.filter.system_exclusive );
    case 14: return LineFilter(func, PSTR_filter_active_sensing,    settings.filter.active_sensing   );
    case 15: return LineFilter(func, PSTR_filter_other,             settings.filter.other            );
    default: return DefaultLine(func);
  }
}

Page::LineResult PageSettings::LineInputChannel(LineFunction func)
{
  SettingsValues& settings = EE::SettingsRW();
  LineResult result = SingleLine(func, PSTR_input_channel, settings.input_channel, GetInputChannel);
  if (result.redraw) { // input channel has changed
    Debug::BeepHigh();
    MidiProcessing::Configuration next_config = MidiProcessing::GetConfiguration();
    next_config.m_input_channel = settings.input_channel;
    MidiProcessing::SetNextConfiguration(next_config);
  }
  return result; 
}

Page::LineResult PageSettings::LineFilter(Page::LineFunction func, const char* name, uint8_t& value)
{
  return BoolLine(func, name, value, PSTR_monitor_fltr_0, PSTR_monitor_fltr_1);
}

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



  // 3 bytes per line is reached by using function to store parameters
  // instead of storing them in memory. Looks horrible, but uses less
  // memory.
  // Can't do pars = { ... } because it will use more memory again!

  void line_input_channel(ParsPars& pars)
  {
    pars.types = TypePString|TypeFunction;
    pars.name = (void*) PSTR_input_channel;
    pars.number_of_values = 16;
    pars.values = (void*) GetNumberPlusOne;
  }

  void line_velocity_curve(ParsPars& pars)
  {
    pars.types = TypePString|TypePTable;
    pars.name = (void*) PSTR_velocity_curve;
    pars.number_of_values = PTAB_velocity_size;
    pars.values = (void*) PTAB_velocity;
  }

  void line_brightness(ParsPars& pars)
  {
    pars.types = TypePString|TypeFunction;
    pars.name = (void*) PSTR_brightness;
    pars.number_of_values = 10;
    pars.values = (void*) GetNumberPlusOne;
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
  m_ui_input_channel.Init(line_input_channel, &values.input_channel);
  m_ui_velocity_curve.Init(line_velocity_curve, &values.velocity_curve);
  m_ui_brightness.Init(line_brightness, &values.brightness);
  SetNumberOfLines(15, m_selected_line, 0, m_first_line);
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
    case  2: return SingleCombiLine(func, m_ui_velocity_curve,  Screen::MaxCharsCanvas, 0, true);
    case  3: return SingleCombiLine(func, m_ui_brightness,     Screen::MaxCharsCanvas, 0, true);
    case  4: return LineMessageFilter(func);
    case  5: return LineFilter(func, PSTR_filter_key_pressure,      settings.filter.key_pressure     );
    case  6: return LineFilter(func, PSTR_filter_control_change,    settings.filter.control_change   );
    case  7: return LineFilter(func, PSTR_filter_program_change,    settings.filter.program_change   );
    case  8: return LineFilter(func, PSTR_filter_channel_pressure,  settings.filter.channel_pressure );
    case  9: return LineFilter(func, PSTR_filter_pitch_bend,        settings.filter.pitch_bend       );
    case 10: return LineFilter(func, PSTR_filter_system_exclusive,  settings.filter.system_exclusive );
    case 11: return LineFilter(func, PSTR_filter_time_sync,         settings.filter.time_sync        );
    case 12: return LineFilter(func, PSTR_filter_transport,         settings.filter.transport        ); 
    case 13: return LineFilter(func, PSTR_filter_active_sensing,    settings.filter.active_sensing   );
    case 14: return LineFilter(func, PSTR_filter_other,             settings.filter.other            );
    default: return DefaultLine(func);
  }
}

Page::LineResult PageSettings::LineInputChannel(LineFunction func)
{
  LineResult result = SingleCombiLine(func, m_ui_input_channel, Screen::MaxCharsCanvas, 0, true);
  if (result.redraw) { // input channel has changed
    Debug::BeepHigh();
    MidiProcessing::Configuration next_config = MidiProcessing::GetConfiguration();
    next_config.m_input_channel = m_ui_input_channel.GetSelectedValue();
    MidiProcessing::SetNextConfiguration(next_config);
  }
  return result; 
}

Page::LineResult PageSettings::LineFilter(Page::LineFunction func, const char* name, uint8_t& value)
{
  return BoolLine(func, name, value, PSTR_monitor_fltr_0, PSTR_monitor_fltr_1);
}

Page::LineResult PageSettings::LineMessageFilter(Page::LineFunction func)
{
  SettingsValues& settings = EE::SettingsRW();
  Page::LineResult result = MessageFilterLine(func, settings.filter);
  settings.filter.note_on = settings.filter.note_off = 1; // Notes can never be filtered.
  return result;  
}


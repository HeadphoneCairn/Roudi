#include "PageSettings.h"

#include "Data.h"
#include "Debug.h"
#include "MidiFilter.h"
#include "MidiProcessing.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*

·[MIDI Settings]       ·
·Input channel       01·
·Velocity        normal·      normal|custom|none  (none = velocity 64)
·Program change   block·      block|allow   (includes bank select)
·Screen brightness     ·

Future:
·Custom Velocity   MENU·
·CC Mapping        MENU

*/


namespace {
  PSTRING(PSTR_page_settings,   " SETTINGS ");

  PSTRING(PSTR_input_channel,   "Input channel");
  PSTRING(PSTR_velocity_curve,  "Velocity curve");
  PSTRING(PSTR_program_change,  "Program change");
  PSTRING(PSTR_brightness,      "Screen brightness");

  PSTRING(PSTR_ellipsis_0,        "...");
  PTABLE(PTAB_ellipsis, PSTR_ellipsis_0);


  PSTRING(PSTR_velocity_0, "soft");
  PSTRING(PSTR_velocity_1, "medium");
  PSTRING(PSTR_velocity_2, "hard");
  PTABLE(PTAB_velocity, PSTR_velocity_0, PSTR_velocity_1, PSTR_velocity_2);

  PSTRING(PSTR_progchange_0, "block");
  PSTRING(PSTR_progchange_1, "allow");
  PTABLE(PTAB_progchange, PSTR_progchange_0, PSTR_progchange_1);



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

  void line_program_change(ParsPars& pars)
  {
    pars.types = TypePString|TypePTable;
    pars.name = (void*) PSTR_program_change;
    pars.number_of_values = PTAB_progchange_size;
    pars.values = (void*) PTAB_progchange;
  }

  void line_brightness(ParsPars& pars)
  {
    pars.types = TypePString|TypeFunction;
    pars.name = (void*) PSTR_brightness;
    pars.number_of_values = 10;
    pars.values = (void*) GetNumberPlusOne;
  }

  PSTRING(PSTR_monitor_fltr_0, "remove");
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
  m_ui_program_change.Init(line_program_change, &values.program_change);
  m_ui_brightness.Init(line_brightness, &values.brightness);
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


FilterSettingsValues fs;

Page::LineResult PageSettings::Line(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case  0: return LineInputChannel(func);
    case  1: return SingleCombiLine(func, m_ui_velocity_curve,  Screen::MaxCharsCanvas, 0, true);
    case  2: return SingleCombiLine(func, m_ui_program_change,  Screen::MaxCharsCanvas, 0, true);
    case  3: return SingleCombiLine(func, m_ui_brightness,     Screen::MaxCharsCanvas, 0, true);
    case  4: return MessageFilterLine(func, fs);
    case  5: return LineFilter(func, PSTR_filter_note_on_off,       fs.note_off         );
    case  6: return LineFilter(func, PSTR_filter_key_pressure,      fs.key_pressure     );
    case  7: return LineFilter(func, PSTR_filter_control_change,    fs.control_change   );
    case  8: return LineFilter(func, PSTR_filter_program_change,    fs.program_change   );
    case  9: return LineFilter(func, PSTR_filter_channel_pressure,  fs.channel_pressure );
    case 10: return LineFilter(func, PSTR_filter_pitch_bend,        fs.pitch_bend       );
    case 11: return LineFilter(func, PSTR_filter_system_exclusive,  fs.system_exclusive );
    case 12: return LineFilter(func, PSTR_filter_time_sync,         fs.time_sync        );
    case 13: return LineFilter(func, PSTR_filter_transport,         fs.transport        ); 
    case 14: return LineFilter(func, PSTR_filter_active_sensing,    fs.active_sensing   );
    case 15: return LineFilter(func, PSTR_filter_other,             fs.other            );
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

bool PageSettings::ShowChannelMenu()
{
//  Menus::SetNextMenu(MENU_CHANNELS);
  return false;
}

Page::LineResult PageSettings::LineFilter(Page::LineFunction func, const char* name, uint8_t& value)
{
  return BoolLine(func, name, value, PSTR_monitor_fltr_0, PSTR_monitor_fltr_1);
}



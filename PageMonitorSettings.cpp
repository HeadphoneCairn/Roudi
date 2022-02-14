#if 0
#include "PageMonitorSettings.h"

#include "Data.h"
#include "Debug.h"

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


/*
Options:
  Show: in, out, in+out
  Show: only from our input channel and its output channels
  Filter: sysex, ...
*/



PSTRING(PSTR_page_monitor_settings,   " MONITOR SETTINGS ");



PSTRING(PSTR_FILTER_NOTE_OFF,           "Note off");
PSTRING(PSTR_FILTER_NOTE_ON,            "Note on" );
PSTRING(PSTR_FILTER_KEY_PRESSURE,       "Key pressure");
PSTRING(PSTR_FILTER_CONTROL_CHANGE,     "Control change");
PSTRING(PSTR_FILTER_PROGRAM_CHANGE,     "Program change");
PSTRING(PSTR_FILTER_CHANNEL_PRESSURE,   "Channel pressure");
PSTRING(PSTR_FILTER_PITCH_BEND,         "Pitch bend");
PSTRING(PSTR_FILTER_SYSTEM_EXCLUSIVE,   "System exclusive");
PSTRING(PSTR_FILTER_TIME_SYNC,          "Time sync");
PSTRING(PSTR_FILTER_TRANSPORT,          "Transport");
PSTRING(PSTR_FILTER_ACTIVE_SENSING,     "Active sensing");
PSTRING(PSTR_FILTER_OTHER,              "Other");


struct filter_settings {
  uint8_t note_off;
  uint8_t note_on;
  uint8_t key_pressure;
  uint8_t control_change;
  uint8_t program_change;
  uint8_t channel_pressure;
  uint8_t pitch_bend;
  uint8_t system_exclusive;
  uint8_t time_sync;
  uint8_t transport;
  uint8_t active_sensing;
  uint8_t other;
};
filter_settings g_filter_settings;

// Pffff not sure what to do with CombiLine: should I include the pstring in the function,
// or add it to the class. Also, should I use bitfields for the filters to save memory???


PSTRING(PSTR_monitor_filter_0, "hide");
PSTRING(PSTR_monitor_filter_1, "show");
PTABLE(PTAB_monitor_filter, PSTR_monitor_filter_0, PSTR_monitor_filter_1);


static void line_monitor_filter_(ParsPars& pars)
{
  pars.types = TypePTable;
  pars.number_of_values = PTAB_monitor_filter_size;
  pars.values = (void*) PTAB_progchange;
}




PageMonitorSettings::PageMonitorSettings(): 
  Page()
  {}

void PageMonitorSettings::OnStart(uint8_t)
{
  SettingsValues& values = EE::SettingsRW();
  m_ui_input_channel.Init(line_input_channel, &values.input_channel);
  m_ui_velocity_curve.Init(line_velocity_curve, &values.velocity_curve);
  m_ui_program_change.Init(line_program_change, &values.program_change);
  m_ui_brightness.Init(line_brightness, &values.brightness);
  SetNumberOfLines(4);
}

void PageMonitorSettings::OnStop() 
{  
  EE::SetSettings();
}

const char* PageMonitorSettings::GetTitle()
{
  return GetPString(PSTR_page_monitor_settings);
}

Page::LineResult PageMonitorSettings::Line(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case 0: return LineInputChannel(func);
    case 1: return SingleCombiLine(func, m_ui_velocity_curve,  Screen::MaxCharsCanvas, 0, true);
    case 2: return SingleCombiLine(func, m_ui_program_change,  Screen::MaxCharsCanvas, 0, true);
    case 3: return SingleCombiLine(func, m_ui_brightness,     Screen::MaxCharsCanvas, 0, true);
    default: return DefaultLine(func);
  }
}

Page::LineResult PageMonitorSettings::LineInputChannel(LineFunction func)
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

bool PageMonitorSettings::ShowChannelMenu()
{
//  Menus::SetNextMenu(MENU_CHANNELS);
  return false;
}

#endif
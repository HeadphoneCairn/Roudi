#include "PageMonitorSettings.h"

#include "Data.h"
#include "Debug.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


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




PageMonitorSettings::PageMonitorSettings(): 
  Page()
  {}

void PageMonitorSettings::OnStart(uint8_t)
{
/*
  SettingsValues& values = EE::SettingsRW();
  m_ui_input_channel.Init(line_input_channel, &values.input_channel);
  m_ui_velocity_curve.Init(line_velocity_curve, &values.velocity_curve);
  m_ui_program_change.Init(line_program_change, &values.program_change);
  m_ui_brightness.Init(line_brightness, &values.brightness);
*/
  SetNumberOfLines(12);
}

void PageMonitorSettings::OnStop() 
{  
//  EE::SetSettings();
}

const char* PageMonitorSettings::GetTitle()
{
  return GetPString(PSTR_page_monitor_settings);
}

Page::LineResult BoolLine(Page::LineFunction func, const char* name, uint8_t& value, const char* true_value, const char* false_value)
{
  char* text = Screen::buffer;
  text[0] = 0;
  bool redraw = false;
  Screen::Inversion inversion = Screen::inversion_all;
  if (func == Page::GET_TEXT) {
    strncat(text, GetPString(name), Screen::buffer_len); // always terminates with a zero!, unlike strncpy
    const uint8_t name_len = strlen(text);
    const char* tf_value = GetPString(value ? true_value : false_value);
    const uint8_t tf_value_len = strlen(tf_value);
    if (name_len + tf_value_len <= Screen::buffer_len) { // safety precaution
      PadRight(text, Screen::buffer_len - (name_len + tf_value_len));
      strcat(text, tf_value);
      inversion = { Screen::InvertGiven, static_cast<uint8_t>(Screen::buffer_len - tf_value_len), Screen::buffer_len };
    }
  } else if (func == Page::DO_LEFT || func == Page::DO_RIGHT) {
    value = !value;
    redraw = true;
  }
  return { 1, text, inversion, redraw };
}

Page::LineResult PageMonitorSettings::Line(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case  0: return BoolLine(func, PSTR_FILTER_NOTE_OFF,          g_filter_settings.note_off,        PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  1: return BoolLine(func, PSTR_FILTER_NOTE_ON,           g_filter_settings.note_on,         PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  2: return BoolLine(func, PSTR_FILTER_KEY_PRESSURE,      g_filter_settings.key_pressure,    PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  3: return BoolLine(func, PSTR_FILTER_CONTROL_CHANGE,    g_filter_settings.control_change,  PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  4: return BoolLine(func, PSTR_FILTER_PROGRAM_CHANGE,    g_filter_settings.program_change,  PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  5: return BoolLine(func, PSTR_FILTER_CHANNEL_PRESSURE,  g_filter_settings.channel_pressure,PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  6: return BoolLine(func, PSTR_FILTER_PITCH_BEND,        g_filter_settings.pitch_bend,      PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  7: return BoolLine(func, PSTR_FILTER_SYSTEM_EXCLUSIVE,  g_filter_settings.system_exclusive,PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  8: return BoolLine(func, PSTR_FILTER_TIME_SYNC,         g_filter_settings.time_sync,       PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case  9: return BoolLine(func, PSTR_FILTER_TRANSPORT,         g_filter_settings.transport,       PSTR_monitor_filter_0, PSTR_monitor_filter_1); 
    case 10: return BoolLine(func, PSTR_FILTER_ACTIVE_SENSING,    g_filter_settings.active_sensing,  PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    case 11: return BoolLine(func, PSTR_FILTER_OTHER,             g_filter_settings.other,           PSTR_monitor_filter_0, PSTR_monitor_filter_1);
    default: return DefaultLine(func);
  }
}




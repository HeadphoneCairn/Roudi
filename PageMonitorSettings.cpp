#include "PageMonitorSettings.h"

#include "Data.h"
#include "Debug.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>



PSTRING(PSTR_page_monitor_settings,   " MONITOR SETTINGS ");


PSTRING(PSTR_FILTER_NOTE_OFF,           "- Note off");
PSTRING(PSTR_FILTER_NOTE_ON,            "- Note on" );
PSTRING(PSTR_FILTER_KEY_PRESSURE,       "- Key pressure");
PSTRING(PSTR_FILTER_CONTROL_CHANGE,     "- Control change");
PSTRING(PSTR_FILTER_PROGRAM_CHANGE,     "- Program change");
PSTRING(PSTR_FILTER_CHANNEL_PRESSURE,   "- Channel pressure");
PSTRING(PSTR_FILTER_PITCH_BEND,         "- Pitch bend");
PSTRING(PSTR_FILTER_SYSTEM_EXCLUSIVE,   "- System exclusive");
PSTRING(PSTR_FILTER_TIME_SYNC,          "- Time sync");
PSTRING(PSTR_FILTER_TRANSPORT,          "- Transport");
PSTRING(PSTR_FILTER_ACTIVE_SENSING,     "- Active sensing");
PSTRING(PSTR_FILTER_OTHER,              "- Other");

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




PSTRING(PSTR_MIDIMON_CHANNELS,    "Channels to show");
PSTRING(PSTR_MIDIMON_INOUT,       "IO to show");
PSTRING(PSTR_MIDIMON_FILTER_MSGS, "Filter messages:");

struct midi_monitor_settings
{
  uint8_t all_channels; // 0 = no, only the involved input and output channels, 1 = all
  uint8_t in_out; // 0 = input + output, 1 = input, 2 = output
  filter_settings filter;
};

midi_monitor_settings midimon_settings;

PSTRING(PSTR_MIDIMON_channels_0, "routed");
PSTRING(PSTR_MIDIMON_channels_1, "all");

PSTRING(PSTR_MIDIMON_inout_0, "in and out");
PSTRING(PSTR_MIDIMON_inout_1, "only input");
PSTRING(PSTR_MIDIMON_inout_2, "only output");
PTABLE(PTAB_MIDIMON_inout, PSTR_MIDIMON_inout_0, PSTR_MIDIMON_inout_1, PSTR_MIDIMON_inout_2);
static void line_inout(ParsPars& pars)
{
  pars.types = TypePString|TypePTable;
  pars.name = (void*) PSTR_MIDIMON_INOUT;
  pars.number_of_values = PTAB_MIDIMON_inout_size;
  pars.values = (void*) PTAB_MIDIMON_inout;
}

PSTRING(PSTR_monitor_filter_0, "    ");
PSTRING(PSTR_monitor_filter_1, "show");
Page::LineResult FilterLine(Page::LineFunction func, const char* name, uint8_t& value)
{
  return BoolLine(func, name, value, PSTR_monitor_filter_0, PSTR_monitor_filter_1);
}

Page::LineResult AllFiltersLine(Page::LineFunction func, uint8_t& value)
{
  Page::LineResult result = TextLine(func, PSTR_MIDIMON_FILTER_MSGS);
  if (func == Page::DO_LEFT || func == Page::DO_RIGHT) {
    value = value ? 0 : 1;
    memset(&midimon_settings.filter, value, sizeof(midimon_settings.filter));
    result.redraw = true;
  }
  return result;
}


PageMonitorSettings::PageMonitorSettings(): 
  Page(),
  m_show_hide(0) 
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
  m_ui_inout.Init(line_inout, &midimon_settings.in_out);
  SetNumberOfLines(15);
}

void PageMonitorSettings::OnStop() 
{  
//  EE::SetSettings();
}

const char* PageMonitorSettings::GetTitle()
{
  return GetPString(PSTR_page_monitor_settings);
}

Page::LineResult PageMonitorSettings::Line(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case  0: return BoolLine(func, PSTR_MIDIMON_CHANNELS, midimon_settings.all_channels, PSTR_MIDIMON_channels_0, PSTR_MIDIMON_channels_1);
    case  1: return SingleCombiLine(func, m_ui_inout,  Screen::MaxCharsCanvas, 0, true);
    case  2: return AllFiltersLine(func, m_show_hide);
    case  3: return FilterLine(func, PSTR_FILTER_NOTE_OFF,          midimon_settings.filter.note_off         );
    case  4: return FilterLine(func, PSTR_FILTER_NOTE_ON,           midimon_settings.filter.note_on          );
    case  5: return FilterLine(func, PSTR_FILTER_KEY_PRESSURE,      midimon_settings.filter.key_pressure     );
    case  6: return FilterLine(func, PSTR_FILTER_CONTROL_CHANGE,    midimon_settings.filter.control_change   );
    case  7: return FilterLine(func, PSTR_FILTER_PROGRAM_CHANGE,    midimon_settings.filter.program_change   );
    case  8: return FilterLine(func, PSTR_FILTER_CHANNEL_PRESSURE,  midimon_settings.filter.channel_pressure );
    case  9: return FilterLine(func, PSTR_FILTER_PITCH_BEND,        midimon_settings.filter.pitch_bend       );
    case 10: return FilterLine(func, PSTR_FILTER_SYSTEM_EXCLUSIVE,  midimon_settings.filter.system_exclusive );
    case 11: return FilterLine(func, PSTR_FILTER_TIME_SYNC,         midimon_settings.filter.time_sync        );
    case 12: return FilterLine(func, PSTR_FILTER_TRANSPORT,         midimon_settings.filter.transport        ); 
    case 13: return FilterLine(func, PSTR_FILTER_ACTIVE_SENSING,    midimon_settings.filter.active_sensing   );
    case 14: return FilterLine(func, PSTR_FILTER_OTHER,             midimon_settings.filter.other            );
    default: return DefaultLine(func);
  }
}

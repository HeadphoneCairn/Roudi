#include "PageMonitorSettings.h"

#include "Data.h"
#include "Debug.h"
#include "MidiFilter.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


struct MidiMonitorSettings
{
  uint8_t all_channels; // 0 = no, only the involved input and output channels, 1 = all
  uint8_t in_out; // 0 = input + output, 1 = input, 2 = output
  FilterSettingsValues filter;
};

MidiMonitorSettings midimon_settings;



PSTRING(PSTR_page_monitor_settings,   " MONITOR SETTINGS ");

PSTRING(PSTR_midimon_channels,    "Channels to show");
PSTRING(PSTR_midimon_inout,       "IO to show");

PSTRING(PSTR_midimon_channels_0, "routed");
PSTRING(PSTR_midimon_channels_1, "all");

PSTRING(PSTR_monitor_filter_0, "hide");
PSTRING(PSTR_monitor_filter_1, "show");

PSTRING(PSTR_midimon_inout_0, "in and out");
PSTRING(PSTR_midimon_inout_1, "only input");
PSTRING(PSTR_midimon_inout_2, "only output");
PTABLE(PTAB_midimon_inout, PSTR_midimon_inout_0, PSTR_midimon_inout_1, PSTR_midimon_inout_2);
static void line_inout(ParsPars& pars)
{
  pars.types = TypePString|TypePTable;
  pars.name = (void*) PSTR_midimon_inout;
  pars.number_of_values = PTAB_midimon_inout_size;
  pars.values = (void*) PTAB_midimon_inout;
}


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
    case  0: return BoolLine(func, PSTR_midimon_channels, midimon_settings.all_channels, PSTR_midimon_channels_0, PSTR_midimon_channels_1);
    case  1: return SingleCombiLine(func, m_ui_inout, Screen::MaxCharsCanvas, 0, true);
    case  2: return MessageFilterLine(func, midimon_settings.filter);
    case  3: return LineFilter(func, PSTR_filter_note_off,          midimon_settings.filter.note_off         );
    case  4: return LineFilter(func, PSTR_filter_note_on,           midimon_settings.filter.note_on          );
    case  5: return LineFilter(func, PSTR_filter_key_pressure,      midimon_settings.filter.key_pressure     );
    case  6: return LineFilter(func, PSTR_filter_control_change,    midimon_settings.filter.control_change   );
    case  7: return LineFilter(func, PSTR_filter_program_change,    midimon_settings.filter.program_change   );
    case  8: return LineFilter(func, PSTR_filter_channel_pressure,  midimon_settings.filter.channel_pressure );
    case  9: return LineFilter(func, PSTR_filter_pitch_bend,        midimon_settings.filter.pitch_bend       );
    case 10: return LineFilter(func, PSTR_filter_system_exclusive,  midimon_settings.filter.system_exclusive );
    case 11: return LineFilter(func, PSTR_filter_time_sync,         midimon_settings.filter.time_sync        );
    case 12: return LineFilter(func, PSTR_filter_transport,         midimon_settings.filter.transport        ); 
    case 13: return LineFilter(func, PSTR_filter_active_sensing,    midimon_settings.filter.active_sensing   );
    case 14: return LineFilter(func, PSTR_filter_other,             midimon_settings.filter.other            );
    default: return DefaultLine(func);
  }
}

Page::LineResult PageMonitorSettings::LineFilter(Page::LineFunction func, const char* name, uint8_t& value)
{
  return BoolLine(func, name, value, PSTR_monitor_filter_0, PSTR_monitor_filter_1);
}

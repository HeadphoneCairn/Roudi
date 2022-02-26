#include "PageMonitorSettings.h"

#include "Data.h"
#include "Debug.h"
#include "MidiFilter.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


namespace {

  PSTRING(PSTR_page_monitor_settings,   " MONITOR SETTINGS ");

  PSTRING(PSTR_midimon_channels,    "Channels to show");
  PSTRING(PSTR_midimon_inout,       "IO to show");

  PSTRING(PSTR_midimon_channels_0, "routed");
  PSTRING(PSTR_midimon_channels_1, "all");

  PSTRING(PSTR_monitor_filter_0, "hide");
  PSTRING(PSTR_monitor_filter_1, "show");

  PSTRING(PSTR_midimon_inout_0, "in and out");
  PSTRING(PSTR_midimon_inout_1, "input");
  PSTRING(PSTR_midimon_inout_2, "output");
  PTABLE(PTAB_midimon_inout, PSTR_midimon_inout_0, PSTR_midimon_inout_1, PSTR_midimon_inout_2);
  PTABLE_GETTER(GetInOut, PTAB_midimon_inout);

}

uint8_t PageMonitorSettings::m_selected_line = 0;
uint8_t PageMonitorSettings::m_first_line = 0;

PageMonitorSettings::PageMonitorSettings(): 
  Page() 
  {}

void PageMonitorSettings::OnStart(uint8_t)
{
  EE::GetMidiMonSettings(m_settings);
  SetNumberOfLines(15, m_selected_line, 0, m_first_line);
}

void PageMonitorSettings::OnStop() 
{
  MidiMonSettingsValues eeprom_settings;
  EE::GetMidiMonSettings(eeprom_settings);
  if (memcmp(&eeprom_settings, &m_settings, sizeof(eeprom_settings)) != 0) 
    EE::SetMidiMonSettings(m_settings);
  m_selected_line = GetSelectedLine();
  m_first_line = GetFirstLine();
}

const char* PageMonitorSettings::GetTitle()
{
  return GetPString(PSTR_page_monitor_settings);
}

Page::LineResult PageMonitorSettings::Line(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case  0: return BoolLine(func, PSTR_midimon_channels, m_settings.all_channels, PSTR_midimon_channels_0, PSTR_midimon_channels_1);
    case  1: return SingleLine(func, PSTR_midimon_inout, m_settings.in_out, GetInOut);
    case  2: return LineAllFilters(func, m_settings.filter);
    case  3: return LineFilter(func, PSTR_filter_note_on,           m_settings.filter.note_on          );
    case  4: return LineFilter(func, PSTR_filter_note_off,          m_settings.filter.note_off         );
    case  5: return LineFilter(func, PSTR_filter_pitch_bend,        m_settings.filter.pitch_bend       );
    case  6: return LineFilter(func, PSTR_filter_channel_pressure,  m_settings.filter.channel_pressure );
    case  7: return LineFilter(func, PSTR_filter_key_pressure,      m_settings.filter.key_pressure     );
    case  8: return LineFilter(func, PSTR_filter_program_change,    m_settings.filter.program_change   );
    case  9: return LineFilter(func, PSTR_filter_control_change,    m_settings.filter.control_change   );
    case 10: return LineFilter(func, PSTR_filter_time_sync,         m_settings.filter.time_sync        );
    case 11: return LineFilter(func, PSTR_filter_transport,         m_settings.filter.transport        ); 
    case 12: return LineFilter(func, PSTR_filter_system_exclusive,  m_settings.filter.system_exclusive );
    case 13: return LineFilter(func, PSTR_filter_active_sensing,    m_settings.filter.active_sensing   );
    case 14: return LineFilter(func, PSTR_filter_other,             m_settings.filter.other            );
    default: return DefaultLine(func);
  }
}

Page::LineResult PageMonitorSettings::LineFilter(Page::LineFunction func, const char* name, uint8_t& value)
{
  return BoolLine(func, name, value, PSTR_monitor_filter_0, PSTR_monitor_filter_1);
}

Page::LineResult PageMonitorSettings::LineAllFilters(Page::LineFunction func, FilterSettingsValues& filters)
{
  static uint8_t show = 0;

  Page::LineResult result = TextLine(func, PSTR_filter_messages);
  if (func == Page::DO_LEFT || func == Page::DO_RIGHT) {
    show = 1 - show;
    memset(&filters, show, sizeof(filters));
    result.redraw = true;
  }
  return result;
}



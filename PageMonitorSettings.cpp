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

  PSTRING(PSTR_midimon_channels_0, "all");
  PSTRING(PSTR_midimon_channels_1, "routed");

  PSTRING(PSTR_monitor_filter_0, "hide");
  PSTRING(PSTR_monitor_filter_1, "show");

  PSTRING(PSTR_midimon_inout_0, "in and out");
  PSTRING(PSTR_midimon_inout_1, "input");
  PSTRING(PSTR_midimon_inout_2, "output");
  PTABLE(PTAB_midimon_inout, PSTR_midimon_inout_0, PSTR_midimon_inout_1, PSTR_midimon_inout_2);
  static void line_inout(ParsPars& pars)
  {
    pars.types = TypePString|TypePTable;
    pars.name = (void*) PSTR_midimon_inout;
    pars.number_of_values = PTAB_midimon_inout_size;
    pars.values = (void*) PTAB_midimon_inout;
  }

}

PageMonitorSettings::PageMonitorSettings(): 
  Page() 
  {}

void PageMonitorSettings::OnStart(uint8_t)
{
  EE::GetMidiMonSettings(m_settings);
  m_ui_inout.Init(line_inout, &m_settings.in_out);
  SetNumberOfLines(15);
}

void PageMonitorSettings::OnStop() 
{
  MidiMonSettingsValues eeprom_settings;
  EE::GetMidiMonSettings(eeprom_settings);
  if (memcmp(&eeprom_settings, &m_settings, sizeof(eeprom_settings)) != 0) 
    EE::SetMidiMonSettings(m_settings);
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
    case  1: return SingleCombiLine(func, m_ui_inout, Screen::MaxCharsCanvas, 0, true);
    case  2: return LineAllFilters(func, m_settings.filter);
    case  3: return LineFilter(func, PSTR_filter_note_off,          m_settings.filter.note_off         );
    case  4: return LineFilter(func, PSTR_filter_note_on,           m_settings.filter.note_on          );
    case  5: return LineFilter(func, PSTR_filter_key_pressure,      m_settings.filter.key_pressure     );
    case  6: return LineFilter(func, PSTR_filter_control_change,    m_settings.filter.control_change   );
    case  7: return LineFilter(func, PSTR_filter_program_change,    m_settings.filter.program_change   );
    case  8: return LineFilter(func, PSTR_filter_channel_pressure,  m_settings.filter.channel_pressure );
    case  9: return LineFilter(func, PSTR_filter_pitch_bend,        m_settings.filter.pitch_bend       );
    case 10: return LineFilter(func, PSTR_filter_system_exclusive,  m_settings.filter.system_exclusive );
    case 11: return LineFilter(func, PSTR_filter_time_sync,         m_settings.filter.time_sync        );
    case 12: return LineFilter(func, PSTR_filter_transport,         m_settings.filter.transport        ); 
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



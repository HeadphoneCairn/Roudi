#include "PageSettings.h"

#include "Data.h"
#include "Debug.h"
#include "MidiFilter.h"
#include "MidiProcessing.h"
#include "Pages.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

namespace 
{
  PSTRING(PSTR_page_settings, " SETTINGS ");

  PSTRING(PSTR_input_channel, "Input channel");
  const char* GetInputChannel(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = NumberOfChannels;
    return GetNumber(i_value + 1);
  }
 
  PSTRING(PSTR_block_other, "Block other channels");
  PSTRING(PSTR_block_other_0, "no");
  PSTRING(PSTR_block_other_1, "yes");
  PTABLE(PTAB_block_other, PSTR_block_other_0, PSTR_block_other_1);
  PTABLE_GETTER(GetBlockOther, PTAB_block_other);

  PSTRING(PSTR_velocity_curve, "Velocity curve");
  PTABLE(PTAB_velocity, PSTR_velocity_curve_0, PSTR_velocity_curve_1, PSTR_velocity_curve_2, PSTR_velocity_curve_3);
  PTABLE_GETTER(GetVelocityCurve, PTAB_velocity);

  PSTRING(PSTR_velocity_curve_edit, "Velocity curve edit   ...");

  PSTRING(PSTR_brightness, "Screen brightness");
  PSTRING(PSTR_brightness_0, "low");
  PSTRING(PSTR_brightness_1, "medium");
  PSTRING(PSTR_brightness_2, "high");
  PTABLE(PTAB_brightness, PSTR_brightness_0, PSTR_brightness_1, PSTR_brightness_2);
  PTABLE_GETTER(GetBrightness, PTAB_brightness);

  PSTRING(PSTR_filter_title, "Input channel filter:");
  PSTRING(PSTR_filter_0, "block");
  PSTRING(PSTR_filter_1, "pass");
  PTABLE(PSTR_filter, PSTR_filter_0, PSTR_filter_1);
  PTABLE_GETTER(GetMonitorFltr, PSTR_filter);
}

uint8_t PageSettings::m_selected_line = 0;
uint8_t PageSettings::m_first_line = 0;

PageSettings::PageSettings(): 
  Page()
  {}

void PageSettings::OnStart(uint8_t)
{
  SettingsValues& values = EE::SettingsRW();
  SetNumberOfLines(18, m_selected_line, 0, m_first_line);
}

void PageSettings::OnStop() 
{  
  SaveIfModified();
}

void PageSettings::OnTimeout()
{
  SaveIfModified();
}

const char* PageSettings::GetTitle()
{
  return GetPString(PSTR_page_settings);
}

Page::LineResult PageSettings::Line(LineFunction func, uint8_t line, uint8_t field)
{
  LineResult result = ActualLine(func, line, field);
  if (result.redraw) // A setting has changed, so we update the midi configuration
    SetMidiConfiguration();
  return result;
}

Page::LineResult PageSettings::ActualLine(LineFunction func, uint8_t line, uint8_t field)
{
  SettingsValues& settings = EE::SettingsRW();
  switch (line)
  {
    case  0: return SingleLine(func, PSTR_input_channel, settings.input_channel, GetInputChannel);;
    case  1: return SingleLine(func, PSTR_block_other, settings.block_other, GetBlockOther);
    case  2: return SingleLine(func, PSTR_velocity_curve, settings.velocity_curve, GetVelocityCurve);
    case  3: {
              if (func == DO_LEFT || func == DO_RIGHT) {
                Pages::SetNextPage(PAGE_VELOCITY_SELECT, settings.velocity_curve);
                return DefaultLine(func);
              } else
                return TextLine(func, PSTR_velocity_curve_edit);
             }
    case  4: { 
              uint8_t previous_brightness = settings.brightness;
              Page::LineResult result = SingleLine(func, PSTR_brightness, settings.brightness, GetBrightness);
              if (previous_brightness != settings.brightness)
                Screen::SetBrightness(static_cast<Screen::Brightness>(settings.brightness));
              return result;
             }
    case  5: return TextLine(func, PSTR_filter_title);
    case  6: {
              Page::LineResult result = SingleLine(func, PSTR_filter_note_on_off, settings.filter.note_off, GetMonitorFltr);
              settings.filter.note_on = settings.filter.note_off; // on and off should be filter together
              return result; 
             }
    case  7: return SingleLine(func, PSTR_filter_pitch_bend,        settings.filter.pitch_bend       , GetMonitorFltr);
    case  8: return SingleLine(func, PSTR_filter_channel_pressure,  settings.filter.channel_pressure , GetMonitorFltr);
    case  9: return SingleLine(func, PSTR_filter_key_pressure,      settings.filter.key_pressure     , GetMonitorFltr);
    case 10: return SingleLine(func, PSTR_filter_program_change,    settings.filter.program_change   , GetMonitorFltr);
    case 11: return SingleLine(func, PSTR_filter_cc_1_mod_wheel,    settings.filter.cc_mod_wheel     , GetMonitorFltr);    
    case 12: return SingleLine(func, PSTR_filter_cc_other,          settings.filter.cc_other         , GetMonitorFltr);
    case 13: return SingleLine(func, PSTR_filter_time_sync,         settings.filter.time_sync        , GetMonitorFltr);
    case 14: return SingleLine(func, PSTR_filter_transport,         settings.filter.transport        , GetMonitorFltr); 
    case 15: return SingleLine(func, PSTR_filter_system_exclusive,  settings.filter.system_exclusive , GetMonitorFltr);
    case 16: return SingleLine(func, PSTR_filter_active_sensing,    settings.filter.active_sensing   , GetMonitorFltr);
    case 17: return SingleLine(func, PSTR_filter_other,             settings.filter.other            , GetMonitorFltr);
    default: return DefaultLine(func);
  }
}

void PageSettings::SaveIfModified()
{
  EE::SetSettings();
  
  m_selected_line = GetSelectedLine();
  m_first_line = GetFirstLine();
}

void PageSettings::SetMidiConfiguration()
{
  MidiProcessing::Configuration next_config = MidiProcessing::GetConfiguration();
  next_config.m_input_channel = EE::SettingsRW().input_channel;
  next_config.m_default_filter = EE::SettingsRW().filter;
  MidiProcessing::SetNextConfiguration(next_config);
}

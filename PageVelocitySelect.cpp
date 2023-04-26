#include "PageVelocitySelect.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"

namespace
{
  PSTRING(PSTR_page_velocity_select, " EDIT VELOCITY CURVE ");
}

PageVelocitySelect::PageVelocitySelect(): Page()
{
}

void PageVelocitySelect::OnStart(uint8_t which_curve)
{
  SetNumberOfLines(6, which_curve);

}

void PageVelocitySelect::OnStop()
{
//  SaveIfModified();
}

const char* PageVelocitySelect::GetTitle()
{
  return GetPString(PSTR_page_velocity_select);
}

Page::LineResult PageVelocitySelect::Line(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line) {
    case 0: return TextLine(func, PSTR_velocity_curve_0);
    case 1: return TextLine(func, PSTR_velocity_curve_1);
    case 2: return TextLine(func, PSTR_velocity_curve_2);
    case 3: return TextLine(func, PSTR_velocity_curve_3);
    case 5:
      if (func==DO_LEFT || func==DO_RIGHT)
        Pages::SetNextPage(PAGE_SETTINGS);
      return TextLine(func, PSTR_done);
  }
  return DefaultLine(func);
}

void PageVelocitySelect::SetMidiConfiguration(uint8_t selected_line)
{
  MidiProcessing::Configuration next_config;
  next_config.m_input_channel = EE::Settings().input_channel;
  if (selected_line < NumberOfChannels) {
    next_config.m_nbr_output_channels = 1;
    next_config.m_output_channel[0].m_channel = selected_line;
  } else {
    next_config.m_nbr_output_channels = 0;
  }
  MidiProcessing::SetNextConfiguration(next_config);
}

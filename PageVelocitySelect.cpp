#include "PageVelocitySelect.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Utils.h"

namespace
{
  #define ADD_ELLIPSIS // uses 44 more bytes of program space
  #ifdef ADD_ELLIPSIS
  const char* GetVelocityCurveNameWithEllipsis(int which)
  {
    strcpy(Screen::buffer, GetVelocityCurveName(which));
    PadRight(Screen::buffer, 22 - strlen(Screen::buffer));
    PadRight(Screen::buffer, 3, '.');
    return Screen::buffer;
  }
  #endif
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
}

const char* PageVelocitySelect::GetTitle()
{
  return GetPString(PSTR_velocity_curve_edit_title);
}

Page::LineResult PageVelocitySelect::Line(LineFunction func, uint8_t line, uint8_t field)
{
  const char* p; 
  if (line == 0) {
    p = GetVelocityCurveName(0);
  } else if (line < 4) {
    if (func==DO_LEFT || func==DO_RIGHT)
      Pages::SetNextPage(PAGE_VELOCITY_EDIT, line);
#ifdef ADD_ELLIPSIS
      p = GetVelocityCurveNameWithEllipsis(line);
#else
      p = GetVelocityCurveName(line);
#endif
  } else if (line == 5) {
    if (func==DO_LEFT || func==DO_RIGHT)
      Pages::SetNextPage(PAGE_SETTINGS);
    p = GetPString(PSTR_done);
  } else {
    p = GetPString(PSTR_empty);
  }

  return Page::LineResult{1, p, Screen::inversion_all, false};
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

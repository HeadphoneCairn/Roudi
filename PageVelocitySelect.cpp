#include "PageVelocitySelect.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Utils.h"

namespace
{
  PSTRING(PSTR_page_velocity_select, " VELOCITY CURVE EDIT ");
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

#define ADD_ELLIPSIS // uses 112 more bytes of program space
#ifdef ADD_ELLIPSIS
const char* GetVelocityCurve(const char* velocity_curve)
{
  strcpy(data_scratch, GetPString(velocity_curve));
  PadRight(data_scratch, 22-strlen(data_scratch));
  PadRight(data_scratch, 3, '.');
  return data_scratch;
}
#endif

Page::LineResult PageVelocitySelect::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (line < 4) {
    if (func==DO_LEFT || func==DO_RIGHT)
      Pages::SetNextPage(PAGE_VELOCITY_EDIT, line);
#ifdef ADD_ELLIPSIS
    const char* p = nullptr;
    switch (line) {
      case 0: p=PSTR_velocity_curve_0; break;
      case 1: p=PSTR_velocity_curve_1; break;
      case 2: p=PSTR_velocity_curve_2; break;
      case 3: p=PSTR_velocity_curve_3; break;
    }
    return Page::LineResult{1, GetVelocityCurve(p), Screen::inversion_all, false};
#else
    const char* p = nullptr;
    switch (line) {
      case 0: p=PSTR_velocity_curve_0; break;
      case 1: p=PSTR_velocity_curve_1; break;
      case 2: p=PSTR_velocity_curve_2; break;
      case 3: p=PSTR_velocity_curve_3; break;
    }
    return TextLine(func, p);
#endif
  } else if (line==5) {
    if (func==DO_LEFT || func==DO_RIGHT)
      Pages::SetNextPage(PAGE_SETTINGS);
    return TextLine(func, PSTR_done);
  } else {
    return DefaultLine(func);
  }

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

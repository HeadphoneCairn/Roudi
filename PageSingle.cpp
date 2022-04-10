#include "PageSingle.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"

namespace
{
  PSTRING(PSTR_page_single, " SINGLE "); 
  PSTRING(PSTR_none, "None");
  PSTRING(PSTR_panic, "> Panic!");
}

PageSingle::PageSingle(): Page()
{
}

void PageSingle::OnStart(uint8_t)
{
  SingleValues values;
  EE::GetSingle(values);
  SetNumberOfLines(NumberOfChannels + 3, values.channel, 0, values.first_line);
  SetMidiConfiguration(values.channel);
}

void PageSingle::OnStop()
{
  SaveIfModified();
}

void PageSingle::OnTimeout()
{
  SaveIfModified();
}

const char* PageSingle::GetTitle()
{
  return GetPString(PSTR_page_single);
}

Page::LineResult PageSingle::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (line <= NumberOfChannels) {
    const char* text = nullptr;
    if (func == GET_TEXT) {
      text = (line < NumberOfChannels) ? EE::GetChannelNameFormatted(line) : GetPString(PSTR_none);
    } else if (func == DO_LEFT || func == DO_RIGHT) {
      if (line < NumberOfChannels)
        Pages::SetNextPage(PAGE_NAME_CHANNEL, line);
    } else if (func == DO_SELECTED) {
      SetMidiConfiguration(line);
    }
    return {1, text, Screen::inversion_all, false};
  } else if (line == NumberOfChannels + 2) {
    if (func == DO_LEFT || func == DO_RIGHT)
      MidiProcessing::SetPanic(); 
    return TextLine(func, PSTR_panic);
  }
  return DefaultLine(func);
}

void PageSingle::SaveIfModified()
{
  // Update "cursor"
  SingleValues values = {GetFirstLine(), GetSelectedLine()};
  
  // Save values
  EE::SetSingle(values); // (will only save if values have changed)
}

void PageSingle::SetMidiConfiguration(uint8_t selected_line)
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

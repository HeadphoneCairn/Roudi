#include "PageSingle.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"

PSTRING(PSTR_page_single, " SINGLE "); 

PageSingle::PageSingle(): Page()
{
}

void PageSingle::OnStart(uint8_t)
{
  SingleValues values;
  EE::GetSingle(values);
  SetNumberOfLines(NumberOfChannels + 1, values.channel, 0, values.first_line);
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
  const char* text = nullptr;
  if (func == GET_TEXT) {
    text = (line < NumberOfChannels) ? EE::GetChannelNameFormatted(line) : GetPStringNone();
  } else if (func == DO_LEFT || func == DO_RIGHT) {
    if (line < NumberOfChannels)
      Pages::SetNextPage(PAGE_NAME_CHANNEL, line);
  } else if (func == DO_SELECTED) {
    SetMidiConfiguration(line);
  }
  
  return {1, text, Screen::inversion_all, false};
}

void PageSingle::SaveIfModified()
{
  SingleValues values = {GetFirstLine(), GetSelectedLine()};
  SingleValues stored_values;
  EE::GetSingle(stored_values);
  if (memcmp(&stored_values, &values, sizeof(values)) != 0)
    EE::SetSingle(values);
}

void PageSingle::SetMidiConfiguration(uint8_t selected_line)
{
  MidiProcessing::Configuration next_config;
    next_config.m_input_channel = EE::GetSettings().input_channel;
  if (selected_line < NumberOfChannels) {
    next_config.m_nbr_output_channels = 1;
    next_config.m_output_channel[0].m_channel = selected_line;
  } else {
    next_config.m_nbr_output_channels = 0;
  }
  MidiProcessing::SetNextConfiguration(next_config);
}

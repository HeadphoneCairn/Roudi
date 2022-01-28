#include "PageNameChannel.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"
#include "Utils.h"

PSTRING(PSTR_page_name_channel, " CHANNEL NAME "); 
PSTRING(PSTR_channel_prefix, "ch");

PageNameChannel::PageNameChannel(): PageName()
{
}

void PageNameChannel::OnStart(uint8_t channel_value)
{
  SetValues(EE::GetChannelName(channel_value), channel_value, NumberOfChannels, PSTR_channel_prefix);
  PageName::OnStart();
}

const char* PageNameChannel::GetTitle()
{
  return GetPString(PSTR_page_name_channel);
}

bool PageNameChannel::OnUpDown(UpDownAction action)
{  
  bool redraw = PageName::OnUpDown(action);
  if (GetResult() == ACCEPT) {
    uint8_t channel_value;
    GetNameAndValue(data_scratch, channel_value);
    EE::SetChannelName(channel_value, data_scratch);
    Pages::SetNextPage(PAGE_SINGLE);
  } else if (GetResult() == REJECT) {
    Pages::SetNextPage(PAGE_SINGLE);
  }
  return redraw;
}
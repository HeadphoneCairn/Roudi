#include "PageNameChannel.h"

#include "Debug.h"
#include "Data.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

PSTRING(PSTR_page_name_channel, " CHANNEL NAME "); 
PSTRING(PSTR_channel_prefix, "ch");

PageNameChannel::PageNameChannel(/*uint8_t channel_value*/): PageName()
{
}

void PageNameChannel::OnStart()
{
  uint8_t channel_number = 4;
  SetValues(GetChannelNameBrol(channel_number), channel_number, PSTR_channel_prefix);
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
    //GetNameAndValue(data_scratch, number)
    //EE::SetChannelName(m_channel_value, data_scratch);
    //Pages::SetNextPage(PAGE_CHANNELS);
    Debug::Beep();
  } else if (GetResult() == REJECT) {
    //Pages::SetNextPage(PAGE_CHANNELS);
    Debug::BeepLow();
  }
  return redraw;
}
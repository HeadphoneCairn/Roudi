/*
 * Roudi, a ROUter for miDI messages for Blokas Midiboy
 * Copyright (C) 2019-2023 Headphone Cairn Software
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2 of the License.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "PageSingleName.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"
#include "Utils.h"

PSTRING(PSTR_page_name_channel, " CHANNEL NAME "); 
PSTRING(PSTR_channel_prefix, "ch");

PageSingleName::PageSingleName(): PageName()
{
}

void PageSingleName::OnStart(uint8_t channel_value)
{
  SetValues(EE::GetChannelName(channel_value), channel_value, NumberOfChannels, PSTR_channel_prefix);
  PageName::OnStart();
}

const char* PageSingleName::GetTitle()
{
  return GetPString(PSTR_page_name_channel);
}

bool PageSingleName::OnUpDown(UpDownAction action)
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
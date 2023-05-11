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


#include "PageMultiName.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"
#include "Utils.h"

PSTRING(PSTR_page_name_multi, " SAVE MULTI AS "); 
PSTRING(PSTR_multi_prefix, "mul");

PageMultiName::PageMultiName(): PageName(false)
{
}

void PageMultiName::OnStart(uint8_t which_multi)
{
  m_which_multi = which_multi;
  const uint8_t max_multi_allowed = min(EE::GetNumberOfMultis() + 1, EE::GetMaxNumberOfMultis()); // User can create a new multi by saving one further than current number of multis. 
  MultiValues values;
  EE::GetMulti(m_which_multi, values);
  SetValues(values.name, m_which_multi, max_multi_allowed, PSTR_multi_prefix);
  PageName::OnStart();
}

const char* PageMultiName::GetTitle()
{
  return GetPString(PSTR_page_name_multi);
}

bool PageMultiName::OnUpDown(UpDownAction action)
{  
  bool redraw = PageName::OnUpDown(action);
  if (GetResult() == ACCEPT) {
    MultiValues values;
    EE::GetMulti(m_which_multi, values); // I can just read this multi from disk, as it was saved through OnStop before showing this page.
    uint8_t selected_multi;
    GetNameAndValue(values.name, selected_multi); // Replace the name in the values
    if (selected_multi != m_which_multi) // If we save to another position, we reset the position of the "cursor" 
      values.selected_line = values.selected_field = values.first_line = 0; 
    EE::SetMulti(selected_multi, values);
    Pages::SetNextPage(PAGE_MULTI, selected_multi);
  } else if (GetResult() == REJECT) {
    Pages::SetNextPage(PAGE_MULTI);
  }
  return redraw;
}
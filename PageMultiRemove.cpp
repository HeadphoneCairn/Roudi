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


#include "PageMultiRemove.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"
#include "Utils.h"

PSTRING(PSTR_remove_title, " REMOVE MULTI? "); 
PSTRING(PSTR_remove_cancel, "Cancel");
PSTRING(PSTR_remove_remove_format, "Remove %02d. %s");

PageMultiRemove::PageMultiRemove()
{
}

void PageMultiRemove::OnStart(uint8_t which_multi)
{
  m_which_multi = which_multi;
  SetNumberOfLines(2);
}

const char* PageMultiRemove::GetTitle()
{
  return GetPString(PSTR_remove_title);
}

Page::LineResult PageMultiRemove::Line(LineFunction func, uint8_t line, uint8_t field)
{
  char* text = Screen::buffer;
  if (line == 0)
    strcpy(text, GetPString(PSTR_remove_cancel));
  else {
    MultiValues values;
    EE::GetMulti(m_which_multi, values);
    snprintf(text, sizeof(Screen::buffer), GetPString(PSTR_remove_remove_format), m_which_multi + 1, values.name);
  }
  if (func == DO_LEFT || func == DO_RIGHT) {
    uint8_t multi_to_show = m_which_multi;
    if (line == 1)
      multi_to_show = EE::RemoveMulti(m_which_multi);
    Pages::SetNextPage(PAGE_MULTI, multi_to_show); 
  }
  return Page::LineResult{1, text, Screen::inversion_all, false};
}

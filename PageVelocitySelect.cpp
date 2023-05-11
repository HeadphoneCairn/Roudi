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


#include "PageVelocitySelect.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Roudi.h"
#include "Utils.h"

#ifdef ENABLE_VELOCITY_EDIT_PAGE

PageVelocitySelect::PageVelocitySelect(): Page()
{
}

void PageVelocitySelect::OnStart(uint8_t which_curve)
{
  SetNumberOfLines(GetNumberOfVelocityCurves() + 2, which_curve);
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
  } else if (line < GetNumberOfVelocityCurves()) {
    if (func==DO_LEFT || func==DO_RIGHT)
      Pages::SetNextPage(PAGE_VELOCITY_EDIT, line);
      strcpy(Screen::buffer, GetVelocityCurveName(line)); 
      p = AddEllipsis(Screen::buffer);
  } else if (line == GetNumberOfVelocityCurves() + 1) {
    if (func==DO_LEFT || func==DO_RIGHT)
      Pages::SetNextPage(PAGE_SETTINGS);
    p = GetPString(PSTR_done);
  } else {
    p = GetPString(PSTR_empty);
  }

  return Page::LineResult{1, p, Screen::inversion_all, false};
}

#endif // ENABLE_VELOCITY_EDIT_PAGE

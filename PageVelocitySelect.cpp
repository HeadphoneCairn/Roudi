#include "PageVelocitySelect.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Utils.h"

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

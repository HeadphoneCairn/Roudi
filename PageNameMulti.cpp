#include "PageNameMulti.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"
#include "Utils.h"

PSTRING(PSTR_page_name_multi, " SAVE MULTI AS "); 
PSTRING(PSTR_multi_prefix, "mul");

PageNameMulti::PageNameMulti(): PageName(false)
{
}

void PageNameMulti::OnStart(uint8_t which_multi)
{
  m_which_multi = which_multi;
  const uint8_t max_multi_allowed = min(EE::GetNumberOfMultis() + 1, EE::GetMaxNumberOfMultis()); // User can create a new multi by saving one further than current number of multis. 
  MultiValues values;
  EE::GetMulti(m_which_multi, values);
  SetValues(values.name, m_which_multi, max_multi_allowed, PSTR_multi_prefix);
  PageName::OnStart();
}

const char* PageNameMulti::GetTitle()
{
  return GetPString(PSTR_page_name_multi);
}

bool PageNameMulti::OnUpDown(UpDownAction action)
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
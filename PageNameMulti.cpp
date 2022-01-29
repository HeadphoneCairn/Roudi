#include "PageNameMulti.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"
#include "Utils.h"

PSTRING(PSTR_page_name_multi, " SAVE MULTI AS "); 
PSTRING(PSTR_multi_prefix, "mul");

PageNameMulti::PageNameMulti(): PageName()
{
}

void PageNameMulti::OnStart(uint8_t which_multi)
{
  const uint8_t max_multi_allowed = min(EE::GetNumberOfMultis() + 1, EE::GetMaxNumberOfMultis()); // User can create a new multi by saving one further than current number of multis. 
  MultiValues values;
  EE::GetMulti(which_multi, values);
  SetValues(values.name, which_multi, max_multi_allowed, PSTR_multi_prefix);
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
    uint8_t multi;
    MultiValues values;
    GetMultiDefault(values);
    GetNameAndValue(values.name, multi);
    EE::SetMulti(multi, values);
    Pages::SetNextPage(PAGE_MULTI, multi);
  } else if (GetResult() == REJECT) {
    Pages::SetNextPage(PAGE_MULTI);
  }
  return redraw;
}
#include "PageMultiRemove.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"
#include "Utils.h"

PSTRING(PSTR_remove_title, " REMOVE MULTI? "); 
PSTRING(PSTR_remove_cancel, "Cancel");
PSTRING(PSTR_remove_remove, "Remove %02d. %s");

PageMultiRemove::PageMultiRemove()
{
}

void PageMultiRemove::OnStart(uint8_t which_multi)
{
  m_which_multi = which_multi;

  MultiValues values;
  EE::GetMulti(m_which_multi, values);
  snprintf(m_remove, sizeof(m_remove), GetPString(PSTR_remove_remove), m_which_multi + 1, values.name);

  SetNumberOfLines(2);
}

const char* PageMultiRemove::GetTitle()
{
  return GetPString(PSTR_remove_title);
}

Page::LineResult PageMultiRemove::Line(LineFunction func, uint8_t line, uint8_t field)
{
  const char* text = (line == 0) ? GetPString(PSTR_remove_cancel) : m_remove;
  if (func == DO_LEFT || func == DO_RIGHT) {
    uint8_t multi_to_show = m_which_multi;
    if (line == 1)
      multi_to_show = EE::RemoveMulti(m_which_multi);
    Pages::SetNextPage(PAGE_MULTI, multi_to_show); 
  }
  return Page::LineResult{1, text, Screen::inversion_all, false};
}

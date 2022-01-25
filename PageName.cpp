#include "PageName.h"

#include "Debug.h"
#include "Data.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


const uint8_t MaxLength = MaxNameLength; 

PSTRING(PSTR_number_format, "%02d v X");
PSTRING(PSTR_name_help_1, "use left/right to move");
PSTRING(PSTR_name_help_2, "up/down to make changes");
PSTRING(PSTR_name_help_3, "v=save, X=cancel");

PageName::PageName(): Page(), m_number_prefix_len(0), m_number(0), m_position(0), m_result(NONE)
{
}

/*
  The name editing line looks as follows:
   <name>____ <number_prefix><number+1> v X
   Matriarch_ ch01 v X"
  
  NOTE: number starts at 0 but is shown as number + 1.
*/
void PageName::SetValues(const char* name, uint8_t number, uint8_t number_max, const char* number_prefix)
{
  m_number = number % number_max; // make sure m_number is smaller than max
  m_number_prefix_len = strlen(GetPString(number_prefix));
  m_number_max = number_max;
  m_result = NONE;
  Screen::buffer[MaxLength] = ' ';
  strcpy(Screen::buffer + MaxLength + 1, GetPString(number_prefix));
  sprintf(Screen::buffer + MaxLength + 1 + m_number_prefix_len, GetPString(PSTR_number_format), m_number + 1);
  m_name.Init(Screen::buffer, name);
}

PageName::Result PageName::GetResult()
{
  return m_result;
}

void PageName::GetNameAndValue(char* name, uint8_t& number)
{
  m_name.CopyName(name);
  number = m_number; 
}

void PageName::OnStart(uint8_t)
{
  m_position = 0;
  SetNumberOfLines(7, 1, 0);
}

Page::LineResult PageName::Line(LineFunction func, uint8_t line, uint8_t field)
{
  const char* text = nullptr;
  Screen::Inversion inversion = Screen::inversion_none;
  bool redraw = false;

  if (func == GET_TEXT) {
    if (line == 1) {
      text = Screen::buffer;
      uint8_t start = m_position; // NAME
      if (m_position == GetPositionOfNumber()) // NUMBER
        start = MaxLength + 1;
      else if (m_position == GetPositionOfAccept()) // ACCEPT 
        start = MaxLength + 4 + m_number_prefix_len;
      else if (m_position == GetPositionOfCancel()) // CANCEL
        start = MaxLength + 6 + m_number_prefix_len;
      uint8_t stop = (m_position == GetPositionOfNumber()) ? start + 1 + m_number_prefix_len : start;
      inversion = {Screen::InvertGiven, start, stop};
    } else if (line == 4) {
      text = GetPString(PSTR_name_help_1);
    } else if (line == 5) {
      text = GetPString(PSTR_name_help_2);
    } else if (line == 6) {
      text = GetPString(PSTR_name_help_3);
    } else {
      text = GetPStringEmpty();
    }
  } else if (func == DO_LEFT && m_position > 0) {
    m_position--;
    redraw = true;
  } else if (func == DO_RIGHT && m_position < GetMaxPositions() - 1) {
    m_position++;
    redraw = true;
  }

  return {1, text, inversion, redraw};
}

bool PageName::OnUpDown(UpDownAction action)
{
  m_result = NONE;

  if (m_position < m_name.GetPositions()) // NAME
  {
    return m_name.UpDown(m_position, action == UP);
  } else if (m_position == GetPositionOfNumber()) { // NUMBER
    uint8_t old_number = m_number;
    if (action == UP && m_number < m_number_max - 1)
      m_number++;      
    if (action == DOWN && m_number > 0)
      m_number--;
    if (old_number != m_number)
      sprintf(Screen::buffer + MaxLength + 1 + m_number_prefix_len, GetPString(PSTR_number_format), m_number + 1);
    return old_number != m_number;
  } else if (m_position == GetPositionOfAccept()) { // ACCEPT
    m_result = ACCEPT;
  } else if (m_position == GetPositionOfCancel()) { // CANCEL
    m_result = REJECT;
  }
  return false;
}


uint8_t PageName::GetMaxPositions()
{
  return m_name.GetPositions() + 3;
}

uint8_t PageName::GetPositionOfNumber()
{
  return m_name.GetPositions();
}

uint8_t PageName::GetPositionOfAccept()
{
  return m_name.GetPositions() + 1;
}

uint8_t PageName::GetPositionOfCancel()
{
  return m_name.GetPositions() + 2;
}

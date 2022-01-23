#include "PageName.h"

#include "Debug.h"
#include "Data.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>


const uint8_t MaxLength = 10; // TODO MaxNameLength; 

PSTRING(PSTR_number_format, "%02d v X");

PageName::PageName(/*uint8_t channel_value*/): Page(), m_number_prefix_len(0), m_number(0), m_position(0), m_result(NONE)
{
}

/*
  The name editing line looks as follows:
   <name>____ <number_prefix><number> v X
   Matriarch_ ch01 v X"
*/
void PageName::SetValues(const char* name, uint8_t number, const char* number_prefix)
{
  m_number_prefix_len = strlen(number_prefix);
  m_number = number;
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

void PageName::OnStart()
{
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
      if (m_position == GetPositionOfChannel()) // CHANNEL
        start = MaxLength + 1;
      else if (m_position == GetPositionOfAccept()) // ACCEPT 
        start = MaxLength + 4 + m_number_prefix_len;
      else if (m_position == GetPositionOfCancel()) // CANCEL
        start = MaxLength + 6 + m_number_prefix_len;
      uint8_t stop = (m_position == GetPositionOfChannel()) ? start + 1 + m_number_prefix_len : start;
      inversion = {Screen::InvertGiven, start, stop};
    } else if (line == 4) {
      text = GetPStringNameHelp1();
    } else if (line == 5) {
      text = GetPStringNameHelp2();
    } else if (line == 6) {
      text = GetPStringNameHelp3();
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
  } else if (m_position == GetPositionOfChannel()) { // CHANNEL
    uint8_t old_channel_value = m_number;
    if (action == UP && m_number < EE::GetNumberOfChannels() - 1)
      m_number++;      
    if (action == DOWN && m_number > 0)
      m_number--;
    if (old_channel_value != m_number)
      sprintf(Screen::buffer + MaxLength + 1 + m_number_prefix_len, GetPString(PSTR_number_format), m_number + 1);
    return old_channel_value != m_number;
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

uint8_t PageName::GetPositionOfChannel()
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

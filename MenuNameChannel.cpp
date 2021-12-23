#include "MenuNameChannel.h"

#include "Debug.h"
#include "Data.h"
#include "Menus.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

PSTRING(PSTR_menu_name_channel, " CHANNEL NAME "); 

// The channel name editing line looks as follows:
//   "ChannelName____  01  v X"
PSTRING(PSTR_layout_channel, /*...............*/" ch%02d v X");

MenuNameChannel::MenuNameChannel(uint8_t channel_value): Menu(), m_channel_value(channel_value), m_position(0)
{
  SetNumberOfLines(7, 1);
  sprintf(Screen::buffer + MaxNameLength, GetPString(PSTR_layout_channel), channel_value + 1);
  m_name.Init(Screen::buffer, EE::GetChannelName(m_channel_value));
}

const char* MenuNameChannel::GetTitle()
{
  return GetPString(PSTR_menu_name_channel);
}

void MenuNameChannel::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  inversion = {Screen::InvertNone, 0, 0};
  if (line == 1) {
    text = Screen::buffer;
    uint8_t start = m_position; // NAME
    if (m_position == GetPositionOfChannel()) // CHANNEL
      start = MaxNameLength + 1;
    else if (m_position == GetPositionOfAccept()) // ACCEPT 
      start = MaxNameLength + 6;
    else if (m_position == GetPositionOfCancel()) // CANCEL
      start = MaxNameLength + 8;
    uint8_t stop = (m_position == GetPositionOfChannel()) ? start + 3 : start;
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
}

bool MenuNameChannel::OnUpDown(UpDownAction action)
{  
  if (m_position < m_name.GetPositions()) // NAME
  {
    return m_name.UpDown(m_position, action == UP);
  } else if (m_position == GetPositionOfChannel()) { // CHANNEL
    uint8_t old_channel_value = m_channel_value;
    if (action == UP && m_channel_value < EE::GetNumberOfChannels() - 1)
      m_channel_value++;      
    if (action == DOWN && m_channel_value > 0)
      m_channel_value--;
    if (old_channel_value != m_channel_value)
      sprintf(Screen::buffer + MaxNameLength, GetPString(PSTR_layout_channel), m_channel_value + 1);
    return old_channel_value != m_channel_value;
  } else if (m_position == GetPositionOfAccept()) { // ACCEPT
    m_name.CopyName(data_scratch);
    EE::SetChannelName(m_channel_value, data_scratch);
    Menus::SetNextMenu(MENU_CHANNELS);
  } else if (m_position == GetPositionOfCancel()) { // CANCEL
    Menus::SetNextMenu(MENU_CHANNELS);
  }
  return false;
}

bool MenuNameChannel::OnLine(LineAction action, uint8_t line)
{
  if (action == LEFT && m_position > 0) {
    m_position--;
    return true;
  } else if (action == RIGHT && m_position < GetMaxPositions() - 1) {
    m_position++;
    return true;
  } else {
    return false;
  }
}

uint8_t MenuNameChannel::GetMaxPositions()
{
  return m_name.GetPositions() + 3;
}

uint8_t MenuNameChannel::GetPositionOfChannel()
{
  return m_name.GetPositions();
}

uint8_t MenuNameChannel::GetPositionOfAccept()
{
  return m_name.GetPositions() + 1;
}

uint8_t MenuNameChannel::GetPositionOfCancel()
{
  return m_name.GetPositions() + 2;
}

#if 0 // Due to the complexity of named presets, we are currently not using this menu

#include "MenuNamePreset.h"

#include "Debug.h"
#include "Data.h"
#include "Menus.h"
#include "Utils.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

PSTRING(PSTR_menu_name_preset, " PRESET NAME "); 

// The preset name editing line looks as follows:
//   "01. PresetName____   v X"
PSTRING(PSTR_layout_preset,  "%02d. ...............  v X");

MenuNamePreset::MenuNamePreset(uint8_t preset_value): Menu(), m_position(0)
{
  SetNumberOfLines(7, 1);

  if (preset_value != 0xFF)
    m_preset_value = preset_value;
  else
    m_preset_value = 19;  // new preset
  sprintf(Screen::buffer, GetPString(PSTR_layout_preset), m_preset_value + 1);

  const char* name;
  if (preset_value != 0xFF) 
    name = EE::GetChannelName(m_preset_value);
  else
    name = GetPStringEmpty();
  m_name.Init(Screen::buffer + 4, name);
}

const char* MenuNamePreset::GetTitle()
{
  return GetPString(PSTR_menu_name_preset);
}

void MenuNamePreset::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  inversion = Screen::inversion_none;
  if (line == 1) {
    text = Screen::buffer;
    uint8_t start = m_position + 4; // NAME
    if (m_position == GetPositionOfAccept()) // ACCEPT 
      start = MaxNameLength + 6;
    else if (m_position == GetPositionOfCancel()) // CANCEL
      start = MaxNameLength + 8;
    uint8_t stop = start;
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

bool MenuNamePreset::OnUpDown(UpDownAction action)
{  
  if (m_position < m_name.GetPositions()) // NAME
  {
    return m_name.UpDown(m_position, action == UP);
  } else if (m_position == GetPositionOfAccept()) { // ACCEPT
    Menus::PopMenu();
//    m_name.CopyName(data_scratch);
//    Debug::Print("save %s on %d", data_scratch, m_preset_value);
//    Debug::BeepLow();
  } else if (m_position == GetPositionOfCancel()) { // CANCEL
    Menus::PopMenu();
//    Debug::Beep();
  }
  return false;
}

bool MenuNamePreset::OnLine(LineAction action, uint8_t line)
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

void MenuNamePreset::OnStop(uint8_t selected_line, uint8_t first_line)
{
 /*
  g_first_line = first_line;
  g_selected_channel = selected_line < m_number_of_channels ? EE::ChannelIndexToChannelValue(selected_line) : selected_line;
*/
}


uint8_t MenuNamePreset::GetMaxPositions()
{
  return m_name.GetPositions() + 2;
}

uint8_t MenuNamePreset::GetPositionOfAccept()
{
  return m_name.GetPositions();
}

uint8_t MenuNamePreset::GetPositionOfCancel()
{
  return m_name.GetPositions() + 1;
}

#endif
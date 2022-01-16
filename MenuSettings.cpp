#include "MenuSettings.h"
#include "Data.h"
#include "Debug.h"
#include "Menus.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*

·[MIDI Settings]       ·
·Input channel       01·
·Output channels   MENU·
·Channel order    alpha·      alpha|numeric (alpha = alphabetical followed by empty)
·Channel number    show·      show|hide     (hide = show only if channel has no name)
·Velocity        normal·      normal|custom|none  (none = velocity 64)
//·Custom Velocity   MENU·
·Program change   block·      block|allow   (includes bank select)
//·CC Mapping        MENU
·Split inc/dec   octave·      octave|CEA|C major|semitone
·Screen brightness     ·

*/

namespace
{
  uint8_t g_selected_line = 0;
  uint8_t g_first_line = 0;
}

PSTRING(PSTR_menu_settings,   " SETTINGS ");

PSTRING(PSTR_input_channel,   "Input channel");
PSTRING(PSTR_output_channels, "Output channels");
PSTRING(PSTR_channel_order,   "Channel order");
PSTRING(PSTR_channel_number,  "Channel number");
PSTRING(PSTR_velocity_curve,  "Velocity curve");
PSTRING(PSTR_program_change,  "Program change");
PSTRING(PSTR_split_delta,     "Split inc/dec");
PSTRING(PSTR_brightness,      "Screen brightness");

PSTRING(PSTR_ellipsis_0,        "...");
PTABLE(PTAB_ellipsis, PSTR_ellipsis_0);

PSTRING(PSTR_chorder_0, "alpha");
PSTRING(PSTR_chorder_1, "numeric");
PTABLE(PTAB_chorder, PSTR_chorder_0, PSTR_chorder_1);

PSTRING(PSTR_chnumber_0, "hide");
PSTRING(PSTR_chnumber_1, "show");
PTABLE(PTAB_chnumber, PSTR_chnumber_0, PSTR_chnumber_1);

PSTRING(PSTR_velocity_0, "soft");
PSTRING(PSTR_velocity_1, "medium");
PSTRING(PSTR_velocity_2, "hard");
PTABLE(PTAB_velocity, PSTR_velocity_0, PSTR_velocity_1, PSTR_velocity_2);

PSTRING(PSTR_progchange_0, "block");
PSTRING(PSTR_progchange_1, "allow");
PTABLE(PTAB_progchange, PSTR_progchange_0, PSTR_progchange_1);

PSTRING(PSTR_split_0, "c octave");
PSTRING(PSTR_split_1, "c, e, a");
PSTRING(PSTR_split_2, "c major");
PSTRING(PSTR_split_3, "semitone");
PTABLE(PTAB_split, PSTR_split_0, PSTR_split_1, PSTR_split_2, PSTR_split_3);





// 3 bytes per line is reached by using function to store parameters
// instead of storing them in memory. Looks horrible, but uses less
// memory.
// Can't do pars = { ... } because it will use more memory again!

static void line_input_channel(ParsPars& pars)
{
  pars.types = TypePString|TypeRAlign|TypeFunction;
  pars.name = (void*) PSTR_input_channel;
  pars.number_of_values = 16;
  pars.values = (void*) GetNumberPlusOne;
}

static void line_output_channels(ParsPars& pars)
{
  pars.types = TypePString|TypeRAlign|TypePTable;
  pars.name = (void*) PSTR_output_channels;
  pars.number_of_values = PTAB_ellipsis_size;
  pars.values = (void*) PTAB_ellipsis;
}

static void line_channel_order(ParsPars& pars)
{
  pars.types = TypePString|TypeRAlign|TypePTable;
  pars.name = (void*) PSTR_channel_order;
  pars.number_of_values = PTAB_chorder_size;
  pars.values = (void*) PTAB_chorder;
}

static void line_channel_number(ParsPars& pars)
{
  pars.types = TypePString|TypeRAlign|TypePTable;
  pars.name = (void*) PSTR_channel_number;
  pars.number_of_values = PTAB_chnumber_size;
  pars.values = (void*) PTAB_chnumber;
}

static void line_velocity_curve(ParsPars& pars)
{
  pars.types = TypePString|TypeRAlign|TypePTable;
  pars.name = (void*) PSTR_velocity_curve;
  pars.number_of_values = PTAB_velocity_size;
  pars.values = (void*) PTAB_velocity;
}

static void line_program_change(ParsPars& pars)
{
  pars.types = TypePString|TypeRAlign|TypePTable;
  pars.name = (void*) PSTR_program_change;
  pars.number_of_values = PTAB_progchange_size;
  pars.values = (void*) PTAB_progchange;
}

static void line_split_delta(ParsPars& pars)
{
  pars.types = TypePString|TypeRAlign|TypePTable;
  pars.name = (void*) PSTR_split_delta;
  pars.number_of_values = PTAB_split_size;
  pars.values = (void*) PTAB_split;
}

static void line_brightness(ParsPars& pars)
{
  pars.types = TypePString|TypeRAlign|TypeFunction;
  pars.name = (void*) PSTR_brightness;
  pars.number_of_values = 10;
  pars.values = (void*) GetNumberPlusOne;
}



MenuSettings::MenuSettings(): 
  Menu()
{
  EE::GetSettings(m_values);
  m_lines[0].Init(line_input_channel,   &m_values.input_channel);
  m_lines[1].Init(line_output_channels, &m_values.output_channels);
  m_lines[2].Init(line_channel_order,   &m_values.channel_order);
  m_lines[3].Init(line_channel_number,  &m_values.channel_number);
  m_lines[4].Init(line_velocity_curve,  &m_values.velocity_curve);
  m_lines[5].Init(line_program_change,  &m_values.program_change);
  m_lines[6].Init(line_split_delta,     &m_values.split_delta);
  m_lines[7].Init(line_brightness,      &m_values.brightness);
  // 8: 
  // 9: Undo
  SetNumberOfLines(m_number_of_combilines + 2, g_selected_line, g_first_line); // Must correspond with m_lines definition in .h! + 2
}

const char* MenuSettings::GetTitle()
{
  return GetPString(PSTR_menu_settings);
}

void MenuSettings::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  if (line < m_number_of_combilines) {
    text = m_lines[line].GetText();
    inversion = m_lines[line].GetInversion();
  } else if (line == m_number_of_combilines) {
    text = GetPStringEmpty();
    inversion = {Screen::InvertGiven, 0, 0};
  } else {
    text = GetPStringUndo();
    inversion = Screen::inversion_all;
  }
}

bool MenuSettings::OnLine(LineAction action, uint8_t line)
{
  if (action == SELECTED)
    return false;

  if (line == 1) {
    return ShowChannelMenu();
  } else if (line < m_number_of_combilines) {
    return action == LEFT ? m_lines[line].OnLeft() : m_lines[line].OnRight();
  } else if (line == m_number_of_combilines) {
    return false;
  } else if (line == m_number_of_combilines + 1) {
    return Undo();
  } 
}

void MenuSettings::OnStop(uint8_t selected_line, uint8_t first_line) 
{  
  g_selected_line = selected_line;
  g_first_line = first_line;

  struct SettingsValues m_stored_values;
  EE::GetSettings(m_stored_values);
  if (memcmp(&m_stored_values, &m_values, sizeof(m_values)) != 0) {
    // Values have changed, must save
    EE::SetSettings(m_values);
  }
}

bool MenuSettings::ShowChannelMenu()
{
  Menus::SetNextMenu(MENU_CHANNELS);
  return false;
}

bool MenuSettings::Undo()
{
  EE::GetSettings(m_values);
  return true; // make sure menu gets updated
}

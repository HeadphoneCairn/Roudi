#include "PageSettings.h"
#include "Data.h"
#include "Debug.h"
#include "Pages.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*

·[MIDI Settings]       ·
·Input channel       01·
·Velocity        normal·      normal|custom|none  (none = velocity 64)
·Program change   block·      block|allow   (includes bank select)
·Screen brightness     ·

Future:
·Custom Velocity   MENU·
·CC Mapping        MENU

*/


PSTRING(PSTR_page_settings,   " SETTINGS ");

PSTRING(PSTR_input_channel,   "Input channel");
PSTRING(PSTR_velocity_curve,  "Velocity curve");
PSTRING(PSTR_program_change,  "Program change");
PSTRING(PSTR_brightness,      "Screen brightness");

PSTRING(PSTR_ellipsis_0,        "...");
PTABLE(PTAB_ellipsis, PSTR_ellipsis_0);


PSTRING(PSTR_velocity_0, "soft");
PSTRING(PSTR_velocity_1, "medium");
PSTRING(PSTR_velocity_2, "hard");
PTABLE(PTAB_velocity, PSTR_velocity_0, PSTR_velocity_1, PSTR_velocity_2);

PSTRING(PSTR_progchange_0, "block");
PSTRING(PSTR_progchange_1, "allow");
PTABLE(PTAB_progchange, PSTR_progchange_0, PSTR_progchange_1);



// 3 bytes per line is reached by using function to store parameters
// instead of storing them in memory. Looks horrible, but uses less
// memory.
// Can't do pars = { ... } because it will use more memory again!

static void line_input_channel(ParsPars& pars)
{
  pars.types = TypePString|TypeFunction;
  pars.name = (void*) PSTR_input_channel;
  pars.number_of_values = 16;
  pars.values = (void*) GetNumberPlusOne;
}

static void line_velocity_curve(ParsPars& pars)
{
  pars.types = TypePString|TypePTable;
  pars.name = (void*) PSTR_velocity_curve;
  pars.number_of_values = PTAB_velocity_size;
  pars.values = (void*) PTAB_velocity;
}

static void line_program_change(ParsPars& pars)
{
  pars.types = TypePString|TypePTable;
  pars.name = (void*) PSTR_program_change;
  pars.number_of_values = PTAB_progchange_size;
  pars.values = (void*) PTAB_progchange;
}

static void line_brightness(ParsPars& pars)
{
  pars.types = TypePString|TypeFunction;
  pars.name = (void*) PSTR_brightness;
  pars.number_of_values = 10;
  pars.values = (void*) GetNumberPlusOne;
}



PageSettings::PageSettings(): 
  Page()
  {}

void PageSettings::OnStart(uint8_t)
{
  EE::GetSettings(m_values);
  m_ui_input_channel.Init(line_input_channel, &m_values.input_channel);
  m_ui_velocity_curve.Init(line_velocity_curve, &m_values.velocity_curve);
  m_ui_program_change.Init(line_program_change, &m_values.program_change);
  m_ui_brightness.Init(line_brightness, &m_values.brightness);
  SetNumberOfLines(5);
}

const char* PageSettings::GetTitle()
{
  return GetPString(PSTR_page_settings);
}

Page::LineResult PageSettings::Line(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case 0: return SingleCombiLine(func, m_ui_input_channel,   24, 0, true);
    case 1: return SingleCombiLine(func, m_ui_velocity_curve,  24, 0, true);
    case 2: return SingleCombiLine(func, m_ui_program_change,  24, 0, true);
    case 3: return SingleCombiLine(func, m_ui_brightness,      24, 0, true);
    default: return DefaultLine(func);
  }
}


void PageSettings::OnStop(uint8_t selected_line, uint8_t first_line) 
{  

/*
  struct SettingsValues m_stored_values;
  EE::GetSettings(m_stored_values);
  if (memcmp(&m_stored_values, &m_values, sizeof(m_values)) != 0) {
    // Values have changed, must save
    EE::SetSettings(m_values);
  }
*/
}

bool PageSettings::ShowChannelMenu()
{
//  Menus::SetNextMenu(MENU_CHANNELS);
  return false;
}


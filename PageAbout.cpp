#include "PageAbout.h"

#include "Debug.h"
#include "Data.h"
#include "Roudi.h"

namespace
{
  uint8_t g_first_line = 0; 
  uint8_t g_selected_line = 0;
}


//                      123456789012345678901234
PSTRING(PSTR_about_00, "");
PSTRING(PSTR_about_01, "     === Roudi ===");
PSTRING(PSTR_about_02, "ROUter for miDI messages");
PSTRING(PSTR_about_03, "      version 0.1");
PSTRING(PSTR_about_04, "       2019-2022");
PSTRING(PSTR_about_05, "");
PSTRING(PSTR_about_06, "Made by Headphone Cairn");
PSTRING(PSTR_about_07, "Software for the Blokas");
PSTRING(PSTR_about_08, "midiboy.");
PSTRING(PSTR_about_09, "Parts by blokas a.o.");
PSTRING(PSTR_about_10, "");
PSTRING(PSTR_about_11, "   === Some help ===");
PSTRING(PSTR_about_12, "A or B: switch pages");
PSTRING(PSTR_about_13, "A + B: toggle utils");
PSTRING(PSTR_about_14, "v, ^: switch fields");
PSTRING(PSTR_about_15, "<, >: change values");
PSTRING(PSTR_about_16, "");

PTABLE(PTAB_about_text,
       PSTR_about_00,
       PSTR_about_01,
       PSTR_about_02,
       PSTR_about_03,
       PSTR_about_04,
       PSTR_about_05,
       PSTR_about_06,
       PSTR_about_07,
       PSTR_about_08,
       PSTR_about_09,
       PSTR_about_10,
       PSTR_about_11,
       PSTR_about_12,
       PSTR_about_13,
       PSTR_about_14,
       PSTR_about_15,
       PSTR_about_16,
       );


PSTRING(PSTR_page_about, " ABOUT ROUDI "); 

PageAbout::PageAbout(): Page()
{
}

void PageAbout::OnStart()
{
  SetNumberOfLines(PTAB_about_text_size, g_selected_line, g_first_line);
  SetMidiConfiguration(g_selected_line);
}

const char* PageAbout::GetTitle()
{
  return GetPString(PSTR_page_about);
}

Page::LineResult PageAbout::Line(LineFunction func, uint8_t line, uint8_t field)
{
  return {1, GetPStringFromPTable(PTAB_about_text, line), Screen::inversion_all, false};
}


void PageAbout::OnStop(uint8_t selected_line, uint8_t first_line)
{
  g_first_line = first_line;
}

void PageAbout::SetMidiConfiguration(uint8_t selected_line)
{
  g_next_midi_config.config.SetDefaults();
  g_next_midi_config.config.m_nbr_output_channels = 0;
  g_next_midi_config.go = true;
}

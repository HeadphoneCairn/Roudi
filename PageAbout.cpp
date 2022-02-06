#include "PageAbout.h"

#include "Debug.h"
#include "Data.h"
#include "MemoryFree.h"
#include "MidiProcessing.h"
#include "Pages.h"


PSTRING(PSTR_page_about, " ABOUT ROUDI "); 

//                      1234567890123456789012345
PSTRING(PSTR_about_00, "");
PSTRING(PSTR_about_01, "         Roudi");
PSTRING(PSTR_about_02, "ROUter for miDI messages");
PSTRING(PSTR_about_03, "      version 0.1");
PSTRING(PSTR_about_04, "       2019-2022");
PSTRING(PSTR_about_05, "");
PSTRING(PSTR_about_06, "Made by Headphone Cairn");
PSTRING(PSTR_about_07, "Software for the Blokas");
PSTRING(PSTR_about_08, "midiboy.");
PSTRING(PSTR_about_09, "Parts by blokas, a.o.");
PSTRING(PSTR_about_10, "");
PSTRING(PSTR_about_11, "SOME HELP:");
PSTRING(PSTR_about_12, " A or B: switch pages");
PSTRING(PSTR_about_13, " A + B: toggle utils");
PSTRING(PSTR_about_14, " up, down: switch fields");
PSTRING(PSTR_about_15, " left, right: modify");
PSTRING(PSTR_about_16, "");
PSTRING(PSTR_about_17, "STATUS:");

/*
To add:
- Everything is saved automatically.
*/

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
       PSTR_about_17,
       );



PSTRING(PSTR_about_memory, " free memory: %d bytes");
PSTRING(PSTR_about_pages,  " pages total: %d bytes");
PSTRING(PSTR_about_sizeof, "    -> %s");


PageAbout::PageAbout(): Page()
{
}

void PageAbout::OnStart(uint8_t)
{
  SetNumberOfLines(PTAB_about_text_size + 3);
  SetMidiConfiguration(); // TODO
}

const char* PageAbout::GetTitle()
{
  return GetPString(PSTR_page_about);
}

Page::LineResult PageAbout::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (line < PTAB_about_text_size)
    return {1, GetPStringFromPTable(PTAB_about_text, line), Screen::inversion_all, false};
  else if (line == PTAB_about_text_size)
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_about_memory), freeMemory());
  else if (line == PTAB_about_text_size + 1)
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_about_pages), Pages::GetTotalPageUsage());
  else
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_about_sizeof), Pages::GetPageUsage());
  return {1, Screen::buffer, Screen::inversion_all, false};
}


void PageAbout::OnStop()
{
}

void PageAbout::SetMidiConfiguration()
{
  MidiProcessing::Configuration next_config;
  next_config.m_input_channel = EE::GetSettings().input_channel;
  next_config.m_nbr_output_channels = 0;
  MidiProcessing::SetNextConfiguration(next_config);
}

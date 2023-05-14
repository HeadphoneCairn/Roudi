/*
 * Roudi, a ROUter for miDI messages for Blokas Midiboy
 * Copyright (C) 2019-2023 Headphone Cairn Software
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2 of the License.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


#include "PageAbout.h"

#include "Debug.h"
#include "Data.h"
#include "MemoryFree.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Roudi.h" // for VERSION and ENABLE_MEMORY_STATUS


PSTRING(PSTR_page_about, " ABOUT ROUDI "); 

// alignment           "0123456789     9876543210"
PSTRING(PSTR_about_00, "");
PSTRING(PSTR_about_01, "          Roudi");
PSTRING(PSTR_about_02, "ROUter for miDI messages");
PSTRING(PSTR_about_03, ROUDI_VERSION);
PSTRING(PSTR_about_04, ROUDI_DATE);
PSTRING(PSTR_about_05, "");
PSTRING(PSTR_about_06, "Made by Headphone Cairn");
PSTRING(PSTR_about_07, "Software for the Blokas");
PSTRING(PSTR_about_08, "midiboy.");
PSTRING(PSTR_about_09, "Libs by Blokas et al.");
PSTRING(PSTR_about_10, "");
PSTRING(PSTR_about_11, "SOME HELP:");
PSTRING(PSTR_about_12, " A or B: switch pages");
PSTRING(PSTR_about_13, " A + B: toggle utils");
PSTRING(PSTR_about_14, " up, down: switch fields");
PSTRING(PSTR_about_15, " left, right: modify");
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

#ifdef ENABLE_MEMORY_STATUS
PSTRING(PSTR_about_status,       "STATUS:");
PSTRING(PSTR_about_free_heap,    " free heap mem: %d bytes");
PSTRING(PSTR_about_min_stack,    " min stack mem: %d bytes");
PSTRING(PSTR_about_heap_frag_ok, " heap fragments: %d");
PSTRING(PSTR_about_heap_frag_nok," heap fragments: %d !!!");
PSTRING(PSTR_about_biggest_page, " biggest page: %d bytes");
PSTRING(PSTR_about_sizeof,       "   %s");
#endif


uint8_t PageAbout::m_selected_line = 0;
uint8_t PageAbout::m_first_line = 0;

PageAbout::PageAbout(): Page()
{
}

void PageAbout::OnStart(uint8_t)
{
  SetNumberOfLines(PTAB_about_text_size 
#ifdef ENABLE_MEMORY_STATUS
  + 7
#endif
  , m_selected_line, 0, m_first_line);
  SetMidiConfiguration(); // TODO
}

void PageAbout::OnStop()
{
  m_selected_line = GetSelectedLine();
  m_first_line = GetFirstLine();
}

const char* PageAbout::GetTitle()
{
  return GetPString(PSTR_page_about);
}

Page::LineResult PageAbout::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (line < PTAB_about_text_size)
    return {1, GetPStringFromPTable(PTAB_about_text, line), Screen::inversion_all, false};
#ifdef ENABLE_MEMORY_STATUS
  else if (line == PTAB_about_text_size)
    return TextLine(func, PSTR_about_status);
  else if (line == PTAB_about_text_size + 1)
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_about_free_heap), getFreeMemory()-getMallocMargin());
  else if (line == PTAB_about_text_size + 2)
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_about_min_stack), getMallocMargin());
  else if (line == PTAB_about_text_size + 3)
    snprintf(Screen::buffer, sizeof(Screen::buffer), 
            GetPString((getNumberOfHeapFragments() == 1) ? PSTR_about_heap_frag_ok: PSTR_about_heap_frag_nok), 
            getNumberOfHeapFragments());
  else if (line == PTAB_about_text_size + 4)
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_about_biggest_page), Pages::GetBiggestPageUsage());
  else if (line == PTAB_about_text_size + 5)
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_about_sizeof), Pages::GetPageUsage(0));
  else
    snprintf(Screen::buffer, sizeof(Screen::buffer), GetPString(PSTR_about_sizeof), Pages::GetPageUsage(1));
  return {1, Screen::buffer, Screen::inversion_all, false};
#else
  return DefaultLine(func);
#endif
}

void PageAbout::SetMidiConfiguration()
{
  MidiProcessing::Configuration next_config;
  next_config.m_input_channel = EE::Settings().input_channel;
  next_config.m_nbr_output_channels = 0;
  MidiProcessing::SetNextConfiguration(next_config);
}

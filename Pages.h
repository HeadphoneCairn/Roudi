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


#pragma once

#include <Arduino.h> // for uint8_t
#include "Roudi.h"

enum PageID
{
  // --- Lower pages ---
  PAGE_SINGLE           = 0,
  PAGE_MULTI            = 1,
  PAGE_ABOUT            = 2,

  // --- Upper pages ---
  PAGE_MONITOR          = 10,
  PAGE_SETTINGS         = 11,
  PAGE_MONITOR_SETTINGS = 12,

  // --- Popup pages ---
  PAGE_SINGLE_NAME      = 101,
  PAGE_MULTI_NAME       = 102,
  PAGE_MULTI_REMOVE     = 103,
  PAGE_VELOCITY_SELECT  = 104,
  PAGE_VELOCITY_EDIT    = 105,
  
  // --- Special pages ---
  PAGE_NONE         = 255
};


namespace Pages
{
  // Start the first page
  void Start();

  // Sometimes a page wants to start another page: e.g. PageSettings starts PageChannels.
  // Doing this directly can cause problems because showing a page means overwriting the
  // active page => problematic members, problematic stack
  // So, the starting of the page is postponed until we out of the Page level and 
  // back at Pages level.
  void SetNextPage(PageID page_id, uint8_t data = 0xFF); 

  // Act on pushed buttons
  void ButtonUp();
  void ButtonDown();
  void ButtonLeft();
  void ButtonRight();
  void ButtonA();
  void ButtonB();
  void ButtonAB();

  // Timeout
  void Timeout();
  const unsigned long timeout =  // in ms, OnTimeout is called on the active page when no button has been pushed for this time 
#ifdef ENABLE_LONG_AUTOSAVE_TIMEOUT
    /* 15 minutes */ 900L
#else
    /* 10 seconds */ 10L
#endif
    * 1000L;

  // Redraw the current page
  void Redraw();

  // Debug info
  uint16_t GetBiggestPageUsage();
  const char* GetPageUsage(uint8_t part);
}

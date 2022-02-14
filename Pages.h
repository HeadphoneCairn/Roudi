#pragma once

#include <Arduino.h> // for uint8_t

enum PageID
{
  // --- Lower pages ---
  PAGE_SINGLE       = 0,
  PAGE_MULTI        = 1,
  PAGE_ABOUT        = 2,

  // --- Upper pages ---
  PAGE_MONITOR      = 10,
  PAGE_SETTINGS     = 11,

  // --- Popup pages ---
  PAGE_NAME_CHANNEL = 101,
  PAGE_NAME_MULTI   = 102,
  
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
  const unsigned long timeout = 900 * 1000; // in ms, OnTimeout is called on the active page when no button has been pushed for this time 

  // Redraw the current page
  void Redraw();

  // Debug info
  uint16_t GetBiggestPageUsage();
  const char* GetPageUsage();
}
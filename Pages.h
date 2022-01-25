#pragma once

#include <Arduino.h> // for uint8_t

enum PageID
{
  // --- AB pages ---
  PAGE_SINGLE       = 0,
  PAGE_MULTI        = 1,
  PAGE_SETTINGS     = 2,
  PAGE_ABOUT        = 3,

  // --- Popup pages ---
  PAGE_CHANNELS     = 100,
  PAGE_NAME_CHANNEL = 101,
  PAGE_NAME_MULTI   = 102,
  
  // --- Special pages ---
  PAGE_DEBUG        = 254,
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

  void ButtonUp();
  void ButtonDown();
  void ButtonLeft();
  void ButtonRight();
  void ButtonA();
  void ButtonB();
  void ButtonAB();

  uint16_t GetTotalPageUsage();
  const char* GetPageUsage();
}
#include "Pages.h"

#include "Pages.h"
#include "PageSingle.h"
#include "PageMulti.h"
#include "PageSettings.h"
//#include "PageNameChannel.h"
//#include "PageNamePreset.h"

#include "Data.h"
#include "Screen.h"

#include <stdio.h>


#include "PageSingle.h"
#include "PageMulti.h"
#include "PageSettings.h"



/* Pages overview

  Lower level:
  0. PageSingle
  1. PageMulti -------- PageNamePreset
  n. PageAbout

  Upper level:
  1.0  PageMonitor
  1.1. PageSettings ----- PageChannels ----- PageNameChannel
*/


namespace 
{
  PageSingle    g_page_single;
  PageMulti     g_page_multi;
  PageSettings  g_page_settings;

  PageID g_current_lower_id = PAGE_SINGLE;
  PageID g_current_upper_id = PAGE_SETTINGS;
  Page*  g_current_page     = nullptr;
  bool   g_current_lower    = true;


  // The page that should be started when Left/Right/Up/Down returns
  // if so requested by the current page. 
  PageID g_next_page_id = PAGE_NONE;
  uint8_t g_next_page_data = 0xFF;



}

namespace Pages
{
  void ShowPage(PageID page_id, uint8_t data = 0xFF, uint8_t selected_line = 0, uint8_t first_line = 0xFF)
  {
    // --- Stop previous page ---
    if (g_current_page)
      g_current_page->Stop();
    
    // --- Start requested page ---
    if (g_current_lower)
      g_current_lower_id = page_id;
    else
      g_current_upper_id = page_id;
    switch(page_id) {
      case PAGE_SINGLE:   g_current_page = &g_page_single; break;
      case PAGE_MULTI:    g_current_page = &g_page_multi; break;
      case PAGE_SETTINGS: g_current_page = &g_page_settings; break;
      default:            g_current_page = &g_page_single; break;
    }
    g_current_page->Start();
  }

  void ShowNextPageIfRequested()
  {
    if (g_next_page_id != PAGE_NONE) {
      ShowPage(g_next_page_id, g_next_page_data);
      g_next_page_id = PAGE_NONE;
      g_next_page_data = 0xFF;
    }
  }

  void Start()
  {
    ShowPage(g_current_lower_id);
  }

  void SetNextPage(PageID page_id, uint8_t data)
  {
    g_next_page_id = page_id;
    g_next_page_data = data;
  }

  void ButtonUp()
  {
    g_current_page->Up();
    ShowNextPageIfRequested();
  }

  void ButtonDown()
  {
    g_current_page->Down();
    ShowNextPageIfRequested();
  }

  void ButtonLeft()
  {
    g_current_page->Left();
    ShowNextPageIfRequested();
  }

  void ButtonRight()
  {
    g_current_page->Right();
    ShowNextPageIfRequested();
  }
  
  void ButtonA() 
  {
    // Ignore A/B for some pages
    //if (g_page == g_page_channels ||
    //    g_page == p_page_name_channel)
    //  return;

    // Go to previous page
    PageID page_to_show = PAGE_SINGLE; // fallback  // TODO make this about
    if (g_current_lower) {
      switch (g_current_lower_id) {
        case PAGE_SINGLE:   page_to_show = PAGE_MULTI; break;
        case PAGE_MULTI:    page_to_show = PAGE_SINGLE; break;
      }
    } else {
      switch (g_current_upper_id) {
        case PAGE_SETTINGS: page_to_show = PAGE_SETTINGS; break;
      }
    }

    ShowPage(page_to_show);
  }

  void ButtonB()
  {
    // Ignore A/B for some pages
    //if (g_page == g_page_channels ||
    //    g_page == p_page_name_channel)
    //  return;

    // Go to next page
    PageID page_to_show = PAGE_SINGLE; // fallback  // TODO make this about
    if (g_current_lower) {
      switch (g_current_lower_id) {
        case PAGE_SINGLE:   page_to_show = PAGE_MULTI; break;
        case PAGE_MULTI:    page_to_show = PAGE_SINGLE; break;
      }
    } else {
      switch (g_current_upper_id) {
        case PAGE_SETTINGS: page_to_show = PAGE_SETTINGS; break;
      }
    }

    ShowPage(page_to_show);
  }  

  void ButtonAB()
  {
    g_current_lower = !g_current_lower;
    ShowPage(g_current_lower ? g_current_lower_id : g_current_upper_id);
  }


  uint16_t GetTotalPageUsage()
  {
    return sizeof(PageSingle) + 
           sizeof(PageMulti) +
           sizeof(PageSettings);
      //sizeof(PageChannels) +
      //sizeof(PageNameChannel);
  }

}
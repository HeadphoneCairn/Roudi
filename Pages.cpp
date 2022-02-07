#include "Pages.h"

#include "PageAbout.h"
#include "PageChannels.h"
#include "PageMonitor.h"
#include "PageMulti.h"
#include "PageNameChannel.h"
#include "PageNameMulti.h"
#include "PageSettings.h"
#include "PageSingle.h"

#include "Debug.h"
#include "Data.h"
#include "Screen.h"

#include <stdio.h>


/* Pages overview

  Lower level:
  0. PageSingle ------- PageNameChannel
  1. PageMulti -------- PageNameMulti
  n. PageAbout

  Upper level:
  1.0  PageMonitor
  1.1. PageSettings
*/


namespace 
{
  // NOTE: Could possibly save some memory by having only one page in memory at
  //       a time by allocating the pages dynamically into a fixed size buffer,
  //       like I did in my old version. But that adds complexity, which is the
  //       reason I removed it.
  PageSingle    g_page_single;
  PageNameChannel g_page_name_channel;
  PageMulti     g_page_multi;
  PageNameMulti g_page_name_multi;
  PageAbout     g_page_about;
  PageMonitor   g_page_monitor;
  PageSettings  g_page_settings;

  PageID g_current_lower_id = PAGE_SINGLE;
  PageID g_current_upper_id = PAGE_MONITOR;
  Page*  g_current_page     = nullptr;
  bool   g_current_lower    = true;
  uint8_t g_current_multi   = 0;


  // The page that should be started when Left/Right/Up/Down returns
  // if so requested by the current page. 
  PageID g_next_page_id = PAGE_NONE;
  uint8_t g_next_page_data = 0xFF;
}

namespace Pages
{
  void ShowPage(PageID page_id, uint8_t data = 0xFF /*, uint8_t selected_line = 0, uint8_t first_line = 0xFF*/)
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
      case PAGE_MULTI:    
        g_current_page = &g_page_multi; 
        if (data == 0xFF)
          data = g_current_multi; 
        else 
          g_current_multi = data; 
        break;
      case PAGE_ABOUT:    g_current_page = &g_page_about; break;
      case PAGE_MONITOR:  g_current_page = &g_page_monitor; break;
      case PAGE_SETTINGS: g_current_page = &g_page_settings; break;
      case PAGE_NAME_CHANNEL: g_current_page = &g_page_name_channel; break;
      case PAGE_NAME_MULTI:   g_current_page = &g_page_name_multi; break;
      default:            g_current_page = &g_page_single; break;
    }
    g_current_page->Start(data);
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
    if (g_current_page == &g_page_name_channel ||
        g_current_page == &g_page_name_multi)
      return;

    // Go to previous page
    uint8_t data = 0xFF;
    PageID page_to_show = PAGE_ABOUT; // fallback
    if (g_current_lower) {
      switch (g_current_lower_id) {
        case PAGE_SINGLE:  
          page_to_show = PAGE_ABOUT;
          break;
        case PAGE_MULTI:
          if (g_current_multi > 0) {
            data = g_current_multi - 1;
            page_to_show = PAGE_MULTI;
          } else
            page_to_show = PAGE_SINGLE;
          break;
        case PAGE_ABOUT:   
          page_to_show = PAGE_MULTI;
          data = EE::GetNumberOfMultis() - 1; 
          break;
      }
    } else {
      switch (g_current_upper_id) {
        case PAGE_MONITOR: page_to_show = PAGE_SETTINGS; break;
        case PAGE_SETTINGS: page_to_show = PAGE_MONITOR; break;
      }
    }
  
    ShowPage(page_to_show, data);
  }

  void ButtonB()
  {
    // Ignore A/B for some pages
    if (g_current_page == &g_page_name_channel ||
        g_current_page == &g_page_name_multi)
      return;

    // Go to next page
    uint8_t data = 0xFF;
    PageID page_to_show = PAGE_ABOUT; // fallback
    if (g_current_lower) {
      switch (g_current_lower_id) {
        case PAGE_SINGLE:   
          page_to_show = PAGE_MULTI;
          data = 0; // new current multi 
          break;
        case PAGE_MULTI:
          if (g_current_multi + 1 < EE::GetNumberOfMultis()) {
            data = g_current_multi + 1;
            page_to_show = PAGE_MULTI;
          } else 
            page_to_show = PAGE_ABOUT; 
          break;
        case PAGE_ABOUT:    
          page_to_show = PAGE_SINGLE; break;
      }
    } else {
      switch (g_current_upper_id) {
        case PAGE_MONITOR: page_to_show = PAGE_SETTINGS; break;
        case PAGE_SETTINGS: page_to_show = PAGE_MONITOR; break;
      }
    }

    ShowPage(page_to_show, data);
  }  

  void ButtonAB()
  {
    // Ignore A/B for some pages
    if (g_current_page == &g_page_name_channel ||
        g_current_page == &g_page_name_multi)
      return;

    g_current_lower = !g_current_lower;
    ShowPage(g_current_lower ? g_current_lower_id : g_current_upper_id);
  }

  void Timeout()
  {
    // TODO Save the selected page to EEPROM
    g_current_page->Timeout();
  }

  void Redraw()
  {
    g_current_page->Redraw();
  }

  uint16_t GetTotalPageUsage()
  {
    return sizeof(PageSingle) + sizeof(PageNameChannel) +
           sizeof(PageMulti) + sizeof(PageNameMulti) +
           sizeof(PageMonitor) + sizeof(PageSettings) + 
           sizeof(PageAbout);
  }

  PSTRING(PSTR_page_usage, "%d %d %d %d %d %d %d");
  const char* GetPageUsage()
  {
    snprintf(data_scratch, sizeof(data_scratch), GetPString(PSTR_page_usage),
             sizeof(PageSingle), sizeof(PageNameChannel),
             sizeof(PageMulti), sizeof(PageNameMulti),
             sizeof(PageMonitor), sizeof(PageSettings), 
             sizeof(PageAbout));
    return data_scratch;
  }

}
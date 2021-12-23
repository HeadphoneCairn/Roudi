#include "Menus.h"

#include "Menu.h"
#include "MenuChannels.h"
#include "MenuDebug.h"
#include "MenuLayer.h"
#include "MenuNameChannel.h"
#include "MenuNamePreset.h"
#include "MenuSettings.h"
#include "MenuSingle.h"
#include "MenuSplit.h"

#include "Data.h"
#include "Screen.h"

#include <new>
#include <stdio.h>


/* Menus overview

  MenuNameChannel
  MenuNamePreset
  MenuChannels

  0. MenuSingle
  1. MenuSplit -------- MenuNamePreset
  2. MenuLayer -------- MenuNamePreset
  3. Presets ---------- MenuNamePreset
  4. MenuSettings ----- MenuChannels ----- MenuNameChannel
  5. MenuDebug
*/


namespace 
{
  PSTRING(PSTR_bad_buffer_1, "gMenuBufferSize is %d,");
  PSTRING(PSTR_bad_buffer_2, "but it should be %d!");

  // The currently used menu
  MenuID g_current_menu_id = MENU_NONE;

  // The menu that should be started when Left/Right/Up/Down returns
  // if so requested by the current menu. 
  MenuID g_next_menu_id = MENU_NONE;
  uint8_t g_next_menu_data = 0xFF;
}

namespace Menus
{
  void ShowMenu(MenuID menu, uint8_t data = 0xFF, uint8_t selected_line = 0, uint8_t first_line = 0xFF)
  {
    // --- Stop previous menu ---
    if (g_current_menu_id != MENU_NONE)
      g_menu->Stop();

    // --- Start requested menu ---
    g_current_menu_id = menu;
    // All allocations of Menus is done in the same memory block
    // to prevent heap fragmentation.
    switch (g_current_menu_id) {
      case MENU_SINGLE:       new (g_menu) MenuSingle(); break;
      case MENU_SPLIT:        new (g_menu) MenuSplit(); break;
      case MENU_LAYER:        new (g_menu) MenuLayer(); break;
      case MENU_SETTINGS:     new (g_menu) MenuSettings(); break;
      case MENU_CHANNELS:     new (g_menu) MenuChannels(); break;
      case MENU_NAME_CHANNEL: new (g_menu) MenuNameChannel(data); break;
      case MENU_DEBUG:        new (g_menu) MenuDebug(); break; 
      default:                new (g_menu) MenuSingle(); break;
    }
    g_menu->Start();
  }

  void ShowNextMenuIfRequested()
  {
    if (g_next_menu_id != MENU_NONE) {
      ShowMenu(g_next_menu_id, g_next_menu_data);
      g_next_menu_id = MENU_NONE;
      g_next_menu_data = 0xFF;
    }
  }

  void Start()
  {
    const uint8_t max_menu_size = 
      max(sizeof(MenuSingle), 
      max(sizeof(MenuSplit), 
      max(sizeof(MenuLayer), 
      max(sizeof(MenuSettings), 
      max(sizeof(MenuDebug), 
      max(sizeof(MenuChannels), sizeof(MenuNameChannel))
      )))));
    if (max_menu_size != gMenuBufferSize) {
      Screen::Clear();
      sprintf(Screen::buffer, GetPString(PSTR_bad_buffer_1), gMenuBufferSize);
      Screen::Print(Screen::CanvasComplete, 3, 0xFF, Screen::buffer, Screen::LineClear, {Screen::InvertNone, 0, 0});
      sprintf(Screen::buffer, GetPString(PSTR_bad_buffer_2), max_menu_size);
      Screen::Print(Screen::CanvasComplete, 4, 0xFF, Screen::buffer, Screen::LineClear, {Screen::InvertNone, 0, 0});
    } else {
      ShowMenu(MENU_SINGLE);
    }
  }

  void SetNextMenu(MenuID menu, uint8_t data)
  {
    g_next_menu_id = menu;
    g_next_menu_data = data;
  }

  void ButtonUp()
  {
    g_menu->Up();
    ShowNextMenuIfRequested();
  }

  void ButtonDown()
  {
    g_menu->Down();
    ShowNextMenuIfRequested();
  }

  void ButtonLeft()
  {
    g_menu->Left();
    ShowNextMenuIfRequested();
  }

  void ButtonRight()
  {
    g_menu->Right();
    ShowNextMenuIfRequested();
  }
  
  void ButtonA() 
  {
    // Ignore A/B for some menus
    if (g_current_menu_id == MENU_CHANNELS ||
        g_current_menu_id == MENU_NAME_CHANNEL)
      return;

    // Go to previous menu for standard menus
    MenuID menu_to_show = MENU_SINGLE; // fallback
    switch (g_current_menu_id) {
      case MENU_SINGLE: menu_to_show = MENU_DEBUG; break;
      case MENU_SPLIT: menu_to_show = MENU_SINGLE; break;
      case MENU_LAYER: menu_to_show = MENU_SPLIT; break;
      case MENU_SETTINGS: menu_to_show = MENU_LAYER; break;
      case MENU_DEBUG: menu_to_show = MENU_SETTINGS; break;
    }
    ShowMenu(menu_to_show);
  }

  void ButtonB()
  {
    // Ignore A/B for some menus
    if (g_current_menu_id == MENU_CHANNELS ||
        g_current_menu_id == MENU_NAME_CHANNEL)
      return;

    MenuID menu_to_show = MENU_SINGLE; // fallback
    switch (g_current_menu_id) {
      case MENU_SINGLE: menu_to_show = MENU_SPLIT; break;
      case MENU_SPLIT: menu_to_show = MENU_LAYER; break;
      case MENU_LAYER: menu_to_show = MENU_SETTINGS; break;
      case MENU_SETTINGS: menu_to_show = MENU_DEBUG; break;
      case MENU_DEBUG: menu_to_show = MENU_SINGLE; break;
    }
    ShowMenu(menu_to_show);
  }  

}
#if 0

#pragma once

#include <Arduino.h> // for uint8_t

enum MenuID
{
  // --- AB menus ---
  MENU_SINGLE       = 0,
  MENU_SPLIT        = 1,
  MENU_LAYER        = 2,
  // There used to be PRESET menus here
  MENU_SETTINGS     = 50,  
  // --- Popup menus ---
  MENU_CHANNELS     = 100,
  MENU_NAME_CHANNEL = 101,
  // --- Special menus ---
  MENU_DEBUG        = 254,
  MENU_NONE         = 255
};


namespace Menus
{
  // Start the first menu
  void Start();

  // Sometimes a menu wants to start another menu: e.g. MenuSettings starts MenuChannels.
  // Doing this directly can cause problems because showing a menu means overwriting the
  // active menu => problematic members, problematic stack
  // So, the starting of the menu is postponed until we out of the Menu level and 
  // back at Menus level.
  void SetNextMenu(MenuID menu, uint8_t data = 0xFF); 

  void ButtonUp();
  void ButtonDown();
  void ButtonLeft();
  void ButtonRight();
  void ButtonA();
  void ButtonB();  
}

#endif
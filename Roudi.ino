/*

TODO
- Scrollbar for the menu
- Read from EEPROM
- kop logo + naam bedenken
- rotate values if beyond end
- channel visualization

  Text buffers (length of screen line):
  - Screen::buffer: fill it and supply it to Screen::Print
  - progmem_string_buffer: used for reading string from PROGMEM
  - data_scratch: names read from eeprom and for all other things

 */

#include "Roudi.h"
#include "Debug.h"
#include "DinMidiboy.h"
#include "Data.h"
#include "Logo.h"
#include "Menus.h"
#include "MidiProcessing.h"
#include "PageMulti.h"

NextMidiConfiguration g_next_midi_config;

PageMulti multi;

void setup()
{
  DinMidiboy.begin();
  DinMidiboy.setButtonRepeatMs(50);

  EE::Init(); // Create the EEPROM data if it doesn't exist yet.
  MidiProcessing::Init(); // Initialize the MIDI processing subsystem

  //ShowLogo();
  //delay(1000);
  //Menus::Start(); // Start showing the menus
  multi.Start();  


  // For some reason, after boot of the program, the input
  // event queue thinks button A was released. So before
  // we start processing, we clean the input queue.
  DinMidiboy.think();
  MidiboyInput::Event event;
  while (DinMidiboy.readInputEvent(event));
}

void loop()
{
  // --- Get button and midi messages ---
  DinMidiboy.think();

  // --- Parse input midi messages and write result to output queu ---
  MidiProcessing::TreatInput();

  // --- Treat buttons ---
  // up, down, left, right trigger on push and will get repeats every x ms
  // a and b are triggered on release, since we do not need repeat and we want
  //                                   to be able to detect double button release
  static unsigned long last_A = 0; // last time A was released
  static unsigned long last_B = 0; // last time A was released
  const  unsigned long max_A_B_diff = 25; // max number of time difference to wait for A and B to be depressed (actually released)
  unsigned long now = millis();
  //Debug::Print("%lu, %lu, %lu", now, last_A, last_B);
  if (last_A && last_B && last_A + max_A_B_diff > now && last_B + max_A_B_diff > now) {
    last_A = last_B = 0;
    Debug::Beep();
  }
  if (last_A && last_A + max_A_B_diff < now) {
    last_A = 0;
    //Menus::ButtonA();
  }
  if (last_B && last_B + max_A_B_diff < now) {
    last_B = 0;
    //Menus::ButtonB();
  }

  MidiboyInput::Event event;
  while (DinMidiboy.readInputEvent(event))
  {

    if (event.m_type == MidiboyInput::EVENT_DOWN)
    {
      switch (event.m_button)
      {
      case MidiboyInput::BUTTON_UP:
        multi.Up(); //Menus::ButtonUp();
        break;
      case MidiboyInput::BUTTON_DOWN:
        multi.Down(); //Menus::ButtonDown();
        break;
      case MidiboyInput::BUTTON_LEFT:
        multi.Left(); //Menus::ButtonLeft();
        break;
      case MidiboyInput::BUTTON_RIGHT:
        multi.Right(); //Menus::ButtonRight();
        break;
      default:
        break;
      }
    } else {
      switch (event.m_button)
      {
      case MidiboyInput::BUTTON_A:        
        last_A = millis();
        break;
      case MidiboyInput::BUTTON_B:
        last_B = millis();
        break;
      default:
        break;
      }
    }

  }

  // --- Change the midig configuration ---
  // When one of the menus wants to change the midi processing, it will fill in g_next_midi_config.config
  // with the required config and set g_next_midi_config.go to true; 
  if (g_next_midi_config.go) {
    bool success = MidiProcessing::SetConfiguration(g_next_midi_config.config);
    g_next_midi_config.go = !success;
  }

  // --- Write to MIDI ---
  MidiProcessing::WriteToOutput();
}

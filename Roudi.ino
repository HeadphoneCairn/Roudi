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
#include "DinMidiboy.h"
#include "Data.h"
#include "Logo.h"
#include "Menus.h"
#include "MidiProcessing.h"

NextMidiConfiguration g_next_midi_config;

void setup()
{
  DinMidiboy.begin();
  DinMidiboy.setButtonRepeatMs(50);

  EE::Init(); // Create the EEPROM data if it doesn't exist yet.
  MidiProcessing::Init(); // Initialize the MIDI processing subsystem

/*
  Screen::Clear(true);
  delay(60000);  
*/

  ShowLogo();
  delay(1000);
  Menus::Start(); // Start showing the menus
}

void loop()
{
  // --- Get button and midi messages ---
  DinMidiboy.think();

  // --- Parse input midi messages and write result to output queu ---
  MidiProcessing::TreatInput();

  // --- Treat buttons ---
  MidiboyInput::Event event;
  while (DinMidiboy.readInputEvent(event))
  {
    if (event.m_type == MidiboyInput::EVENT_DOWN)
    {
      switch (event.m_button)
      {
      case MidiboyInput::BUTTON_UP:
        Menus::ButtonUp();
        break;
      case MidiboyInput::BUTTON_DOWN:
        Menus::ButtonDown();
        break;
      case MidiboyInput::BUTTON_LEFT:
        Menus::ButtonLeft();
        break;
      case MidiboyInput::BUTTON_RIGHT:
        Menus::ButtonRight();
        break;
      case MidiboyInput::BUTTON_A:
        Menus::ButtonA();
        break;
      case MidiboyInput::BUTTON_B:
        Menus::ButtonB();
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

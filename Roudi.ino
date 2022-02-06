#include "Roudi.h"
#include "Debug.h"
#include "DinMidiboy.h"
#include "Data.h"
#include "Logo.h"
#include "Pages.h"
#include "MidiProcessing.h"

const bool g_show_logo = false;
unsigned long g_last_button_press = 0;  // in ms

void setup()
{
  DinMidiboy.begin();
  DinMidiboy.setButtonRepeatMs(50);

  EE::Init(); // Create the EEPROM data if it doesn't exist yet.
  MidiProcessing::Init(); // Initialize the MIDI processing subsystem

  if (g_show_logo)
    ShowLogo();

  // For some reason, after boot of the program, the input
  // event queue receives an up A, up >, up v, up < and up ^,
  // after about 50 ms, so before we start we start working, 
  // we sleep 100 ms and then we clean the input queue.
  delay(100);
  DinMidiboy.think();
  MidiboyInput::Event event;
  while (DinMidiboy.readInputEvent(event));

  if (g_show_logo)
    delay(1000);

  g_last_button_press = millis();

  Pages::Start(); // Start showing the menus
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
    Pages::ButtonAB();
  }
  if (last_A && last_A + max_A_B_diff < now) {
    last_A = 0;
    Pages::ButtonA();
  }
  if (last_B && last_B + max_A_B_diff < now) {
    last_B = 0;
    Pages::ButtonB();
  }

  MidiboyInput::Event event;
  while (DinMidiboy.readInputEvent(event))
  {
    if (event.m_type == MidiboyInput::EVENT_DOWN)
    {
      switch (event.m_button)
      {
      case MidiboyInput::BUTTON_UP:
        Pages::ButtonUp();
        break;
      case MidiboyInput::BUTTON_DOWN:
        Pages::ButtonDown();
        break;
      case MidiboyInput::BUTTON_LEFT:
        Pages::ButtonLeft();
        break;
      case MidiboyInput::BUTTON_RIGHT:
        Pages::ButtonRight();
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
    g_last_button_press = millis();
  }

  // Check for timeout since last button press
  if (g_last_button_press && (millis() - g_last_button_press > Pages::timeout)) {
    Pages::Timeout();
    g_last_button_press = 0;
  }

  // --- Change the MIDI configuration if needed ---
  MidiProcessing::ActivateNextConfigurationIfAvailable();

  // --- Write to MIDI ---
  MidiProcessing::WriteToOutput();
}



/*
  char keys[200] ="";

  MidiboyInput::Event event;
  while (DinMidiboy.readInputEvent(event))
  {
    switch (event.m_type) 
    {
       case MidiboyInput::EVENT_DOWN: strcat(keys, "d"); break;
       case MidiboyInput::EVENT_UP:   strcat(keys, "u"); break;
       default:                       strcat(keys, "-"); break;
    }
    switch (event.m_button)
    {
    case MidiboyInput::BUTTON_UP:   strcat(keys, "^"); break;
    case MidiboyInput::BUTTON_DOWN: strcat(keys, "v"); break;
    case MidiboyInput::BUTTON_LEFT: strcat(keys, "<"); break;
    case MidiboyInput::BUTTON_RIGHT:strcat(keys, ">"); break;
    case MidiboyInput::BUTTON_A:    strcat(keys, "A"); break;
    case MidiboyInput::BUTTON_B:    strcat(keys, "B"); break;
    default:                        strcat(keys, "?"); break;
    }
  }

  Debug::Print("%s", keys);
*/

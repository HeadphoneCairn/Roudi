#pragma once

#include "MidiboyDisplay.h"
#include "MidiboyDIN5MIDI.h"
#include "MidiboyInput.h"
#include "MidiboyFonts.h"

// The DinMidiboy class is identical to the Blokas created Midiboy class,
// but with the USB MIDI functionality removed.
// This reduces the footprint quite nicely:  
// - 250 bytes of dynamic memory is freed
// - > 2200 bytes of program storage is freed 
// See Midiboy.h for information about the functionality.
class _DinMidiboy : public MidiboyDisplay, public MidiboyDIN5MIDI, public MidiboyInput
{
public:
	// Initialize Midiboy and all its parent classes. Call this function from your setup().
	void begin();

	// Tells Midiboy to think a bit, should be called from your loop(), in case you have blocking internal event
	// loops within your loop() logic, the think() should be called from inside them, to keep USB and button input responsive.
	void think();
};

extern _DinMidiboy DinMidiboy;

/*
 * Roudi, a ROUter for miDI messages for Blokas Midiboy
 * Copyright (C) 2019-2023 Headphone Cairn Software
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2 of the License.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


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

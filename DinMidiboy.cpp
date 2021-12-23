#include "DinMidiboy.h"

_DinMidiboy DinMidiboy;

void _DinMidiboy::begin()
{
	MidiboyDisplay::begin();
	MidiboyDIN5MIDI::begin();
	MidiboyInput::begin();
}

void _DinMidiboy::think()
{
	MidiboyInput::think();
}

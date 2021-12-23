#pragma once

#include "MidiProcessing.h"

class NextMidiConfiguration
{
public:
  MidiProcessing::Configuration config;
  bool go = false;
};

extern NextMidiConfiguration g_next_midi_config;


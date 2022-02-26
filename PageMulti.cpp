#include "PageMulti.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Utils.h"

namespace {


  #define SPLIT_MODE 0
  #define LAYER_MODE 1
  #define SINGLE_MODE 2
  PSTRING(PSTR_mode_split, "SPLIT mode");
  PSTRING(PSTR_mode_layer, "LAYER mode");
  PSTRING(PSTR_single_layer, "SINGLE mode");
  PTABLE(PTAB_mode, PSTR_mode_split, PSTR_mode_layer, PSTR_single_layer);
  PTABLE_GETTER(GetMode, PTAB_mode);

  PSTRING(PSTR_save_as, "> Save As ...");
  PSTRING(PSTR_remove, "> Remove ...");
  PSTRING(PSTR_move_left, "> Move left");
  PSTRING(PSTR_move_right, "> Move right");
  PSTRING(PSTR_new, "> New");

}


PageMulti::PageMulti(): 
  Page(), m_which(0xFF)
{ 
  GetMultiDefault(m_values);
}



void PageMulti::OnStart(uint8_t which_multi)
{
  m_which = which_multi;
  EE::GetMulti(m_which, m_values);
  SetNumberOfLines(11, m_values.selected_line, m_values.selected_field, m_values.first_line);
  SetMidiConfiguration();
}

void PageMulti::OnStop()
{
  SaveIfModified();  
}

void PageMulti::OnTimeout()
{
  SaveIfModified();
}

const char* PageMulti::GetTitle()
{
  //sprintf(Screen::buffer, " %02d/%02d. %s ", m_which + 1, EE::GetNumberOfMultis(), m_values.name);
  sprintf(Screen::buffer, " %02d. %s ", m_which + 1, m_values.name);
  return Screen::buffer;
}


Page::LineResult PageMulti::Line(LineFunction func, uint8_t line, uint8_t field)
{
  LineResult result = ActualLine(func, line, field);
  if (result.redraw) // A setting has changed, so we update the midi configuration
    SetMidiConfiguration();
  return result;
}




// TODO check if PTAB with two values uses less code storage than hardcoded 
// TODO return somthing else than Empty()!
// TODO replace values in BoolLine by a function and call it SingleLine of zoiets
// TODO could have function that takes the max + function ptr (uint8_t): less memory?


const char* GetSplit(uint8_t i_value, uint8_t& o_number_of_values)
{
  o_number_of_values = 128;
  if (i_value < o_number_of_values)
    return GetNoteName(i_value);
  else
    return GetPStringUnknownValue();
}

const char* GetChannel(uint8_t i_value, uint8_t& o_number_of_values)
{
  o_number_of_values = NumberOfChannels;
  if (i_value < o_number_of_values)
    return EE::GetChannelNameFormatted(i_value);
  else
    return GetPStringUnknownValue();
}

const char* GetOctave(uint8_t i_value, uint8_t& o_number_of_values)
{
  o_number_of_values = GetNumberOfOctaves(); 
  if (i_value < o_number_of_values)
    return GetOctaveName(i_value);
  else
    return GetPStringUnknownValue();
}

// TODO check if the PTAB version is more efficient
PSTRING(PSTR_on_off_off, "off");
PSTRING(PSTR_on_off_on, "on");
const char* GetOnOff(uint8_t i_value, uint8_t& o_number_of_values)
{
  o_number_of_values = 2;
  return GetPString(i_value ? PSTR_on_off_on : PSTR_on_off_off);
}


PSTRING(PSTR_percentage_format, "%d%%");
const char* GetVelocity(uint8_t i_value, uint8_t& o_number_of_values)
{
  o_number_of_values = 10;
  sprintf(data_scratch, GetPString(PSTR_percentage_format), i_value*10);
  return data_scratch;
}


PSTRING(PSTR_multi_split,     "split at ");
PSTRING(PSTR_multi_channel,   "channel");
PSTRING(PSTR_multi_octave,    "octave");
PSTRING(PSTR_multi_pbcc,      "pb/cc");
PSTRING(PSTR_multi_velocity,  "velocity");

Page::LineResult PageMulti::ActualLine(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case 0: return DoubleLine(func, field, PSTR_multi_split, 13, m_values.mode, GetMode, m_values.split_note, GetSplit);
    case 1: return DoubleLine(func, field, PSTR_multi_channel, 0xFF, m_values.channel[0], GetChannel, m_values.channel[1], GetChannel);
    case 2: return DoubleLine(func, field, PSTR_multi_octave, 0xFF, m_values.octave[0], GetOctave, m_values.octave[1], GetOctave);
    case 3: return DoubleLine(func, field, PSTR_multi_pbcc, 0xFF, m_values.pbcc[0], GetOnOff, m_values.pbcc[1], GetOnOff);
    case 4: return DoubleLine(func, field, PSTR_multi_velocity, 0xFF, m_values.velocity[0], GetVelocity, m_values.velocity[1], GetVelocity);
    case 5: return DefaultLine(func);
    case 6: // Save As ... 
      if (func == DO_LEFT || func == DO_RIGHT)
        SaveAs();
      return TextLine(func, PSTR_save_as);
    case 7: return TextLine(func, PSTR_remove);
    case 8: return TextLine(func, PSTR_move_left);
    case 9: return TextLine(func, PSTR_move_right);
    case 10: // New ...
      if (func == DO_LEFT || func == DO_RIGHT)
        New();
      return TextLine(func, PSTR_new);
    default: return DefaultLine(func);
  }
}


void PageMulti::SaveIfModified()
{
  // Update "cursor"
  m_values.selected_line = GetSelectedLine();
  m_values.selected_field = GetSelectedField();
  m_values.first_line = GetFirstLine();

  // Save if values are different
  MultiValues stored_values;
  EE::GetMulti(m_which, stored_values);
  if (memcmp(&stored_values, &m_values, sizeof(stored_values)) != 0)
    EE::SetMulti(m_which, m_values);
}

void PageMulti::SaveAs()
{
  Pages::SetNextPage(PAGE_NAME_MULTI, m_which);
}

void PageMulti::New()
{
  uint8_t num_multi = EE::GetNumberOfMultis();
  if (num_multi < EE::GetMaxNumberOfMultis()) {
    MultiValues default_values;
    GetMultiDefault(default_values);
    EE::SetMulti(num_multi, default_values);
    Pages::SetNextPage(PAGE_MULTI, num_multi);
  } else
    Debug::BeepLow();
}

void PageMulti::SetMidiConfiguration()
{
  // If channel 0 and 1 are the same, just program one channel!
  uint8_t active_mode = (m_values.channel[0] == m_values.channel[1]) ? SINGLE_MODE : m_values.mode;
  uint8_t num_active_channels = (active_mode == SINGLE_MODE) ? 1 : 2;

  // Set the configuration
  MidiProcessing::Configuration next_config;
  next_config.m_input_channel = EE::Settings().input_channel;
  next_config.m_nbr_output_channels = num_active_channels;  
  for (int num=0; num < num_active_channels; num++) {  
    next_config.m_output_channel[num].m_channel = m_values.channel[num];
    next_config.m_output_channel[num].m_minimum_velocity = m_values.velocity[num] * 13;
    next_config.m_output_channel[num].m_allow_pitch_modulation = m_values.pbcc[num] != 0;
    next_config.m_output_channel[num].m_transpose = OctaveValueToOctaveDelta(m_values.octave[num]) * 12;
    next_config.m_output_channel[num].m_minimum_note = 0; 
    next_config.m_output_channel[num].m_maximum_note = 127;
    next_config.m_output_channel[num].m_minimum_velocity = m_values.velocity[num] * 13;
  }
  if (active_mode == SPLIT_MODE) {
    // In SPLIT_MODE, the top channel will be at the right side of the keyboard [split_note, 127]
    // and the bottom channel will be at the left side of the keyboard [0, split_note - 1]. 
    const uint8_t split_note = max(1, m_values.split_note); // split_node must be at least 1
    next_config.m_output_channel[0].m_minimum_note = split_note;
    next_config.m_output_channel[1].m_maximum_note = split_note - 1;
  }
  MidiProcessing::SetNextConfiguration(next_config);
}

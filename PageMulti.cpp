#include "PageMulti.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Utils.h"

namespace 
{
  PSTRING(PSTR_multi_channel_short, "|");
  PSTRING(PSTR_multi_channel_long,  "channel");
  PSTRING(PSTR_multi_octave,        "octave");
  PSTRING(PSTR_multi_min_velocity,  "min velocity");
  PSTRING(PSTR_multi_max_velocity,  "max velocity");
  PSTRING(PSTR_multi_pitch_bend,    "pitch bend");
  PSTRING(PSTR_multi_mod_wheel,     "mod wheel");
  PSTRING(PSTR_multi_cc,            "control change");

  #define SPLIT_MODE 0
  #define LAYER_MODE 1
  #define LEFT_MODE  2
  #define RIGHT_MODE 3
  PSTRING(PSTR_mode_split, "SPLIT mode");
  PSTRING(PSTR_mode_layer, "LAYER mode");
  PSTRING(PSTR_mode_left,  "LEFT mode");
  PSTRING(PSTR_mode_right, "RIGHT mode");
  PTABLE(PTAB_mode, PSTR_mode_split, PSTR_mode_layer, PSTR_mode_left, PSTR_mode_right);
  PTABLE_GETTER(GetMode, PTAB_mode);

  PSTRING(PSTR_on_off_off, "off");
  PSTRING(PSTR_on_off_on, "on");
  PTABLE(PSTR_on_off, PSTR_on_off_off, PSTR_on_off_on);
  PTABLE_GETTER(GetOnOff, PSTR_on_off);

  PSTRING(PSTR_multi_split, "split at ");
  const char* GetSplit(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = 128;
    if (i_value < o_number_of_values) {
      char note[10]; // Should be more than long enough
      strcpy(note, GetNoteName(i_value));
      strcpy(data_scratch, GetPString(PSTR_multi_split));
      strcat(data_scratch, note);
      return data_scratch;
    } else
      return GetPString(PSTR_unknown_value);
  }

  PSTRING(PSTR_channel_number, "%02d.");
  const char* GetChannelNumber(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = NumberOfChannels;
    sprintf(data_scratch, GetPString(PSTR_channel_number), i_value + 1);
    return data_scratch;
  }

  const char* GetChannelName(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = NumberOfChannels;
    if (i_value < o_number_of_values)
      return EE::GetChannelName(i_value);
    else
      return GetPString(PSTR_unknown_value);
  }

  const char* GetOctave(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = GetNumberOfOctaves(); 
    if (i_value < o_number_of_values)
      return GetOctaveName(i_value);
    else
      return GetPString(PSTR_unknown_value);
  }

  PSTRING(PSTR_percentage_format, "%d%%");
  const char* GetVelocity(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = VelocityMidiToVelocityValue(127) + 1;
    return GetNumber(VelocityValueToVelocityMidi(i_value));
  }

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
  SetNumberOfLines(15, m_values.selected_line, m_values.selected_field, m_values.first_line);
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

Page::LineResult PageMulti::ActualLine(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case 0: return DoubleLine(func, field, PSTR_empty, 0xFF, m_values.mode, GetMode, m_values.split_note, GetSplit);
    case 1: return DoubleLine(func, field, PSTR_multi_channel_long, 0xFF, m_values.channel[0], GetChannelNumber, m_values.channel[1], GetChannelNumber);
    case 2: return DoubleLine(func, field, PSTR_multi_channel_short, 0xFF, m_values.channel[0], GetChannelName, m_values.channel[1], GetChannelName);
    case 3: return DoubleLine(func, field, PSTR_multi_octave, 0xFF, m_values.octave[0], GetOctave, m_values.octave[1], GetOctave);
    case 4: return DoubleLine(func, field, PSTR_multi_pitch_bend, 0xFF, m_values.pitch_bend[0], GetOnOff, m_values.pitch_bend[1], GetOnOff);
    case 5: return DoubleLine(func, field, PSTR_multi_mod_wheel, 0xFF, m_values.mod_wheel[0], GetOnOff, m_values.mod_wheel[1], GetOnOff);
    case 6: return DoubleLine(func, field, PSTR_multi_cc, 0xFF, m_values.control_change[0], GetOnOff, m_values.control_change[1], GetOnOff);
    case 7: return DoubleLine(func, field, PSTR_multi_min_velocity, 0xFF, m_values.min_velocity[0], GetVelocity, m_values.min_velocity[1], GetVelocity);
    case 8: return DoubleLine(func, field, PSTR_multi_max_velocity, 0xFF, m_values.max_velocity[0], GetVelocity, m_values.max_velocity[1], GetVelocity);
    case 9: return DefaultLine(func);
    case 10: // Save As ... 
      if (func == DO_LEFT || func == DO_RIGHT)
        SaveAs();
      return TextLine(func, PSTR_save_as);
    case 11: return TextLine(func, PSTR_remove);
    case 12: return TextLine(func, PSTR_move_left);
    case 13: return TextLine(func, PSTR_move_right);
    case 14: // New ...
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
  const uint8_t active_mode = ((m_values.channel[0] == m_values.channel[1]) && (m_values.mode == LAYER_MODE || m_values.mode == SPLIT_MODE)) ? LEFT_MODE : m_values.mode;
  const uint8_t num_active_channels = (active_mode == LEFT_MODE || active_mode == RIGHT_MODE) ? 1 : 2;
  const uint8_t first_active_channel = (active_mode == RIGHT_MODE) ? 1 : 0;

  // Set the configuration
  MidiProcessing::Configuration next_config;
  next_config.m_input_channel = EE::Settings().input_channel;
  next_config.m_nbr_output_channels = num_active_channels;  
  for (int num = 0; num < num_active_channels; num++) {  
    next_config.m_output_channel[num].m_channel = m_values.channel[num+first_active_channel];
    next_config.m_output_channel[num].m_minimum_note = 0; 
    next_config.m_output_channel[num].m_maximum_note = 127;
    next_config.m_output_channel[num].m_minimum_velocity = VelocityValueToVelocityMidi(m_values.min_velocity[num+first_active_channel]);
    next_config.m_output_channel[num].m_maximum_velocity = VelocityValueToVelocityMidi(m_values.max_velocity[num+first_active_channel]);    
    next_config.m_output_channel[num].m_allow_pitch_bend = m_values.pitch_bend[num+first_active_channel];
    next_config.m_output_channel[num].m_allow_modulation = m_values.mod_wheel[num+first_active_channel];
    next_config.m_output_channel[num].m_allow_control_change = m_values.control_change[num+first_active_channel];
    next_config.m_output_channel[num].m_transpose = OctaveValueToOctaveDelta(m_values.octave[num+first_active_channel]) * 12;
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

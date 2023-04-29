#include "PageMulti.h"

#include "Debug.h"
#include "Data.h"
#include "MidiFilter.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Utils.h"
#include "Roudi.h" // For SetRedrawNext

#include <midi_serialization.h> // for midi_event_t

namespace 
{
  PSTRING(PSTR_multi_title_format,  " %02d. %s ");

  PSTRING(PSTR_multi_channel_left,  "Left");
  PSTRING(PSTR_multi_channel_right, "Right");
  PSTRING(PSTR_multi_octave,        "Octave");
  PSTRING(PSTR_multi_min_velocity,  "Min velocity");
  PSTRING(PSTR_multi_max_velocity,  "Max velocity");
  PSTRING(PSTR_multi_pitch_bend,    "Pitch bend");
  PSTRING(PSTR_multi_mod_wheel,     "Mod wheel");
  PSTRING(PSTR_multi_cc,            "Control change");


  PSTRING(PSTR_mode, "Mode");
  #define SPLIT_MODE 0
  #define LAYER_MODE 1
  #define LEFT_MODE  2
  #define RIGHT_MODE 3
  PSTRING(PSTR_mode_split, "split");
  PSTRING(PSTR_mode_layer, "layer");
  PSTRING(PSTR_mode_left,  "left");
  PSTRING(PSTR_mode_right, "right");
  PTABLE(PTAB_mode, PSTR_mode_split, PSTR_mode_layer, PSTR_mode_left, PSTR_mode_right);
  PTABLE_GETTER(GetMode, PTAB_mode);

  PSTRING(PSTR_on_off_off, "off");
  PSTRING(PSTR_on_off_on, "on");
  PTABLE(PSTR_on_off, PSTR_on_off_off, PSTR_on_off_on);
  PTABLE_GETTER(GetOnOff, PSTR_on_off);

  PSTRING(PSTR_multi_split, "Split at");
  const char* GetSplit(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = 128;
    if (i_value < o_number_of_values)
      return GetNoteName(i_value);
    else
      return GetPString(PSTR_unknown_value);
  }

  const char* GetChannelName(uint8_t i_value, uint8_t& o_number_of_values)
  {
    o_number_of_values = NumberOfChannels;
    if (i_value < o_number_of_values)
      return EE::GetChannelNameFormatted(i_value);
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

  PSTRING(PSTR_save_as, "> Save as ...");
  PSTRING(PSTR_remove, "> Remove ...");
  PSTRING(PSTR_move_left, "> Move left or right");
  PSTRING(PSTR_new, "> New");

  void ListenIn(const midi_event_t& event, void* data)
  {
    PageMulti* page_multi = static_cast<PageMulti*>(data);

    // If we have a note on on the active input channel, we store its velocity in m_velocity_of_last_note
    if (event.m_event == 0x9 && event.m_data[2] !=0 && MidiFilter::IsActiveInputChannel(event))
    {
      page_multi->m_last_note = event.m_data[1];
      SetRedrawNext();
    }
  }

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
  SetNumberOfLines(17, m_values.selected_line, m_values.selected_field, m_values.first_line);
  SetMidiConfiguration();

  // Attach listener
  m_last_note = 0xFF;
  MidiProcessing::SetMidiInListener({ListenIn, this});  
}

void PageMulti::OnStop()
{
  // Detach listener
  MidiProcessing::SetMidiInListener({nullptr, nullptr});

//  SaveIfModified();  
}

void PageMulti::OnTimeout()
{
  SaveIfModified();
}

const char* PageMulti::GetTitle()
{
  sprintf(Screen::buffer, GetPString(PSTR_multi_title_format), m_which + 1, m_values.name);
  return Screen::buffer;
}

void PageMulti::Draw(uint8_t from, uint8_t to)
{
  if (m_last_note != 0xFF) {
    // A note was played on the keyboard.
    // If the "Split at" was selected, we update it.
    if (m_values.mode == SPLIT_MODE && GetSelectedLine() == 1 && m_values.split_note != m_last_note) {
      m_values.split_note = m_last_note;
      //Update config???
      Page::Draw(from, to);
    }
    m_last_note = 0xFF;
  } else
    Page::Draw(from, to);
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
    case 0: return SingleLine(func, PSTR_mode, m_values.mode, GetMode);
    case 1: if (m_values.mode == SPLIT_MODE) 
              return SingleLine(func, PSTR_multi_split, m_values.split_note, GetSplit);
            else
              return DefaultLine(func);
    case 2: return SingleLine(func, PSTR_multi_channel_left, m_values.channel[0].channel, GetChannelName);
    case 3: return SingleLine(func, PSTR_multi_channel_right, m_values.channel[1].channel, GetChannelName);
    case 4: return DoubleLine(func, field, PSTR_multi_octave, 5, m_values.channel[0].octave, GetOctave, m_values.channel[1].octave, GetOctave);
    case 5: return DoubleLine(func, field, PSTR_multi_pitch_bend, 5, m_values.channel[0].pitch_bend, GetOnOff, m_values.channel[1].pitch_bend, GetOnOff);
    case 6: return DoubleLine(func, field, PSTR_multi_mod_wheel, 5, m_values.channel[0].mod_wheel, GetOnOff, m_values.channel[1].mod_wheel, GetOnOff);
    case 7: return DoubleLine(func, field, PSTR_multi_cc, 5, m_values.channel[0].control_change, GetOnOff, m_values.channel[1].control_change, GetOnOff);
    case 8: return DoubleLine(func, field, PSTR_multi_min_velocity, 5, m_values.channel[0].min_velocity, GetVelocity, m_values.channel[1].min_velocity, GetVelocity);
    case 9: return DoubleLine(func, field, PSTR_multi_max_velocity, 5, m_values.channel[0].max_velocity, GetVelocity, m_values.channel[1].max_velocity, GetVelocity);
    case 10: return DefaultLine(func);
    case 11: // New
      if (func == DO_LEFT || func == DO_RIGHT)
        New();
      return TextLine(func, PSTR_new);
    case 12: // Move left or right
      if (func == DO_LEFT || func == DO_RIGHT)
        MoveLeftOrRight(func == DO_LEFT);
      return TextLine(func, PSTR_move_left);
    case 13: // Save as ... 
      if (func == DO_LEFT || func == DO_RIGHT)
        SaveAs();
      return TextLine(func, PSTR_save_as);
    case 14: // Remove ...
      if (func == DO_LEFT || func == DO_RIGHT)
        Remove();
      return TextLine(func, PSTR_remove);
    case 16: // Panic!
      if (func == DO_LEFT || func == DO_RIGHT)
        MidiProcessing::SetPanic(); 
      return TextLine(func, PSTR_panic);
    default: return DefaultLine(func);
  }
}

void PageMulti::SaveIfModified()
{
  // Update "cursor"
  m_values.selected_line = GetSelectedLine();
  m_values.selected_field = GetSelectedField();
  m_values.first_line = GetFirstLine();

  // Save values
  EE::SetMulti(m_which, m_values); // (will only save if values have changed)
}

void PageMulti::SaveAs()
{
  Pages::SetNextPage(PAGE_MULTI_NAME, m_which);
}

void PageMulti::Remove()
{
  if (EE::GetNumberOfMultis() > 1)
    Pages::SetNextPage(PAGE_MULTI_REMOVE, m_which);
  else
    Debug::BeepLow();
}

void PageMulti::MoveLeftOrRight(bool left)
{
  uint8_t new_which = left ? m_which - 1 : m_which + 1; // We make use of 0 - 1 = 255
  if (new_which < EE::GetNumberOfMultis()) {
    // We will now swap m_which with new_which
    // Save values from new_which to current m_which
    MultiValues original_values;
    EE::GetMulti(new_which, original_values);
    EE::SetMulti(m_which, original_values);
    // Replace current m_which with the new one, save and update the page
    m_which = new_which;
    SaveIfModified();
    Pages::SetNextPage(PAGE_MULTI, m_which); 
  } else
    Debug::BeepLow();
}

void PageMulti::New()
{
  uint8_t new_multi = EE::NewMulti();
  if (new_multi != 0xFF)
    Pages::SetNextPage(PAGE_MULTI, new_multi);
}

void PageMulti::SetMidiConfiguration()
{
  // If channel 0 and 1 are the same, just program one channel!
  const uint8_t active_mode = ((m_values.channel[0].channel == m_values.channel[1].channel) && (m_values.mode == LAYER_MODE || m_values.mode == SPLIT_MODE)) ? LEFT_MODE : m_values.mode;
  const uint8_t num_active_channels = (active_mode == LEFT_MODE || active_mode == RIGHT_MODE) ? 1 : 2;
  const uint8_t first_active_channel = (active_mode == RIGHT_MODE) ? 1 : 0;

  // Set the configuration
  MidiProcessing::Configuration next_config;
  next_config.m_input_channel = EE::Settings().input_channel;
  next_config.m_nbr_output_channels = num_active_channels;
  next_config.m_override_default_filter = true;
  for (int num = 0; num < num_active_channels; num++) {
    ChannelValues& channel_values = m_values.channel[num + first_active_channel];
    next_config.m_output_channel[num].m_channel = channel_values.channel;
    next_config.m_output_channel[num].m_minimum_note = 0; 
    next_config.m_output_channel[num].m_maximum_note = 127;
    next_config.m_output_channel[num].m_minimum_velocity = VelocityValueToVelocityMidi(channel_values.min_velocity);
    next_config.m_output_channel[num].m_maximum_velocity = VelocityValueToVelocityMidi(channel_values.max_velocity);    
    next_config.m_output_channel[num].m_allow_pitch_bend = channel_values.pitch_bend;
    next_config.m_output_channel[num].m_allow_modulation = channel_values.mod_wheel;
    next_config.m_output_channel[num].m_allow_control_change = channel_values.control_change;
    next_config.m_output_channel[num].m_transpose = OctaveValueToOctaveDelta(channel_values.octave) * 12;
  }
  if (active_mode == SPLIT_MODE) {
    // In SPLIT_MODE, the top channel will be at the right side of the keyboard [split_note, 127]
    // and the bottom channel will be at the left side of the keyboard [0, split_note - 1]. 
    const uint8_t split_note = max(1, m_values.split_note); // split_node must be at least 1
    next_config.m_output_channel[0].m_maximum_note = split_note - 1;
    next_config.m_output_channel[1].m_minimum_note = split_note;
  }
  MidiProcessing::SetNextConfiguration(next_config);
}

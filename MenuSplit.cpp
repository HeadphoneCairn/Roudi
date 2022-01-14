#include "MenuSplit.h"

#include "Data.h"
#include "Menus.h"
#include "Roudi.h"
#include "Utils.h"

namespace 
{
  bool g_first_run = true;
  uint8_t g_selected_line = 0;
  uint8_t g_first_line = 0; 

  struct SplitValues g_values;
  void gValues_ChannelIndexToChannelValue()
  {
    g_values.left_channel = EE::ChannelIndexToChannelValue(g_values.left_channel);
    g_values.right_channel = EE::ChannelIndexToChannelValue(g_values.right_channel);
  }
  void gValues_ChannelValueToChannelIndex()
  {
    g_values.left_channel = EE::ChannelValueToChannelIndex(g_values.left_channel);
    g_values.right_channel = EE::ChannelValueToChannelIndex(g_values.right_channel);
  }
}


PSTRING(PSTR_menu_split, " SPLIT "); 
PSTRING(PSTR_left,       " Left: ");
PSTRING(PSTR_right,      "Right: ");
PSTRING(PSTR_octave,     "       ");
PSTRING(PSTR_split,      "Split note: ");


void line_left_channel(ParsPars& pars)
  {
  pars.types = TypePString|TypeFunction;
  pars.name = (void*) PSTR_left;
  pars.number_of_values = EE::GetNumberOfChannels(); 
  pars.values = (void*) EE::GetChannelNameFormatted; 
  }

void line_left_octave(ParsPars& pars)
  {
  pars.types = TypePString|TypeFunction; 
  pars.name = (void*) PSTR_octave;
  pars.number_of_values = GetNumberOfOctaves(); 
  pars.values = (void*) GetOctaveName;
  }

void line_right_channel(ParsPars& pars)
  {
  pars.types = TypePString|TypeFunction;
  pars.name = (void*) PSTR_right;
  pars.number_of_values = EE::GetNumberOfChannels();
  pars.values = (void*) EE::GetChannelNameFormatted;
  }

void line_right_octave(ParsPars& pars)
  {
  pars.types = TypePString|TypeFunction; 
  pars.name = (void*) PSTR_octave;
  pars.number_of_values = GetNumberOfOctaves();
  pars.values = (void*) GetOctaveName;
  }

void line_split_note(ParsPars& pars)
  {
  pars.types = TypePString|TypeFunction; 
  pars.name = (void*) PSTR_split; 
  pars.number_of_values = 128;
  pars.values = (void*) GetNoteName;
  }



MenuSplit::MenuSplit(): 
  Menu()
{
  if (g_first_run) {
    g_first_run = false;
    EE::GetSplit(0, g_values);
  }

  gValues_ChannelValueToChannelIndex();
  m_lines[0].Init(line_left_channel,  &g_values.left_channel);
  m_lines[1].Init(line_left_octave,   &g_values.left_octave);   // must be initted to 2!
  m_lines[2].Init(line_right_channel, &g_values.right_channel);
  m_lines[3].Init(line_right_octave,  &g_values.right_octave);  // must be initted to 2!
  m_lines[4].Init(line_split_note,    &g_values.split_note);
  SetNumberOfLines(m_number_of_combilines + 8, g_selected_line, g_first_line);

  SettingsValues settings;
  EE::GetSettings(settings);
  m_split_delta_from_settings = settings.split_delta;

  SetMidiConfiguration();
}

const char* MenuSplit::GetTitle()
{
  return GetPString(PSTR_menu_split);
}

void MenuSplit::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  if (line < m_number_of_combilines) {
    text = m_lines[line].GetText();
    inversion = m_lines[line].GetInversion();
  } else if (line >= m_number_of_combilines + 1 && line <= m_number_of_combilines + 3) { // Load presets
    text = GetPStringLoadPreset(line - m_number_of_combilines);
    inversion = Screen::inversion_all;
  } else if (line >= m_number_of_combilines + 4 && line <= m_number_of_combilines + 6) {  // Save presets
    text = GetPStringSavePreset(line - (m_number_of_combilines + 3));
    inversion = Screen::inversion_all;
  } else if (line == m_number_of_combilines + 7) {
    text = GetPStringMonitor();
    inversion = Screen::inversion_all;
  } else {
    text = GetPStringEmpty();
    inversion = {Screen::InvertGiven, 0, 0};
  }
}

bool MenuSplit::OnLine(LineAction action, uint8_t line)
{
  bool redraw = ActualOnLine(action, line);
  if (redraw) // A setting has changed, so we update the midi configuration
    SetMidiConfiguration();
  return redraw;
}

bool MenuSplit::ActualOnLine(LineAction action, uint8_t line)
{
  if (action == SELECTED)
    return false;

  if (line == 4) { // Split note
    return ChangeSplitNote(*(m_lines[line].GetSelectedValue()), action == LEFT);
  } else if (line < m_number_of_combilines) { // combilines
    return action == LEFT ? m_lines[line].OnLeft() : m_lines[line].OnRight();
  } else if (line >= m_number_of_combilines + 1 && line <= m_number_of_combilines + 3) {  // Load presets
    EE::GetSplit(line - m_number_of_combilines, g_values);
    gValues_ChannelValueToChannelIndex();
    return true;
  } else if (line >= m_number_of_combilines + 4 && line <= m_number_of_combilines + 6) {  // Save presets
    gValues_ChannelIndexToChannelValue();
    EE::SetSplit(line - (m_number_of_combilines + 3), g_values);
    gValues_ChannelValueToChannelIndex();
    return false;
  } else
    return false;
}

void MenuSplit::OnStop(uint8_t selected_line, uint8_t first_line)
{
  gValues_ChannelIndexToChannelValue();
  g_selected_line = selected_line;
  g_first_line = first_line;
}


static bool IsC(int16_t value)
// 0 - c octave: 0, 12, 24, 36, ...
{
  return (value % 12 == 0);
}

static bool IsCEA(int16_t value)
// 1 - c e a:  0, 4, 9,   12, 16, 21,  ... 
{
  int16_t mod = (value + 12) % 12;
  return (mod == 0 || mod == 4 || mod ==9);
}

static bool IsCDEFGAB(int16_t value)
// 2 - c major: 0, 2, 4, 5, 7, 9, 11,   
{
  int16_t mod = (value + 12) % 12;
  return !(mod == 1 || mod == 3 || mod == 6 || mod == 8 || mod == 10); 
}

static bool IsSemitone(int16_t value)
// 3 - semitone
{
  return true;
}

bool MenuSplit::ChangeSplitNote(uint8_t& note_value, bool left)
{ 
  int16_t value = static_cast<int16_t>(note_value);

  // Increment/Decrement
  bool (*allowed_value)(int16_t value);
  switch (m_split_delta_from_settings) {
    case 0: allowed_value = IsC; break; 
    case 1: allowed_value = IsCEA; break; 
    case 2: allowed_value = IsCDEFGAB; break;
    default: allowed_value = IsSemitone; break;
  };
  if (left) {
    do { value--; } while (!allowed_value(value));
  } else {
    do { value++; } while (!allowed_value(value));
  }

  // Trim
  uint8_t new_note_value = static_cast<uint8_t>(constrain(value, 0, 127));
  if (new_note_value != note_value) {
    note_value = new_note_value;
    return true;
  } else
    return false;
} 

void MenuSplit::SetMidiConfiguration()
// Left channel will play on notes [0, split_note - 1], right on notes [split_note, 127]
// In the unlikely case that the split note is 0 = C-1, we have left on [0, 0] and right on [0, 127],
// so, some overlap. But don't care.  
{
  g_next_midi_config.config.SetDefaults();
//g_next_midi_config.config.m_input_channel = 0; // TODO ??????
  g_next_midi_config.config.m_nbr_output_channels = 2;
  g_next_midi_config.config.m_output_channel[0].m_channel = EE::ChannelIndexToChannelValue(g_values.left_channel);
  g_next_midi_config.config.m_output_channel[0].m_transpose = OctaveValueToOctaveDelta(g_values.left_octave) * 12;
  g_next_midi_config.config.m_output_channel[0].m_minimum_note = 0; 
  g_next_midi_config.config.m_output_channel[0].m_maximum_note = g_values.split_note > 0 ? g_values.split_note - 1 : 0;
  g_next_midi_config.config.m_output_channel[1].m_channel = EE::ChannelIndexToChannelValue(g_values.right_channel);
  g_next_midi_config.config.m_output_channel[1].m_transpose = OctaveValueToOctaveDelta(g_values.right_octave) * 12;
  g_next_midi_config.config.m_output_channel[1].m_minimum_note = g_values.split_note; 
  g_next_midi_config.config.m_output_channel[1].m_maximum_note = 127; 
  g_next_midi_config.go = true;
}

#include "PageMulti.h"

#include "Data.h"
#include "Menus.h"
#include "Roudi.h"
#include "Utils.h"
#include "Debug.h"

// TODO: Remove all global variables here. Danger of incorrect initiation order!!!

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

  uint8_t g_pitchbend_1 = 0, g_pitchbend_2 =1;
  uint8_t g_velocity_1 = 3, g_velocity_2 = 8;
}


PSTRING(PSTR_page_multi, " MULTI "); 
PSTRING(PSTR_left,       "Left is good.");
PSTRING(PSTR_right,      "Right is bad. ");
PSTRING(PSTR_split,      "Split note!");



namespace {

  PSTRING(PSTR_channel, "");
  void g_par_channel(NewParsPars& pars)
  {
    pars.types = TypePString|TypeFunction;
    pars.name = (void*) PSTR_channel;
    pars.number_of_values = 16;
    pars.values = (void*) GetChannelNameBrol;
  }

  PSTRING(PSTR_octave, "oct: ");
  void g_par_octave(NewParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_octave;
    pars.number_of_values = GetNumberOfOctaves(); 
    pars.values = (void*) GetOctaveName;
    }

  PSTRING(PSTR_pitchbend, "    pb/cc: ");
  void g_par_pitchbend(NewParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_pitchbend;
    pars.number_of_values = 2; 
    pars.values = (void*) GetOnOff;
    }

  PSTRING(PSTR_percentage_format, "%d%%");
  const char* GetPercentage(uint8_t selected_value)
  {
    sprintf(data_scratch, GetPString(PSTR_percentage_format), selected_value*10);
    return data_scratch;
  }

  PSTRING(PSTR_velocity, "vel: ");
  void g_par_velocity(NewParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_velocity;
    pars.number_of_values = 10;
    pars.values = (void*) GetPercentage;
    }


}


#if 0


void line_split_note(NewParsPars& pars)
  {
  pars.types = TypePString|TypeFunction; 
  pars.name = (void*) PSTR_split; 
  pars.number_of_values = 128;
  pars.values = (void*) GetNoteName;
  }
#endif


PageMulti::PageMulti(): 
  Page() {}

void PageMulti::OnStart()
{
  if (g_first_run) {
    g_first_run = false;
    EE::GetSplit(0, g_values);
  }

  gValues_ChannelValueToChannelIndex();
  g_values.left_octave = OctaveDeltaToOctaveValue(0);

  // Bind ui elements to values
  m_ui_channel_1.Init(g_par_channel, &g_values.left_channel);
  m_ui_octave_1.Init(g_par_octave, &g_values.left_octave);
  m_ui_pitchbend_1.Init(g_par_pitchbend, &g_pitchbend_1);
  m_ui_velocity_1.Init(g_par_velocity, &g_velocity_1);

  m_ui_channel_2.Init(g_par_channel, &g_values.right_channel);
  m_ui_octave_2.Init(g_par_octave, &g_values.right_octave);
  m_ui_pitchbend_2.Init(g_par_pitchbend, &g_pitchbend_2);
  m_ui_velocity_2.Init(g_par_velocity, &g_velocity_2);

  SetNumberOfLines(20, g_selected_line, g_first_line);

  SettingsValues settings;
  EE::GetSettings(settings);
  m_split_delta_from_settings = settings.split_delta;

  SetMidiConfiguration();
}

const char* PageMulti::GetTitle()
{
  return GetPString(PSTR_page_multi);
}

Page::LineResult TextLine(Page::LineFunction func, const char* pstring)
{
  return Page::LineResult{1, GetPString(pstring), Screen::inversion_all, false};
}

Page::LineResult DumbLine(Page::LineFunction func, uint8_t line, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return Page::LineResult{1, nullptr, Screen::inversion_none, false};
  if (func == Page::GET_TEXT)
    return Page::LineResult{1, GetPStringLoadPreset(line), field==0 ? Screen::inversion_all : Screen::inversion_none, false};

  return Page::LineResult{1, nullptr, Screen::inversion_none, false};
}


Page::LineResult PageMulti::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (line == 0)
    return LineChannelOctave1(func, field);
  else if (line == 1)
    return LinePitchbendVelocity1(func, field);
  else if (line == 2)
    return TextLine(func, PSTR_left);
  else if (line == 3)
    return TextLine(func, PSTR_right);
  else if (line == 4)
    return TextLine(func, PSTR_split);
  else
    return DumbLine(func, line, field);
}

Page::LineResult PageMulti::LineChannelOctave1(LineFunction func, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return {2, nullptr, Screen::inversion_none, false};
  if (func == Page::GET_TEXT) {
    char* text = Screen::buffer;
    const uint8_t text_len = Screen::buffer_len;
    Screen::Inversion ch_inversion, oct_inversion;
    m_ui_channel_1.GetText(text, text_len, ch_inversion, 0, 14, 2);
    m_ui_octave_1.GetText(text, text_len, oct_inversion, strlen(text), 7, 0);
    return {2, text, field==0 ? ch_inversion : oct_inversion, false};
  }
  if (func == Page::DO_LEFT)
    return {2, nullptr, Screen::inversion_none, field==0 ? m_ui_channel_1.OnLeft() : m_ui_octave_1.OnLeft()};
  if (func == Page::DO_RIGHT)
    return {2, nullptr, Screen::inversion_none, field==0 ? m_ui_channel_1.OnRight() : m_ui_octave_1.OnRight()};

  return {2, nullptr, Screen::inversion_none, false};
}

Page::LineResult PageMulti::LinePitchbendVelocity1(LineFunction func, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return {2, nullptr, Screen::inversion_none, false};
  if (func == Page::GET_TEXT) {
    char* text = Screen::buffer;
    const uint8_t text_len = Screen::buffer_len;
    Screen::Inversion pb_inversion, vel_inversion;
    m_ui_pitchbend_1.GetText(text, text_len, pb_inversion, 0, 14, 2);
    m_ui_velocity_1.GetText(text, text_len, vel_inversion, strlen(text), 8, 0);
    return {2, text, field==0 ? pb_inversion : vel_inversion, false};
  }
  if (func == Page::DO_LEFT)
    return {2, nullptr, Screen::inversion_none, field==0 ? m_ui_pitchbend_1.OnLeft() : m_ui_velocity_1.OnLeft()};
  if (func == Page::DO_RIGHT)
    return {2, nullptr, Screen::inversion_none, field==0 ? m_ui_pitchbend_1.OnRight() : m_ui_velocity_1.OnRight()};

  return {2, nullptr, Screen::inversion_none, false};
}



#if 0

bool PageMulti::OnLine(LineAction action, uint8_t line)
{
  bool redraw = ActualOnLine(action, line);
  if (redraw) // A setting has changed, so we update the midi configuration
    SetMidiConfiguration();
  return redraw;
}

bool PageMulti::ActualOnLine(LineAction action, uint8_t line)
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

#endif

void PageMulti::OnStop(uint8_t selected_line, uint8_t first_line)
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

bool PageMulti::ChangeSplitNote(uint8_t& note_value, bool left)
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

void PageMulti::SetMidiConfiguration()
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

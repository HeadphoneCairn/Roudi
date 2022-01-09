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
}


PSTRING(PSTR_page_multi, " MULTI "); 
PSTRING(PSTR_left,       "Left is good.");
PSTRING(PSTR_right,      "Right is bad. ");
PSTRING(PSTR_split,      "Split note!");



namespace {

  PSTRING(PSTR_octave,     "octave: ");
  void g_par_octave(NewParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_octave;
    pars.number_of_values = GetNumberOfOctaves(); 
    pars.values = (void*) GetOctaveName;
    }


  PSTRING(PSTR_numformat, "%d");
  const char* GetNumberPlusOne(uint8_t selected_value)
  {
    sprintf(data_scratch, GetPString(PSTR_numformat), selected_value + 1);
    return data_scratch;
  }

  PSTRING(PSTR_channel,     "ch: ");
  void g_par_channel(NewParsPars& pars)
  {
    pars.types = TypePString|TypeFunction;
    pars.name = (void*) PSTR_channel;
    pars.number_of_values = 16;
    pars.values = (void*) GetNumberPlusOne;
  }

}


#if 0


void line_left_channel(NewParsPars& pars)
  {
  pars.types = TypePString|TypeFunction;
  pars.name = (void*) PSTR_left;
  pars.number_of_values = EE::GetNumberOfChannels(); 
  pars.values = (void*) EE::GetChannelNameFormatted; 
  }

void line_left_octave(NewParsPars& pars)
  {
  pars.types = TypePString|TypeFunction; 
  pars.name = (void*) PSTR_octave;
  pars.number_of_values = GetNumberOfOctaves(); 
  pars.values = (void*) GetOctaveName;
  }

void line_right_channel(NewParsPars& pars)
  {
  pars.types = TypePString|TypeFunction;
  pars.name = (void*) PSTR_right;
  pars.number_of_values = EE::GetNumberOfChannels();
  pars.values = (void*) EE::GetChannelNameFormatted;
  }

void line_right_octave(NewParsPars& pars)
  {
  pars.types = TypePString|TypeFunction; 
  pars.name = (void*) PSTR_octave;
  pars.number_of_values = GetNumberOfOctaves();
  pars.values = (void*) GetOctaveName;
  }

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
  g_values.left_octave = 0;
  m_ui_channel_1.Init(g_par_channel, &g_values.left_channel);
  m_ui_channel_2.Init(g_par_channel, &g_values.right_channel);
  m_ui_octave_1.Init(g_par_octave, &g_values.left_octave);
  m_ui_octave_2.Init(g_par_octave, &g_values.right_octave);

#if 0
  m_lines[0].Init(line_left_channel,  &g_values.left_channel);
  m_lines[1].Init(line_left_octave,   &g_values.left_octave);   // must be initted to 2!
  m_lines[2].Init(line_right_channel, &g_values.right_channel);
  m_lines[3].Init(line_right_octave,  &g_values.right_octave);  // must be initted to 2!
  m_lines[4].Init(line_split_note,    &g_values.split_note);
#endif
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

Screen::Inversion none = {Screen::InvertNone, 0, 0};  // TODO put somewhere centrally
Screen::Inversion all = {Screen::InvertAll, 0, 0};


Page::LineResult PageMulti::LineChannel1a(LineFunction func, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return Page::LineResult{1, nullptr, none, false};
  if (func == Page::GET_TEXT) {
    const char* text = m_ui_channel_1.GetText();
    Screen::Inversion inversion = m_ui_channel_1.GetInversion();
    return Page::LineResult{1, text, inversion, false};
  }
  if (func == Page::DO_LEFT)
    return Page::LineResult{1, nullptr, none, m_ui_channel_1.OnLeft()};
  if (func == Page::DO_RIGHT)
    return Page::LineResult{1, nullptr, none, m_ui_channel_1.OnRight()};

  return Page::LineResult{1, nullptr, none, false};
}

Page::LineResult PageMulti::LineChannel1b(LineFunction func, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return LineResult{2, nullptr, none, false};
  if (func == Page::GET_TEXT) {
    char* text = Screen::buffer;
    sprintf(text, "par1: %s, par2: %s", "val1", "val2");
    Screen::Inversion inversion = none;
    if (field == 0)
      inversion = { Screen::InvertGiven, 6, 9};
    else if (field == 1)
      inversion = { Screen::InvertGiven, 18, 21};
    return LineResult{2, text, inversion, false};
  }

  return LineResult{2, nullptr, none, false};
}

/*
Page::LineResult PageMulti::Line1(LineFunction func, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return LineResult{2, nullptr, none, false};
  if (func == Page::GET_TEXT) {
    char* text = Screen::buffer;
    sprintf(text, "par1: %s, par2: %s", "val1", "val2");
    Screen::Inversion inversion = none;
    if (field == 0)
      inversion = { Screen::InvertGiven, 6, 9};
    else if (field == 1)
      inversion = { Screen::InvertGiven, 18, 21};
    return LineResult{2, text, inversion, false};
  }

  return LineResult{2, nullptr, none, false};
}

Page::LineResult PageMulti::Line2(LineFunction func, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return Page::LineResult{1, nullptr, none, false};
  if (func == Page::GET_TEXT) {
    //char* text = Screen::buffer;
    //sprintf(text, "dit is test");


    const char* text = m_ui_octave.GetText();
    Screen::Inversion inversion = m_ui_octave.GetInversion();
    return Page::LineResult{1, text, inversion, false};


//    Screen::Inversion inversion = m_ui_octave.GetInversion();
//    return Page::LineResult{1, text, inversion, false};
    
    //Screen::Inversion inversion = none;
    //if (field == 0)
    //  inversion = { Screen::InvertGiven, 6, 9};
    //else if (field == 1)
    //  inversion = { Screen::InvertGiven, 18, 21};
    //return Page::LineResult{2, text, inversion, false};
  }
  if (func == Page::DO_LEFT)
    return Page::LineResult{1, nullptr, none, m_ui_octave.OnLeft()};
  if (func == Page::DO_RIGHT)
    return Page::LineResult{1, nullptr, none, m_ui_octave.OnRight()};

  return Page::LineResult{1, nullptr, none, false};
}


Page::LineResult PageMulti::Line3(LineFunction func, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return Page::LineResult{1, nullptr, none, false};
  if (func == Page::GET_TEXT) {
    const char* text = m_ui_channel_1.GetText();
    Screen::Inversion inversion = m_ui_channel_1.GetInversion();
    return Page::LineResult{1, text, inversion, false};
  }
  if (func == Page::DO_LEFT)
    return Page::LineResult{1, nullptr, none, m_ui_channel_1.OnLeft()};
  if (func == Page::DO_RIGHT)
    return Page::LineResult{1, nullptr, none, m_ui_channel_1.OnRight()};

  return Page::LineResult{1, nullptr, none, false};
}
*/

Page::LineResult DumbLine(Page::LineFunction func, uint8_t line, uint8_t field)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return Page::LineResult{1, nullptr, none, false};
  if (func == Page::GET_TEXT)
    return Page::LineResult{1, GetPStringLoadPreset(line), field==0 ? all : none, false};

  return Page::LineResult{1, nullptr, none, false};
}


Page::LineResult TextLine(Page::LineFunction func, const char* pstring)
{
  return Page::LineResult{1, GetPString(pstring), all, false};
}


Page::LineResult PageMulti::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (line == 0 || line == 9 || line == 18 || line == 19)
    return LineChannel1a(func, field);
  if (line == 1)
    return LineChannel1b(func, field);

//  else if (line == 1)
//    return Line2(func, field);
//  else if (line == 2)
//    return Line3(func, field);
  else if (line==3)
    return TextLine(func, PSTR_left);
  else if (line==4)
    return TextLine(func, PSTR_right);
  else if (line==5)
    return TextLine(func, PSTR_split);
  else
    return DumbLine(func, line, field);
}

#if 0

void PageMulti::GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion)
{
  if (line < m_number_of_combilines) {
    text = m_lines[line].GetText();
    inversion = m_lines[line].GetInversion();
  } else if (line >= m_number_of_combilines + 1 && line <= m_number_of_combilines + 3) { // Load presets
    text = GetPStringLoadPreset(line - m_number_of_combilines);
    inversion = {Screen::InvertAll, 0, 0};
  } else if (line >= m_number_of_combilines + 4 && line <= m_number_of_combilines + 6) {  // Save presets
    text = GetPStringSavePreset(line - (m_number_of_combilines + 3));
    inversion = {Screen::InvertAll, 0, 0};
  } else if (line == m_number_of_combilines + 7) {
    text = GetPStringMonitor();
    inversion = {Screen::InvertAll, 0, 0};
  } else {
    text = GetPStringEmpty();
    inversion = {Screen::InvertGiven, 0, 0};
  }
}

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

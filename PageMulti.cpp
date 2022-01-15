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
  uint8_t g_mode = 0, g_split_note = 100;



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

  PSTRING(PSTR_mode_split, "SPLIT mode");
  PSTRING(PSTR_mode_layer, "LAYER mode");
  const char* GetMode(uint8_t mode)
  {
    return GetPString(mode ? PSTR_mode_layer : PSTR_mode_split);
  }

  PSTRING(PSTR_mode, "");
  void g_par_mode(NewParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_mode;
    pars.number_of_values = 2; 
    pars.values = (void*) GetMode;
    }
  // todo verander in ptab

  PSTRING(PSTR_split_note, "split: ");
  void g_par_split_note(NewParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_split_note;
    pars.number_of_values = 128; 
    pars.values = (void*) GetNoteName;
    }

  PSTRING(PSTR_save, "> Save");
  PSTRING(PSTR_save_as, "> Save As");
  PSTRING(PSTR_rename, "> Rename");
  PSTRING(PSTR_move_left, "> Move left");
  PSTRING(PSTR_move_right, "> Move right");

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

  m_ui_mode.Init(g_par_mode, &g_mode);
  m_ui_split_note.Init(g_par_split_note, &g_split_note);

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


Page::LineResult PageMulti::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (line == 0)
    return DoubleCombiline(func, field, m_ui_channel_1, 14, 2, false, m_ui_octave_1,  7, 0, false);
  else if (line == 1)
    return DoubleCombiline(func, field, m_ui_pitchbend_1, 14, 2, false, m_ui_velocity_1,  8, 0, false);
  else if (line == 2)
    return DoubleCombiline(func, field, m_ui_channel_2, 14, 2, false, m_ui_octave_2,  7, 0, false);
  else if (line == 3)
    return DoubleCombiline(func, field, m_ui_pitchbend_2, 14, 2, false, m_ui_velocity_2,  8, 0, false);
  else if (line == 4)
    return DoubleCombiline(func, field, m_ui_mode, 10, 4, false, m_ui_split_note, 10, 0, false);
  else if (line == 5)
    return DefaultLine(func);
  else if (line == 6)
    return TextLine(func, PSTR_save);
  else if (line == 7)
    return TextLine(func, PSTR_save_as);
  else if (line == 8)
    return TextLine(func, PSTR_rename);
  else if (line == 9)
    return TextLine(func, PSTR_move_left);
  else if (line == 10)
    return TextLine(func, PSTR_move_right);
  else
    return DefaultLine(func);
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

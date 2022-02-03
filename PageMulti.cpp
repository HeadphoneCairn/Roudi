#include "PageMulti.h"

#include "Debug.h"
#include "Data.h"
#include "Pages.h"
#include "Roudi.h"
#include "Utils.h"

namespace {

  PSTRING(PSTR_channel, "");
  void g_par_channel(ParsPars& pars)
  {
    pars.types = TypePString|TypeFunction;
    pars.name = (void*) PSTR_channel;
    pars.number_of_values = NumberOfChannels;
    pars.values = (void*) EE::GetChannelNameFormatted;
  }

  PSTRING(PSTR_octave, "oct: ");
  void g_par_octave(ParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_octave;
    pars.number_of_values = GetNumberOfOctaves(); 
    pars.values = (void*) GetOctaveName;
    }

  PSTRING(PSTR_pitchbend, "    pb/cc: ");
  void g_par_pitchbend(ParsPars& pars)
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
  void g_par_velocity(ParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_velocity;
    pars.number_of_values = 10;
    pars.values = (void*) GetPercentage;
    }

  PSTRING(PSTR_mode_split, "SPLIT mode");
  PSTRING(PSTR_mode_layer, "LAYER mode");
  PSTRING(PSTR_single_layer, "SINGLE mode");
  PTABLE(PTAB_mode, PSTR_mode_split, PSTR_mode_layer, PSTR_single_layer);
  PSTRING(PSTR_mode, "");
  void g_par_mode(ParsPars& pars)
    {
    pars.types = TypePString|TypePTable;
    pars.name = (void*) PSTR_mode;
    pars.number_of_values = PTAB_mode_size;
    pars.values = (void*) PTAB_mode;
    }

  PSTRING(PSTR_split_note, "split: ");
  void g_par_split_note(ParsPars& pars)
    {
    pars.types = TypePString|TypeFunction; 
    pars.name = (void*) PSTR_split_note;
    pars.number_of_values = 128; 
    pars.values = (void*) GetNoteName;
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

  // Bind ui elements to values
  m_ui_channel_1.Init(g_par_channel, &m_values.channel[0]);
  m_ui_octave_1.Init(g_par_octave, &m_values.octave[0]);
  m_ui_pitchbend_1.Init(g_par_pitchbend, &m_values.pbcc[0]);
  m_ui_velocity_1.Init(g_par_velocity, &m_values.velocity[0]);

  m_ui_channel_2.Init(g_par_channel, &m_values.channel[1]);
  m_ui_octave_2.Init(g_par_octave, &m_values.octave[1]);
  m_ui_pitchbend_2.Init(g_par_pitchbend, &m_values.pbcc[1]);
  m_ui_velocity_2.Init(g_par_velocity, &m_values.velocity[1]);

  m_ui_mode.Init(g_par_mode, &m_values.mode);
  m_ui_split_note.Init(g_par_split_note, &m_values.split_note);

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
  switch (line)
  {
    case 0: return DoubleCombiline(func, field, m_ui_channel_1, 16, 1, false, m_ui_octave_1, 7, 0, false);
    case 1: return DoubleCombiline(func, field, m_ui_pitchbend_1, 14, 2, false, m_ui_velocity_1, 8, 0, false);
    case 2: return DoubleCombiline(func, field, m_ui_channel_2, 16, 1, false, m_ui_octave_2, 7, 0, false);
    case 3: return DoubleCombiline(func, field, m_ui_pitchbend_2, 14, 2, false, m_ui_velocity_2, 8, 0, false);
    case 4: return DoubleCombiline(func, field, m_ui_mode, 12, 2, false, m_ui_split_note, 10, 0, false);
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
    EE::GetSplit(line - m_number_of_combilines, m_values);
    gValues_ChannelValueToChannelIndex();
    return true;
  } else if (line >= m_number_of_combilines + 4 && line <= m_number_of_combilines + 6) {  // Save presets
    gValues_ChannelIndexToChannelValue();
    EE::SetSplit(line - (m_number_of_combilines + 3), m_values);
    gValues_ChannelValueToChannelIndex();
    return false;
  } else
    return false;
}

#endif



void PageMulti::SetMidiConfiguration()
// In SPLIT mode, left channel will play on notes [0, split_note - 1], right on notes [split_note, 127]
{
  g_next_midi_config.config.SetDefaults();
  g_next_midi_config.config.m_input_channel = EE::GetSettings().input_channel;
  g_next_midi_config.config.m_nbr_output_channels = 2;  

  // TODO: If channel 0 and 1 are the same, just program one channel!

  for (int num=0; num < 2; num++) {  
    g_next_midi_config.config.m_output_channel[num].m_channel = m_values.channel[num];
    g_next_midi_config.config.m_output_channel[num].m_minimum_velocity = m_values.velocity[num] * 13;
    g_next_midi_config.config.m_output_channel[num].m_allow_pitch_modulation = m_values.pbcc[num] != 0;
    g_next_midi_config.config.m_output_channel[num].m_transpose = OctaveValueToOctaveDelta(m_values.octave[num]) * 12;
    g_next_midi_config.config.m_output_channel[num].m_minimum_note = 0; 
    g_next_midi_config.config.m_output_channel[num].m_maximum_note = 127; 
  }
  if (m_values.mode == 0) { // Split
    const uint8_t split_note = max(1, m_values.split_note); // split_node must be at least 1
    g_next_midi_config.config.m_output_channel[0].m_maximum_note = split_note - 1;
    g_next_midi_config.config.m_output_channel[1].m_minimum_note = split_note;
  }

  g_next_midi_config.go = true;
}

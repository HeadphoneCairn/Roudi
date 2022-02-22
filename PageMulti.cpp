#include "PageMulti.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"
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

  #define SPLIT_MODE 0
  #define LAYER_MODE 1
  #define SINGLE_MODE 2
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

  SetNumberOfLines(12, m_values.selected_line, m_values.selected_field, m_values.first_line);

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


// Returns the string representation of i_value, + the number of values possible in o_max_value.
typedef const char* (*ValueFunction)(uint8_t i_value, uint8_t& o_number_of_values);

PSTRING(PSTR_test_on, "off");
PSTRING(PSTR_test_off, "on");
PSTRING(PSTR_test_maybe, "maybe");
const char* TestValueFunction(uint8_t i_value, uint8_t& o_number_of_values)
{
  o_number_of_values = 3;
  switch (i_value) {
    case 0: return GetPString(PSTR_test_on);
    case 1: return GetPString(PSTR_test_off);
    case 2: return GetPString(PSTR_test_maybe);
    default: return GetPStringEmpty();
  } 
}

const char* ChannelValueFunction(uint8_t i_value, uint8_t& o_number_of_values)
{
  o_number_of_values = 5;
  switch (i_value) {
    case 0: return "kanaal 0";
    case 1: return "kanaal 1";
    case 2: return "kanaal 2";
    case 3: return "kanaal 3";
    case 4: return "kanaal 4";
    default: return GetPStringEmpty();
  }

}


static Page::LineResult DoubleLine(
  Page::LineFunction func,
  uint8_t field, 
  const char* name,
  uint8_t name_pos, 
  uint8_t& left_value, 
  ValueFunction left_function,
  uint8_t& right_value, 
  ValueFunction right_function
)
{
  Screen::Inversion inversion = Screen::inversion_none;
  bool redraw = false;
  char* text = Screen::buffer;
  text[0] = 0;

  // Set up arrays for easy access to parameters
  uint8_t* pvalues[] = {&left_value, &right_value};
  ValueFunction pfunctions[] = {left_function, right_function};

  if (func == Page::GET_TEXT) {
    // Prepare buffer
    PadRight(text, Screen::buffer_len);   

    // Add name
    if (name_pos == 0xFF)
      name_pos = (Screen::buffer_len - strlen(name)) >> 1;
    strncpy(text + name_pos, GetPString(name), strlen(name));
    
    // Add values
    for (uint8_t i=0; i<2; i++) {
      uint8_t dummy;
      const char* value = pfunctions[i](*pvalues[i], dummy);
      if (i==0) {
        strncpy(text, value, strlen(value));
        if (field == 0)
          inversion = { Screen::InvertGiven, 0, static_cast<uint8_t>(strlen(value) -  1)};  // TODO could be negative
      } else { 
        strncpy(text + Screen::buffer_len - strlen(value), value, strlen(value));
        if (field == 1)
          inversion = { Screen::InvertGiven, static_cast<uint8_t>(Screen::buffer_len - strlen(value)), Screen::buffer_len - 1}; // TODO could be 26
      }
    }

  } else if (func == Page::DO_LEFT) {
    if (*pvalues[field] > 0) {
      *pvalues[field] -= 1;
      redraw = true;
    }
  } else if (func == Page::DO_RIGHT) {
    uint8_t number_of_values = 0;
    pfunctions[field](*pvalues[field], number_of_values); // Get number of values
    if (*pvalues[field] + 1 < number_of_values) {
      *pvalues[field] += 1;
      redraw = true;
    }
  }

  return Page::LineResult{2, text, inversion, redraw};
}

static uint8_t ff = 0;
static uint8_t ss = 1;
PSTRING(PSTR_ss, "Just string");

static Page::LineResult DoDoubleLine(Page::LineFunction func, uint8_t field)
{
  return DoubleLine(
    func,
    field, 
    PSTR_ss,
    5, //0xFF, 
    ff, TestValueFunction,
    ss, ChannelValueFunction
  );

}


Page::LineResult PageMulti::ActualLine(LineFunction func, uint8_t line, uint8_t field)
{
  switch (line)
  {
    case 0: return DoDoubleLine(func, field);
    case 1: return DoubleCombiline(func, field, m_ui_channel_1, 16, 1, false, m_ui_octave_1, 7, 0, false);
    case 2: return DoubleCombiline(func, field, m_ui_pitchbend_1, 14, 3, false, m_ui_velocity_1, 8, 0, false);
    case 3: return DoubleCombiline(func, field, m_ui_channel_2, 16, 1, false, m_ui_octave_2, 7, 0, false);
    case 4: return DoubleCombiline(func, field, m_ui_pitchbend_2, 14, 3, false, m_ui_velocity_2, 8, 0, false);
    case 5: return DoubleCombiline(func, field, m_ui_mode, 12, 3, false, m_ui_split_note, 10, 0, false);
    case 6: return DefaultLine(func);
    case 7: // Save As ... 
      if (func == DO_LEFT || func == DO_RIGHT)
        SaveAs();
      return TextLine(func, PSTR_save_as);
    case 8: return TextLine(func, PSTR_remove);
    case 9: return TextLine(func, PSTR_move_left);
    case 10: return TextLine(func, PSTR_move_right);
    case 11: // New ...
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

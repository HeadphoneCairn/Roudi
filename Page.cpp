#include "Page.h"

#include "Data.h"
#include "Debug.h"
#include "Utils.h"

#include <string.h>
#include <stdlib.h>


//==============================================================================
//
//                                 P A G E
//
//==============================================================================

Page::Page():
  m_number_of_lines(0),
  m_first_line(0),
  m_selected_line(0),
  m_selected_field(0)
{
}

void Page::SetNumberOfLines(uint8_t i_number_of_lines, uint8_t i_selected_line, uint8_t i_selected_field, uint8_t i_first_line)
{
  // Use remembered positions
  if (i_selected_line == 0xFF)
    i_selected_line = m_selected_line;
  if (i_selected_field == 0xFF)
    i_selected_field = m_selected_field;
  if (i_first_line == 0xFF)
    i_first_line = m_first_line;

  // Check (and correct if needed) the selected line and first line compared to new reality
  m_number_of_lines = i_number_of_lines;
  if (i_selected_line < m_number_of_lines) { 
    // We have a valid i_selected_line.
    m_selected_line = i_selected_line;
    // Now, verify the i_first_line
    const uint8_t max_first_line = (i_number_of_lines < 7) ? 0 : i_number_of_lines - 7;
    if (i_first_line <= max_first_line && (i_selected_line >= i_first_line && i_selected_line < i_first_line + 7)) {
      m_first_line = i_first_line;  
    } else {
      // The first line is not correct (anymore). Correct it.
      // We put the selected line in the middle of the screen
      m_first_line = (i_selected_line > 7/2) ? i_selected_line - 7/2 : 0;
      if (m_first_line > max_first_line)
        m_first_line = max_first_line;
    }
  } else {
    m_selected_line = m_first_line = 0;
  }

  // Check (and correct if needed) the selected field
  if (i_selected_field < Line(GET_NUMBER_OF_FIELDS, m_selected_line, 0xFF).number_of_fields)
    m_selected_field = i_selected_field;
  else 
    m_selected_field = 0;
}

uint8_t Page::GetSelectedLine()
{
  return m_selected_line;
}

uint8_t Page::GetSelectedField()
{
  return m_selected_field;
}

uint8_t Page::GetFirstLine()
{
  return m_first_line;
}


void Page::Start(uint8_t parameter)
{
  OnStart(parameter);
  Screen::Clear();
  Draw(0, 7);
  // Line(DO_SELECTED, m_selected_line, m_selected_field); // Is this a good idea?
}

void Page::Stop()
{
  OnStop();  // Used to pass m_selected_line, m_first_line, but they are now available in separate functions.
}

void Page::Timeout()
{
  OnTimeout();
}

void Page::Redraw()
{
  Draw(1, 7);
}

void Page::Up() 
{
  if (OnUpDown(UP)) {
    Draw(1, 7);
    Line(DO_SELECTED, m_selected_line, m_selected_field);       
  }
}

void Page::Down() 
{
  if (OnUpDown(DOWN)) {
    Draw(1, 7);
    Line(DO_SELECTED, m_selected_line, m_selected_field);       
  }
}

bool Page::OnUpDown(UpDownAction action)
{
  if (action == UP) {
    if (m_selected_line == 0 && m_selected_field == 0)
      return false; // do not redraw
    if (m_selected_field > 0) {
      m_selected_field--;
    } else {
      m_selected_line--;
      m_selected_field = Line(GET_NUMBER_OF_FIELDS, m_selected_line, 0xFF).number_of_fields - 1;       
      if (m_selected_line < m_first_line)
        m_first_line = m_selected_line;
    }
    return true; // redraw
  } else /* action == DOWN */ {
    uint8_t nbr_of_fields = Line(GET_NUMBER_OF_FIELDS, m_selected_line, 0xFF).number_of_fields;
    if (m_selected_line == m_number_of_lines - 1 && m_selected_field == nbr_of_fields - 1)
      return false; // do not redraw
    if (m_selected_field + 1 < nbr_of_fields) {
      m_selected_field++;
    } else {
      m_selected_line++;
      m_selected_field = 0;
      if (m_selected_line >= m_first_line + 7)
        m_first_line = m_selected_line - (7 - 1);
    }
    return true; // redraw
  }
}

void Page::Left()
{
  if (Line(DO_LEFT, m_selected_line, m_selected_field).redraw)
    Draw(1, 7); // TODO: should draw only one selected line!!!, well not really for undo of page a complete redraw is necessary
}

void Page::Right()
{
  if (Line(DO_RIGHT, m_selected_line, m_selected_field).redraw)
    Draw(1, 7); // TODO: should draw only one selected line!!!, well not really for undo of page a complete redraw is necessary
}

void Page::Draw(uint8_t from, uint8_t to) // from..to are the lines to draw 
{
  //Debug::Beep();
  if (from == 0)
    Screen::Print(Screen::CanvasComplete, 0, 0xFF, GetTitle(), Screen::LineEmphasis, Screen::inversion_none);
  for (uint8_t i = 1; i <= 7; i++) {      
    if (i >= from && i <= to) {
      uint8_t line_to_show = m_first_line + i - 1;
      if (line_to_show < m_number_of_lines) {
        const char* text; 
        LineResult res = Line(GET_TEXT, line_to_show, line_to_show == m_selected_line ? m_selected_field : 0xFF);       
        Screen::Print(Screen::CanvasScrollbar, i, 0, res.text, Screen::LineClear, line_to_show == m_selected_line ? res.text_inversion : Screen::inversion_none);
      }
    }
  }
  if (true) { // draw scroll_bar
    if (m_number_of_lines > 7) {
      const int16_t bar_height = 8 * 7; // number of pixel in scroll bar 
      const int16_t half_number_of_lines = static_cast<int16_t>(m_number_of_lines>>1);
      const int16_t bar_pos = (static_cast<int16_t>(m_first_line) * bar_height + half_number_of_lines) / static_cast<int16_t>(m_number_of_lines) ;
      const int16_t bar_selected = max((7 * bar_height + half_number_of_lines) / static_cast<int16_t>(m_number_of_lines), 1);
      Screen::DrawScrollbar(static_cast<uint8_t>(bar_pos) + 8, static_cast<uint8_t>(bar_pos + bar_selected - 1) + 8);
    }
  }
}



//==============================================================================
//
//                     L I N E   H E L P   F U N C T I O N S
//
//==============================================================================

Page::LineResult DefaultLine(Page::LineFunction func)
{
  return Page::LineResult{1, GetPStringEmpty(), Screen::inversion_none, false};
}

Page::LineResult TextLine(Page::LineFunction func, const char* pstring)
{
  return Page::LineResult{1, GetPString(pstring), Screen::inversion_all, false};
}


#if 0
Page::LineResult SingleCombiLine(
  Page::LineFunction func, 
  Combiline& combiline, uint8_t len, uint8_t extra_padding, bool right_align
)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return {1, nullptr, Screen::inversion_none, false};
  if (func == Page::GET_TEXT) {
    char* text = Screen::buffer;
    const uint8_t text_len = Screen::buffer_len;
    Screen::Inversion inversion;
    combiline.GetText(text, text_len, inversion, 0, len, extra_padding, right_align);
    return {1, text, inversion, false};
  }
  if (func == Page::DO_LEFT)
    return {1, nullptr, Screen::inversion_none, combiline.OnLeft()};
  if (func == Page::DO_RIGHT)
    return {1, nullptr, Screen::inversion_none, combiline.OnRight()};

  return {1, nullptr, Screen::inversion_none, false};
}

Page::LineResult DoubleCombiline(
  Page::LineFunction func, uint8_t field, 
  Combiline& combiline_1, uint8_t len_1, uint8_t extra_padding_1, bool right_align_1, 
  Combiline& combiline_2, uint8_t len_2, uint8_t extra_padding_2, bool right_align_2 
)
{
  if (func == Page::GET_NUMBER_OF_FIELDS)
    return {2, nullptr, Screen::inversion_none, false};
  if (func == Page::GET_TEXT) {
    char* text = Screen::buffer;
    const uint8_t text_len = Screen::buffer_len;
    Screen::Inversion inversion_1, inversion_2;
    combiline_1.GetText(text, text_len, inversion_1, 0, len_1, extra_padding_1, right_align_1);
    combiline_2.GetText(text, text_len, inversion_2, strlen(text), len_2, extra_padding_2, right_align_2);
    return {2, text, field==0 ? inversion_1 : inversion_2, false};
  }
  if (func == Page::DO_LEFT)
    return {2, nullptr, Screen::inversion_none, field==0 ? combiline_1.OnLeft() : combiline_2.OnLeft()};
  if (func == Page::DO_RIGHT)
    return {2, nullptr, Screen::inversion_none, field==0 ? combiline_1.OnRight() : combiline_2.OnRight()};

  return {2, nullptr, Screen::inversion_none, false};  
}
Page::LineResult BoolLine(Page::LineFunction func, const char* name, uint8_t& value, const char* false_value, const char* true_value, bool invert_all)
{
  char* text = Screen::buffer;
  text[0] = 0;
  bool redraw = false;
  Screen::Inversion inversion = Screen::inversion_all;
  if (func == Page::GET_TEXT) {
    strncat(text, GetPString(name), Screen::buffer_len); // always terminates with a zero!, unlike strncpy
    const uint8_t name_len = strlen(text);
    const char* tf_value = GetPString(value ? true_value : false_value);
    const uint8_t tf_value_len = strlen(tf_value);
    if (name_len + tf_value_len <= Screen::buffer_len) { // safety precaution
      PadRight(text, Screen::buffer_len - (name_len + tf_value_len));
      strcat(text, tf_value);
      if (!invert_all)
        inversion = { Screen::InvertGiven, static_cast<uint8_t>(Screen::buffer_len - tf_value_len), Screen::buffer_len };
    }
  } else if (func == Page::DO_LEFT || func == Page::DO_RIGHT) {
    value = !value;
    redraw = true;
  }
  return { 1, text, inversion, redraw };
}
#endif


const char* GetPTable(uint8_t i_value, uint8_t& o_number_of_values, const char *const * ptable, uint8_t ptable_size)
{
  o_number_of_values = ptable_size;
  if (i_value < o_number_of_values)
    return GetPStringFromPTable(ptable, i_value);
  else
    return GetPStringUnknownValue();
}


Page::LineResult DoubleLine(
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
    const char* name_val = GetPString(name);
    uint8_t name_len = strlen(name_val);
    if (name_pos == 0xFF) // center name
      name_pos = (Screen::buffer_len - name_len) >> 1;
    if (name_pos >= Screen::buffer_len) // make sure pos is not outside screen
      name_pos = 0;
    name_len = min(name_len, Screen::buffer_len - name_pos); 
    strncpy(text + name_pos, name_val, name_len);

    // Add values: first value is left aligned, second is right aligned
    for (uint8_t i = 0; i < 2; i++) {
      uint8_t dummy;
      const char* value = pfunctions[i](*pvalues[i], dummy);
      uint8_t value_len = strlen(value);
      value_len = min(value_len, Screen::buffer_len);
      if (i==0) {
        strncpy(text, value, value_len);
        if (field == 0)
          inversion = { Screen::InvertGiven, 0, static_cast<uint8_t>(value_len -  1)};  // If the value is "" the whole line will be selected
      } else { 
        strncpy(text + Screen::buffer_len - value_len, value, value_len);
        if (field == 1)
          inversion = { Screen::InvertGiven, static_cast<uint8_t>(Screen::buffer_len - value_len), Screen::buffer_len - 1};
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


Page::LineResult SingleLine(
  Page::LineFunction func,
  const char* name,
  uint8_t& value, 
  ValueFunction functie
)
{
  Screen::Inversion inversion = Screen::inversion_all;
  bool redraw = false;
  char* text = Screen::buffer;
  text[0] = 0;

  if (func == Page::GET_TEXT) {
    uint8_t dummy;
    strcpy(text, GetPString(name));
    strcat(text, " ");
    strcat(text, functie(value, dummy));
  } else if (func == Page::DO_LEFT) {
    if (value > 0) {
      value -= 1;
      redraw = true;
    }
    // TODO creat function for this!!! this can be combined with DoubleLIne
  } else if (func == Page::DO_RIGHT) {
    uint8_t number_of_values = 0;
    functie(0, number_of_values); // Get number of values
    if (value + 1 < number_of_values) {
      value += 1;
      redraw = true;
    }
  }

  return Page::LineResult{1, text, inversion, redraw};
}

#include "Page.h"

#include "Data.h"
#include "Debug.h"
#include "Utils.h"

#include <string.h>
#include <stdlib.h>


//==============================================================================
//
//                                 M E N U 
//
//==============================================================================

Page::Page():
  m_number_of_lines(0),
  m_first_line(0),
  m_selected_line(0),
  m_selected_field(0)
{
}

void Page::SetNumberOfLines(uint8_t i_number_of_lines, uint8_t i_selected_line, uint8_t i_first_line)
{
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
}

//uint8_t Page::GetSelectedLine()
//{
//  return m_selected_line;
//}
//
//uint8_t Page::GetFirstLine()
//{
//  return m_first_line;
//}

void Page::Start()
{
  OnStart();
  Screen::Clear();
  Draw(0, 7);
}

void Page::Stop()
{
  OnStop(m_selected_line, m_first_line);
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
      m_selected_field = Line(GET_NUMBER_OF_FIELDS, m_selected_line, -1).number_of_fields - 1;       
      if (m_selected_line < m_first_line)
        m_first_line = m_selected_line;
    }
    return true; // redraw
  } else /* action == DOWN */ {
    uint8_t nbr_of_fields = Line(GET_NUMBER_OF_FIELDS, m_selected_line, -1).number_of_fields;
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
        LineResult res = Line(GET_TEXT, line_to_show, line_to_show == m_selected_line ? m_selected_field : -1);       
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

// To prevent heap fragmentation when allocating the pages, I have chosen
// for a single memory block that is used for all pages.
// You have to make sure that the buffer is large enough to store all types
// of pages.
static char page_buffer[gPageBufferSize];
Page* g_page = (Page*)page_buffer;


//==============================================================================
//
//                             C O M B I L I N E 
//
//==============================================================================

NewCombiline::NewCombiline():
  m_par_function(nullptr),
  m_selected_value(nullptr)
{   
  // TODO vul iets onnozel in om crash te voorkomen
}

void NewCombiline::Init(NewCombilineParameters par_function, uint8_t* selected_value)
{
  m_par_function = par_function;
  m_selected_value = selected_value;
}


PSTRING(PSTR_combiline_error, "ERROR"); 

void NewCombiline::GetText(char* text, uint8_t text_len, Screen::Inversion& inversion, uint8_t start, uint8_t len, uint8_t extra_padding, bool right_align) 
/*
- text: this function will write to this string
- text_len: this should be the length of the text* buffer (not including \0), it is a safety feature 
- inversion: this will be filled by this function
- start: we will start writing at text[start] .. 
- len: ... for length len, if there are not enough chars to be written, spaces are written
- extra_padding: number of extra space to be added at the end
- right_align: if enabled, value is aligned to the right

NOTE: Wanted to use sprintf with "%*.*s", but the variable width specifiers don't appear 
      to be supported by the standard library. You can however use constant width 
      specificiers such as "%5.8s".
*/
{
  inversion = Screen::inversion_all;

  // Do some checks
  if (!m_par_function || start + len + extra_padding > text_len ) { 
      strncpy(text, GetPString(PSTR_combiline_error), text_len);
      return;
    }

  text += start; // we start writing to text at the start position

  // ----
  NewParsPars m;
  m_par_function(m);
  // ----

  // Key:
  const char* key = "";  // TODO this "" does not take extra memory, so could remove PSTRING_EMPTY
  if (m.types & TypeCString) {
    key = static_cast<const char*>(m.name);
  } else if (m.types & TypePString) {
    key = GetPString(static_cast<const char*>(m.name));
  }
  strncpy(text, key, len);
  text[len] = 0;

  // Value
  if (m.number_of_values != 0) {
    const char* value = "";
    if (m.types & TypePTable) {
      const char *const * ptable = static_cast<const char *const *>(m.values);
      value = GetPStringFromPTable(ptable, *m_selected_value);
    } else if (m.types & TypeFunction) {
      NewCombiLineFunction function = (NewCombiLineFunction)m.values;
      value = function(*m_selected_value);
    }
    uint8_t inv_start = 0, inv_stop = 0;
    if (right_align) {
      PadRight(text, len - strlen(text)); // Pad end with spaces
      const uint8_t pos = len - strlen(value); // We expect the length of value to be < 24
      strcpy(text + pos, value);
      inv_start = pos;
      inv_stop = len - 1;
    } else {
      inv_start = strlen(text);
      const uint8_t freechar = len - strlen(text); // can never be negative
      strncat(text, value, freechar); // no need to terminate with zero, because already done above
      inv_stop = strlen(text) - 1;
      PadRight(text, len - strlen(text)); // Pad end with spaces
    }
    inversion = {Screen::InvertGiven, inv_start + start, inv_stop + start};
  } 

  // Add extra_padding 
  PadRight(text, extra_padding);

  return;
}


uint8_t* NewCombiline::GetSelectedValue()
{
  return m_selected_value;
}

bool NewCombiline::OnLeft() 
{
  if (*m_selected_value > 0) {
    (*m_selected_value)--;
    return true;
  } else {
    return false; 
  }
}

bool NewCombiline::OnRight() 
{
  // ----
  NewParsPars m;
  m_par_function(m);
  // ---

  if (*m_selected_value < m.number_of_values - 1) {
    (*m_selected_value)++;
    return true;
  } else {
    return false;
  }
}

void NewCombiline::UnitTest()
{
  // TODO: check all possibilities
}
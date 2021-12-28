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
    Line(DO_SELECTED, m_selected_line, 0);       
  }
}

void Page::Down() 
{
  if (OnUpDown(DOWN)) {
    Draw(1, 7);
    Line(DO_SELECTED, m_selected_line, 0);       
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
  if (Line(DO_LEFT, m_selected_line, 0).redraw)
    Draw(1, 7); // TODO: should draw only one selected line!!!, well not really for undo of page a complete redraw is necessary
}

void Page::Right()
{
  if (Line(DO_RIGHT, m_selected_line, 0).redraw)
    Draw(1, 7); // TODO: should draw only one selected line!!!, well not really for undo of page a complete redraw is necessary
}

void Page::Draw(uint8_t from, uint8_t to) // from..to are the lines to draw 
{
  const Screen::Inversion no_inversion = { Screen::InvertNone, 0, 0 };
  if (from == 0)
    Screen::Print(Screen::CanvasComplete, 0, 0xFF, GetTitle(), Screen::LineEmphasis, no_inversion);
  for (uint8_t i = 1; i <= 7; i++) {      
    if (i >= from && i <= to) {
      uint8_t line_to_show = m_first_line + i - 1;
      if (line_to_show < m_number_of_lines) {
        const char* text; 
        LineResult res = Line(GET_TEXT, line_to_show, line_to_show == m_selected_line ? m_selected_field : -1);       
        Screen::Print(Screen::CanvasScrollbar, i, 0, res.text, Screen::LineClear, line_to_show == m_selected_line ? res.text_inversion : no_inversion);
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

const char* NewCombiline::GetText() 
{
  // ----
  NewParsPars m;
  m_par_function(m);
  // ----

  char* text = Screen::buffer;
  const uint8_t max_text = Screen::MaxCharsCanvasScrollbar; // 24, (so without zero)

  // Key:
  const char* key = "";  // TODO this "" does not take extra memory, so could remove PSTRING_EMPTY
  if (m.types & TypeCString) {
    key = static_cast<const char*>(m.name);
  } else if (m.types & TypePString) {
    key = GetPString(static_cast<const char*>(m.name));
  }
  strncpy(text, key, max_text);
  text[max_text] = 0;

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
    if (m.types & TypeRAlign) {
      memset(text + strlen(text), ' ', max_text-strlen(text)); // Pad end with spaces
      const uint8_t pos = max_text - strlen(value); // We expect the length of value to be < 24
      strcpy(text + pos, value);
    } else {
      const uint8_t freechar = max_text - strlen(text);
      strncat(text, value, freechar); // no need to terminate with zero, because already done above
    }
  }

  return text;
}

Screen::Inversion NewCombiline::GetInversion()
// NOTE: When the values are too long, the start and stop will be wrong
//       but the program will not crash, because this is solved by the Screen::Print
// TODO: combine GetText with GetInversion
{
  // ----
  NewParsPars m;
  m_par_function(m);
  // ---

  // --- No value ---
  if (m.number_of_values == 0)
    return {Screen::InvertAll, 0, 0};
  // --- Left aligned ---
  uint8_t start = (m.types & TypeCString) ? strlen(static_cast<const char*>(m.name)) :
                                            strlen(GetPString(static_cast<const char*>(m.name)));
  const char* value = (m.types & TypePTable) ? GetPStringFromPTable(static_cast<const char *const *>(m.values), *m_selected_value) :
                                               ((NewCombiLineFunction)m.values)(*m_selected_value);
  uint8_t stop = start + strlen(value);
  // --- Right aligned ---
  if (m.types & TypeRAlign) {
    start = Screen::MaxCharsCanvasScrollbar - strlen(value);
    stop = Screen::MaxCharsCanvasScrollbar - 1;
  }

  return {Screen::InvertGiven, start, stop};
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
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

//uint8_t Page::GetSelectedLine()
//{
//  return m_selected_line;
//}
//
//uint8_t Page::GetFirstLine()
//{
//  return m_first_line;
//}

/*
const char* Page::GetTitle()
{
  return GetPStringEmpty();
}
*/

void Page::Start(uint8_t parameter)
{
  OnStart(parameter);
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



Page::LineResult SingleCombiLine(
  Page::LineFunction func, 
  NewCombiline& combiline, uint8_t len, uint8_t extra_padding, bool right_align
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
  NewCombiline& combiline_1, uint8_t len_1, uint8_t extra_padding_1, bool right_align_1, 
  NewCombiline& combiline_2, uint8_t len_2, uint8_t extra_padding_2, bool right_align_2 
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

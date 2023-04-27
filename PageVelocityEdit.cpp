#include "PageVelocityEdit.h"

#include "Debug.h"
#include "Data.h"
#include "MidiFilter.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Roudi.h" // For SetRedrawNext

#ifdef ENABLE_VELOCITY_EDIT_PAGE

#include "DinMidiboy.h"

#include <midi_serialization.h> // for midi_event_t

namespace
{
  PSTRING(PSTR_line_0, "o                   v X /");
  PSTRING(PSTR_line_1, "u");
  PSTRING(PSTR_line_2, "t");
  PSTRING(PSTR_line_3, "p");
  PSTRING(PSTR_line_4, "u");
  PSTRING(PSTR_line_5, "t");
  PSTRING(PSTR_line_6, "  input");

  PSTRING(PSTR_coordinates, "(%d, %d)");
  PSTRING(PSTR_in, "in");
  PSTRING(PSTR_out, "out");

  void ListenIn(const midi_event_t& event, void* data)
  {
    PageVelocityEdit* page_velocity_edit = static_cast<PageVelocityEdit*>(data);

    // If we have a note on on the active input channel, we store its velocity in m_velocity_of_last_note
    if (event.m_event == 0x9 && event.m_data[2] !=0 && MidiFilter::IsActiveInputChannel(event))
    {
      page_velocity_edit->m_velocity_of_last_note = event.m_data[2];
      SetRedrawNext();
    }
  }

  const uint8_t left_x = 12;
  const uint8_t left_y = 10;
  const uint8_t factor_x = 5;
  const uint8_t factor_y = 3;

  void DrawPixel(uint8_t pos_x, uint8_t pos_y)
  {
    uint8_t line_pos_y = pos_y >> 3;
    uint8_t bit_to_set = pos_y - (line_pos_y << 3);
    DinMidiboy.setDrawPosition(pos_x, line_pos_y);
    DinMidiboy.drawBits(0x01 << bit_to_set, 1, false);
  }

  void DrawCursor(uint8_t pos_x, uint8_t pos_y)
  {
    const uint8_t n = 2; // max 4
    uint8_t bytes[2] = {0, 0};
    uint8_t lowest_line = (pos_y - n) >> 3;
    for (uint8_t y = pos_y - n; y <= pos_y + n; y++) {
      uint8_t line_pos_y = y >> 3;
      uint8_t bit_to_set = y - (line_pos_y << 3);
      bytes[line_pos_y - lowest_line] += (0x01 << bit_to_set);
    } 
    DinMidiboy.setDrawPosition(pos_x, lowest_line);
    DinMidiboy.drawBits(bytes[0], 1, false);
    if (bytes[1]) {
      DinMidiboy.setDrawPosition(pos_x, lowest_line+1);
      DinMidiboy.drawBits(bytes[1], 1, false);
    }
  }

  void DrawVelocityIn(uint16_t vel_in)
  {
    uint8_t pos_x = static_cast<uint16_t>(left_x) + (vel_in - 1) * (16 * static_cast<uint16_t>(factor_x)) / (127 - 1);
    for (uint8_t line = 1; line < 7; line++) {
      DinMidiboy.setDrawPosition(pos_x, line);
      DinMidiboy.drawBits(0x55, 1, false);
    }
  }

}

PageVelocityEdit::PageVelocityEdit(): Page()
{
}

void PageVelocityEdit::OnStart(uint8_t which_map)
{
  // Init members
  SetNumberOfLines(7, 0);
  m_position = 0;
  m_velocity_of_last_note = 0;

  // Read the velocity map
  m_which = which_map;
  EE::GetVelocityMap(m_which, m_velocity_map);

  // Attach listener
  MidiProcessing::SetMidiInListener({ListenIn, this});  
}

void PageVelocityEdit::OnStop()
{
  // Detach listener
  MidiProcessing::SetMidiInListener({nullptr, nullptr});

  // Set the velocity map specified in the settings
  SetVelocityCurve(EE::Settings().velocity_curve);
}

const char* PageVelocityEdit::GetTitle()
{
  return GetPString(PSTR_velocity_curve_edit_title);
}

Page::LineResult PageVelocityEdit::Line(LineFunction func, uint8_t line, uint8_t field)
{
  const char* text = nullptr;
  Screen::Inversion inversion = Screen::inversion_none;
  bool redraw = false;

  if (func == GET_TEXT) {
    switch(line) {
      case 0: text = GetPString(PSTR_line_0); 
        if (m_position == 17)
          inversion = { Screen::InvertGiven, 20, 20 };
        else if (m_position == 18)
          inversion = { Screen::InvertGiven, 22, 22 };
        else if (m_position == 19)
          inversion = { Screen::InvertGiven, 24, 24 };
        break;
      case 1: text = GetPString(PSTR_line_1); break;
      case 2: text = GetPString(PSTR_line_2); break;
      case 3: text = GetPString(PSTR_line_3); break;
      case 4: text = GetPString(PSTR_line_4); break;
      case 5: text = GetPString(PSTR_line_5); break;
      case 6: text = GetPString(PSTR_line_6); break;
      default: text = GetPString(PSTR_empty); break;
    }
  } else if (func == DO_LEFT && m_position > 0) {
    m_position--;
    redraw = true;
  } else if (func == DO_RIGHT && m_position < 19) {
    m_position++;
    redraw = true;
  }

  return {1, text, inversion, redraw};
}

bool PageVelocityEdit::OnUpDown(UpDownAction action)
{
  bool redraw = false;
  if (m_position < 17) {
    const uint8_t increment = 1;
    uint8_t old_value = m_velocity_map[m_position];
    if (action == UP) {
      if (m_velocity_map[m_position] <= 127 - increment) 
        m_velocity_map[m_position] += increment;
      else
        m_velocity_map[m_position] = 127;
    } else {
      if (m_velocity_map[m_position] > increment) 
        m_velocity_map[m_position] -= increment;
      else
        m_velocity_map[m_position] = 1;
    }
    redraw = (m_velocity_map[m_position] != old_value);    
  } else if (m_position == 17) { // ACCEPT
    EE::SetVelocityMap(m_which, m_velocity_map);
    Pages::SetNextPage(PAGE_VELOCITY_SELECT, m_which);
  } else if (m_position == 18) { // CANCEL
    Pages::SetNextPage(PAGE_VELOCITY_SELECT, m_which);
  } else if (m_position == 19) { // RESET
    EE::GetVelocityMap(0, m_velocity_map);
    redraw = true;
  }

  if (redraw)
    SetVelocityMap(m_velocity_map);
  return redraw;
}

void PageVelocityEdit::Draw(uint8_t from, uint8_t to)
// Due to a lack of program space and RAM, I cannot add the code to optimize drawing.
// So, it is a bit slow, but still OK.
{
  Page::Draw(from, to);
  const char* curve_name = GetVelocityCurveName(m_which);
  Screen::Print(Screen::CanvasScrollbar, 6, 19 - strlen(curve_name), curve_name, Screen::LineLeave, Screen::inversion_none);

  // --- Draw the curve ---
  for (uint8_t i = 0; i < 17; i++) {
    uint8_t pos_x = left_x + i * factor_x;
    uint8_t pos_y = left_y + (m_velocity_map[i] / factor_y);

#ifdef ENABLE_VELOCITY_EDIT_PAGE_SIMPLE_GRAPHICS
    DrawPixel(pos_x, pos_y);
    if (i==m_position) {
      DrawPixel(pos_x + 1, pos_y);
      DrawPixel(pos_x - 1, pos_y);
    }
#else
    if (i==m_position) {
      DrawCursor(pos_x, pos_y);
    } else {
      DrawPixel(pos_x, pos_y);
    }
    if (i > 0) {
      const int16_t delta = m_velocity_map[i] - m_velocity_map[i-1];
      const int16_t rounding = delta >= 0 ? +(factor_x*factor_y)>>1 : -((factor_x*factor_y)>>1);
      for (uint8_t x = 1; x < factor_x; x++)
        DrawPixel(pos_x - x, pos_y - (x*delta+rounding)/(factor_x*factor_y));
    }
#endif
   }

  // --- Draw cursor coordinates ---
  if (m_position < 17) {
    const uint8_t x = m_position == 0 ? 1 : m_position * 8 - 1;
    const uint8_t y = m_velocity_map[m_position];
    sprintf(Screen::buffer, GetPString(PSTR_coordinates), x, y);
    Screen::Print(Screen::CanvasScrollbar, 7, 19 - strlen(Screen::buffer), Screen::buffer, Screen::LineLeave, Screen::inversion_none);
  }

  // --- Draw played notes ---
  if (m_velocity_of_last_note != 0) {
    Screen::Print(Screen::CanvasScrollbar, 4, 22, GetPString(PSTR_in), Screen::LineLeave, Screen::inversion_none);
    Screen::Print(Screen::CanvasScrollbar, 6, 22, GetPString(PSTR_out), Screen::LineLeave, Screen::inversion_none);
    Screen::Print(Screen::CanvasScrollbar, 5, 22, GetNumber(m_velocity_of_last_note), Screen::LineLeave, Screen::inversion_none);
    Screen::Print(Screen::CanvasScrollbar, 7, 22, GetNumber(MapVelocity(m_velocity_of_last_note)), Screen::LineLeave, Screen::inversion_none);
    DrawVelocityIn(m_velocity_of_last_note);
  }

}

#else // ENABLE_VELOCITY_EDIT_PAGE

namespace
{
  PSTRING(PSTR_velocity_edit_disabled_1, "Velocity curve edit was");
  PSTRING(PSTR_velocity_edit_disabled_2, "disabled to save program");
  PSTRING(PSTR_velocity_edit_disabled_3, "space.");
}

PageVelocityEdit::PageVelocityEdit(): Page()
{
}

void PageVelocityEdit::OnStart(uint8_t which_map)
{
  m_which = which_map;
  SetNumberOfLines(3);
}

const char* PageVelocityEdit::GetTitle()
{
  return GetPString(PSTR_velocity_curve_edit_title);
}

Page::LineResult PageVelocityEdit::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (func==DO_LEFT || func==DO_RIGHT)
    Pages::SetNextPage(PAGE_VELOCITY_SELECT, m_which);

  const char* p;
  if (line == 0)
    p = PSTR_velocity_edit_disabled_1;
  else if (line == 1)
    p = PSTR_velocity_edit_disabled_2;
  else
    p = PSTR_velocity_edit_disabled_3;
  return Page::LineResult{1, GetPString(p), Screen::inversion_none, false};
}

#endif // ENABLE_VELOCITY_EDIT_PAGE
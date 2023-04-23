#include "PageVelocityEdit.h"

#include "Debug.h"
#include "Data.h"
#include "MidiFilter.h"
#include "MidiProcessing.h"
#include "Pages.h"
#include "Roudi.h" // For SetRedrawNext

#include "DinMidiboy.h"

#include <midi_serialization.h> // for midi_event_t

namespace
{
  PSTRING(PSTR_page_velocity_edit, " VELOCITY ");

  PSTRING(PSTR_line_0, "o                     v X");
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

    // If we have a note on on the active input channel, we store its velocity in m_last_note_on
    if (event.m_event == 0x9 && event.m_data[2] !=0 && MidiFilter::IsActiveInputChannel(event))
    {
      page_velocity_edit->m_last_note_on = event.m_data[2];
      SetRedrawNext();
    }
  }

/*
  const uint8_t new_linear_map[] = {
      1,
      7,  15,  23,  31, 
     39,  47,  55,  63,
     71,  79,  87,  95, 
    103, 111, 119, 127 
  };
*/

  uint8_t new_custom_map[] = {
    /*   1 -> */ 1,
    /*   7 -> */ 1,  
    /*  15 -> */ 1,  
    /*  23 -> */ 7,  
    /*  31 -> */ 31,
    /*  39 -> */  42,  
    /*  47 -> */  53,  
    /*  55 -> */ 65,  
    /*  63 -> */  73,
    /*  71 -> */  81,
    /*  79 -> */  89,  
    /*  87 -> */ 98,
    /*  95 -> */  112,
    /* 103 -> */ 127, 
    /* 111 -> */ 127, 
    /* 119 -> */ 127, 
    /* 127 -> */ 127,
  };

}

PageVelocityEdit::PageVelocityEdit(): Page()
{
}

void PageVelocityEdit::OnStart(uint8_t)
{
  // Init members
  SetNumberOfLines(7, 0);
  m_position = 0;
  m_last_note_on = 0;

  // Attach listener
  MidiProcessing::SetMidiInListener({ListenIn, this});  

//  SetMidiConfiguration(values.channel);
}

void PageVelocityEdit::OnStop()
{
  // Detach listener
  MidiProcessing::SetMidiInListener({nullptr, nullptr});
//  SaveIfModified();
}

const char* PageVelocityEdit::GetTitle()
{
  return GetPString(PSTR_page_velocity_edit);
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
          inversion = { Screen::InvertGiven, 22, 22 };
        else if (m_position == 18)
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
  } else if (func == DO_RIGHT && m_position < 18) {
    m_position++;
    redraw = true;
  }

  return {1, text, inversion, redraw};

}

bool PageVelocityEdit::OnUpDown(UpDownAction action)
{
  if (m_position < 17) {
/*
    uint8_t old_value = new_custom_map[m_position];
    uint8_t increment = (action == UP) ? +4 : -4;
    uint8_t new_value = old_value + increment;
    if (new_value > 196)
      new_value = 0;
    else if (new_value > 127)
      new_value = 127;
    new_custom_map[m_position] = new_value;
    return (new_value != old_value);
*/
    const uint8_t increment = 3;
    uint8_t old_value = new_custom_map[m_position];
    if (action == UP) {
      if (new_custom_map[m_position] <= 127 - increment) 
        new_custom_map[m_position] += increment;
      else
        new_custom_map[m_position] = 127;
    } else {
      if (new_custom_map[m_position] > increment) 
        new_custom_map[m_position] -= increment;
      else
        new_custom_map[m_position] = 1;
    }
    return (new_custom_map[m_position] != old_value);    
  } else if (m_position == 17) { // ACCEPT
    Debug::BeepHigh();
  } else if (m_position == 18) { // CANCEL
    Debug::BeepLow();
  }
  return false;
}



static void DrawPixel(uint8_t pos_x, uint8_t pos_y)
{
  uint8_t line_pos_y = pos_y >> 3;
  uint8_t bit_to_set = pos_y - (line_pos_y << 3);
  DinMidiboy.setDrawPosition(pos_x, line_pos_y);
  DinMidiboy.drawBits(0x01 << bit_to_set, 1, false);
}

void PageVelocityEdit::Draw(uint8_t from, uint8_t to)
{
  Page::Draw(from, to);

  const uint8_t left_x = 12;
  const uint8_t left_y = 10;
  const uint8_t factor_x = 5;
  const uint8_t factor_y = 3;

  for (uint8_t i = 0; i < 17; i++) {
    uint8_t pos_x = left_x + i * factor_x;
    uint8_t pos_y = left_y + (new_custom_map[i] / factor_y);
    DrawPixel(pos_x, pos_y);
    if (i > 0) {
      const int16_t delta = new_custom_map[i] - new_custom_map[i-1];
      const int16_t rounding = delta >= 0 ? +(factor_x*factor_y)>>1 : -((factor_x*factor_y)>>1);
      for (uint8_t x = 1; x < factor_x; x++)
        DrawPixel(pos_x - x, pos_y - (x*delta+rounding)/(factor_x*factor_y));
    }
    if (i==m_position) {
      DrawPixel(pos_x + 1, pos_y);
      DrawPixel(pos_x - 1, pos_y);
    }
   }

  // --- Draw coordinates ---
  if (m_position < 17) {
    const uint8_t x = m_position == 0 ? 1 : m_position * 8 - 1;
    const uint8_t y = new_custom_map[m_position];
    sprintf(Screen::buffer, GetPString(PSTR_coordinates), x, y);
    Screen::Print(Screen::CanvasScrollbar, 7, 19 - strlen(Screen::buffer), Screen::buffer, Screen::LineLeave, Screen::inversion_none);
  }

  // --- Draw played notes ---
  if (m_last_note_on != 0) {
    Screen::Print(Screen::CanvasScrollbar, 4, 22, GetPString(PSTR_in), Screen::LineLeave, Screen::inversion_none);
    Screen::Print(Screen::CanvasScrollbar, 6, 22, GetPString(PSTR_out), Screen::LineLeave, Screen::inversion_none);
    Screen::Print(Screen::CanvasScrollbar, 5, 22, GetNumber(m_last_note_on), Screen::LineLeave, Screen::inversion_none);
    Screen::Print(Screen::CanvasScrollbar, 7, 22, GetNumber(m_last_note_on * 2), Screen::LineLeave, Screen::inversion_none);
  }

}


void PageVelocityEdit::SetMidiConfiguration(uint8_t selected_line)
{
/*
  MidiProcessing::Configuration next_config;
  next_config.m_input_channel = EE::Settings().input_channel;
  if (selected_line < NumberOfChannels) {
    next_config.m_nbr_output_channels = 1;
    next_config.m_output_channel[0].m_channel = selected_line;
  } else {
    next_config.m_nbr_output_channels = 0;
  }
  MidiProcessing::SetNextConfiguration(next_config);
*/
}

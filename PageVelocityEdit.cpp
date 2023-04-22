#include "PageVelocityEdit.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Pages.h"

#include "DinMidiboy.h"

namespace
{
  PSTRING(PSTR_page_velocity_edit, " VELOCITY ");

  PSTRING(PSTR_line_0, "o                     v X");
  PSTRING(PSTR_line_1, "u                     ");
  PSTRING(PSTR_line_2, "t                     ");
  PSTRING(PSTR_line_3, "p                     in");
  PSTRING(PSTR_line_4, "u                     127");
  PSTRING(PSTR_line_5, "t                     out");
  PSTRING(PSTR_line_6, "      i n p u t       127");

  PSTRING(PSTR_coordinates, "(%d, %d)");


  const uint8_t left_x = 12;
  const uint8_t left_y = 10;

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
//  SingleValues values;
//  EE::GetSingle(values);
  SetNumberOfLines(7, 0);
  m_position = 0;
//  SetMidiConfiguration(values.channel);
}

void PageVelocityEdit::OnStop()
{
//  SaveIfModified();
}

const char* PageVelocityEdit::GetTitle()
{
  return GetPString(PSTR_page_velocity_edit);
}

Page::LineResult PageVelocityEdit::Line(LineFunction func, uint8_t line, uint8_t field)
{
/*
  if (func == DO_LEFT && m_position > 0) {
    m_position--;
    return Page::LineResult{1, nullptr, Screen::inversion_none, true};
  }
  if (func == DO_RIGHT && m_position < 19) {
    m_position++;
    return Page::LineResult{1, "", Screen::inversion_none, true};
  }
  switch(line) {
    case 0: return Page::LineResult{1, GetPString(PSTR_fff), Screen::inversion_none, false};
    case 5: return Page::LineResult{1, GetPString(PSTR_ppp), Screen::inversion_none, false};
    case 6: return TextLine(func, PSTR_vel);
  }
  return DefaultLine(func);
*/


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
/*
  Screen::Print(Screen::CanvasScrollbar, 3, 5, "  res.text", Screen::LineLeave, Screen::inversion_none);
*/
/*
  for (uint8_t i = 1; i <= 6; ++i) {
    DinMidiboy.setDrawPosition(left_x, i);
    DinMidiboy.drawBits(0xFF, 1, false);
  }
  DinMidiboy.setDrawPosition(left_x + 1, 1);
  DinMidiboy.drawBits(0x01, 64, false);
*/

//  if (m_position < 17) {
//    DinMidiboy.setDrawPosition(left_x + m_position * 4, 2);
//    DinMidiboy.drawBits(0xF0, 1, false);
//  }

  for (uint8_t i=0; i<17; i++) {
    uint8_t pos_x = left_x + i * 5;
    uint8_t pos_y = left_y + (new_custom_map[i] / 3);
    DrawPixel(pos_x, pos_y);
    if (i==m_position) {
      DrawPixel(pos_x + 1, pos_y);
      DrawPixel(pos_x - 1, pos_y);
    }
//    DrawPixel(pos_x, pos_y + 1);
//    DrawPixel(pos_x, pos_y - 1);

    /*
    uint8_t line_pos_y = pos_y >> 3;
    uint8_t bit_to_set = pos_y - (line_pos_y << 3);
    DinMidiboy.setDrawPosition(pos_x, line_pos_y);
    DinMidiboy.drawBits(0x01 << bit_to_set, 1, false);
    DinMidiboy.drawBits(0x81, 1, false);
    */
  }

  // --- Draw coordinates ---
  if (m_position < 17) {
    const uint8_t x = m_position == 0 ? 1 : m_position * 8 - 1;
    const uint8_t y = new_custom_map[m_position];
    sprintf(Screen::buffer, GetPString(PSTR_coordinates), x, y);
    Screen::Print(Screen::CanvasScrollbar, 5, 19 - strlen(Screen::buffer), Screen::buffer, Screen::LineLeave, Screen::inversion_none);
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

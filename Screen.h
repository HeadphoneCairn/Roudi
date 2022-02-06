#pragma once

#include <Arduino.h> // for uint8_t

namespace Screen
{
  enum CanvasType {
    CanvasComplete,
    CanvasScrollbar
  };

  const uint8_t MaxCharsCanvas = 25;

  // Use buffer as temporary space to manipulate a string before sending it to the screen
  // It has enough space for a string with the length of the full the screen (plus place for \0).
  const uint8_t buffer_len = MaxCharsCanvas;
  extern char buffer[buffer_len + 1];

  enum LineStyle { 
    LineClear,     // Non-text part of line is cleared 
    LineLeave,     // Non-text part of line is unchanged
    LineEmphasis,  // Non-text part of line is emphasised (like ___)
  };

  enum InversionStyle {
    InvertNone,
    InvertGiven,  
    InvertAll
  };

  struct Inversion
  {
    InversionStyle style;
    uint8_t start;  // when InvertGiven, start and stop
    uint8_t stop;   // tell which part of the text needs to be inverted
  };

  const Inversion inversion_none = {Screen::InvertNone, 0, 0};
  const Inversion inversion_all = {Screen::InvertAll, 0, 0};

  // Print text to screen
  void Print(
    CanvasType canvas_type,
    uint8_t row,    // 0..7
    uint8_t column, // 0..24, 0xFF=center string
    const char *text,
    LineStyle line_style,
    Inversion inversion
    );

  // Draw a filled rectangle, x and y are pixel coordinates
  void DrawScrollbar(uint8_t y1, uint8_t y2);

  // Clear the screen
  void Clear(bool inverse = false);

  // Show a full screen bitmap that is stored in PROGMEM 
  void ShowBitmap(const uint8_t* i_bitmap_data, uint8_t i_number_blocks, uint8_t i_horizontal_size, bool i_inverse);

  // Draw a wiggly line on a specified row
  void DrawWiggle(uint8_t row);
}
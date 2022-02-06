#include "screen.h"

#include "DinMidiboy.h"

#define FONT MIDIBOY_FONT_5X7

namespace Screen
{
  char buffer[buffer_len + 1];
  const uint8_t font_width = 4;

  // Remarks:
  // - A string will always start and end with empty 1 pixel column.
  // - When InvertGiven is used, pixel column left and right from stop and start will also be inverted.
  // - We will only allow as many characters as can fit in the canvas.
  //   Current font is 4x7 (we truncate 5x7 font at 4)
  //     0..127: 1 + 25 * (4 + 1) = 126 pixels, 2 left unused
  // - canvas_type
  //   - CanvasComplete, 0..127: all canvas is cleared if requested
  //   - CanvasScrollbar, 0..125: the last two pixels are not modified 
  // - If column > possible chars on canvas, we wrap
  //   if row > possible lines (8), we wrap
  // - Too long text is cut off
  void Print(
    CanvasType canvas_type,
    uint8_t row,    // 0..7
    uint8_t column, // 0..24, 0xFF=center string
    const char *text,
    LineStyle line_style,
    Inversion inversion
    )
  { 
//    const uint8_t emphasis_bits = 0b00000010;
//    const uint8_t emphasis_bits = 0b00111100;
    const uint8_t emphasis_bits = 0b01010100;

    // --- Some canvas calculations ---
    const uint8_t canvas_min = 0; 
    const uint8_t canvas_max = canvas_type == CanvasComplete ? 127 : 125;
    const uint8_t canvas_width = canvas_max - canvas_min + 1; 
    const uint8_t canvas_max_chars = (canvas_width - 1) / (font_width + 1);

    // --- Determine start x and y position for text ---
    const uint8_t text_len = static_cast<size_t>(strlen(text));
    const uint8_t y_pos = 7 - (row & 7); // wrap row
    uint8_t x_pos = canvas_min;
    if (column != 0xFF) { // Text at given column
      column %= canvas_max_chars;
      x_pos += column * (font_width + 1); // we wrap when position is further than allowed
    } else { // Text at center
      if (text_len <= canvas_max_chars)
        x_pos += (canvas_width - (1 + text_len * (font_width + 1))) / 2;
      else
        x_pos += 0;
    }

    // --- Draw part until text ---
    const uint8_t begin_length = x_pos - canvas_min;
    DinMidiboy.setDrawPosition(canvas_min, y_pos);
    if (line_style == LineClear)
        DinMidiboy.drawSpace(begin_length, inversion.style == InvertAll);
    else if (line_style == LineEmphasis)
        DinMidiboy.drawBits(emphasis_bits, begin_length, inversion.style == InvertAll);
    
    // --- Draw text ---
    DinMidiboy.setDrawPosition(x_pos, y_pos);
    DinMidiboy.drawSpace(1, inversion.style == InvertAll || (inversion.style == InvertGiven && inversion.start == 0));
    uint8_t i = 0;
    uint8_t max_space_for_text = column == 0xFF ? canvas_max_chars : canvas_max_chars - column; // always > 0 because column < canvas_max_chars because of % above
    for (const char *p=text; *p && i < max_space_for_text; p++, i++) {
      const uint8_t* character = &FONT::DATA_P[(*p - ' ') * FONT::WIDTH];
      DinMidiboy.drawBitmap_P(character, 4, inversion.style == InvertAll || (inversion.style == InvertGiven && i >= inversion.start && i <= inversion.stop));
      DinMidiboy.drawSpace(1, inversion.style == InvertAll || (inversion.style == InvertGiven && ((i >= inversion.start && i <= inversion.stop) || i + 1 == inversion.start)));
    }

    // --- Draw part after text ---
    const uint8_t end_length = canvas_width - (x_pos + 1 + i * (font_width + 1));
    if (line_style == LineClear)
        DinMidiboy.drawSpace(end_length, inversion.style == InvertAll);
    else if (line_style == LineEmphasis)
        DinMidiboy.drawBits(emphasis_bits, end_length, inversion.style == InvertAll);

  }

  void DrawScrollbar(uint8_t i_y1, uint8_t i_y2)
  {
    // Width scroll bar
    const uint8_t x1 = 127;
    const uint8_t x2 = 127;
    // Constrain y1, y2
    if (i_y1 > 63) i_y1 = 63;
    if (i_y2 > 63) i_y2 = 63;
    // Invert y1, y2, because real screen coordinates are upside down
    const uint8_t y1 = 63 - i_y2;
    const uint8_t y2 = 63 - i_y1;

    for (uint8_t i = 0; i < 7; i++) {
      const uint8_t b = i << 3;
      const uint8_t e = b + 7;
      uint8_t pixels_1x8;
      if (b > y2 || e < y1) // no overlap
        pixels_1x8 = 0x00;
      else if (b >= y1 && e <= y2) // full overlap
        pixels_1x8 = 0xff;
      else { // some overlap
        uint8_t bb = (b < y1) ? y1 : b;
        uint8_t ee = (e > y2) ? y2 : e;
        bb = bb & 0x07;
        ee = ee & 0x07;
        pixels_1x8 = 0x00;
        for (int j = bb; j <= ee; j++)
          pixels_1x8 += (1 << j);
      }
      DinMidiboy.setDrawPosition(x1, i);
      DinMidiboy.drawBits(pixels_1x8, x2 - x1 + 1, false);
    }
  }

  void Clear(bool inverse)
  {
    if (inverse) {
      for (uint8_t y = 0; y < 8; ++y) {      
        DinMidiboy.setDrawPosition(0, y);
        DinMidiboy.drawBits(0xff, 128, false);
      }
    } else {
      DinMidiboy.clearScreen();
    }
  }


  void ShowBitmap(const uint8_t* i_bitmap_data, uint8_t i_number_blocks, uint8_t i_horizontal_size, bool i_inverse)
  {
    DinMidiboy.clearScreen();
    uint8_t number_blocks = min(8, i_number_blocks);
    uint8_t horizontal_size = min(128, i_horizontal_size);
    uint8_t vertical_pos = (8 - number_blocks) / 2;
    uint8_t horizontal_pos = (128 - horizontal_size) / 2; 
    for (uint8_t block = 0; block < number_blocks; ++block) {
      DinMidiboy.setDrawPosition(horizontal_pos, 7 - (vertical_pos + block));
      const uint8_t* block_start = i_bitmap_data + static_cast<uint16_t>(block) * static_cast<uint16_t>(i_horizontal_size); 
      DinMidiboy.drawBitmap_P(block_start, horizontal_size, i_inverse);
    }
  }

  // Draw a wiggly line on a specified row
  void DrawWiggle(uint8_t row)
  {
    const uint8_t wiggle[] = {
      0b10000001,
      0b01000010,
      0b00100100,
      0b10011001,
      0b10011001,
      0b00100100,
      0b01000010,
    };
    DinMidiboy.setDrawPosition(0, 7 - (row & 7));
    for (uint8_t i = 0; i < 128; i++)
      DinMidiboy.drawBits(wiggle[i%(sizeof(wiggle)/sizeof(wiggle[0]))], 1, false);
  };





/*
  // To test:
  // - extra long text
  // - zero length text
  // - column wrap
  // - row wrap
  // - InversionStyles
  // - LineStyles
  // - CanvasTypes

  void DrawTest()
  {
    Clear();
    Print(CanvasComplete, 0, 0xFF, " A - SINGLE ", LineEmphasis, inversion_none);
    DrawWiggle(1);
    Print(CanvasScrollbar, 2, 0, "01. Pianodubekeklekjejkljejeklej", LineClear, inversion_all);
    Print(CanvasScrollbar, 3, 15, "Kut met peren", LineLeave, inversion_all);
    Print(CanvasScrollbar, 4, 0xFF, "Midden", LineLeave, inversion_all);
    Print(CanvasScrollbar, 5, 2, "En nog meer midden", LineLeave, inversion_all);
    Print(CanvasScrollbar, 6, 4, "En dit is een wreed lange tekst, wellicht te lang", LineClear, inversion_all);
    Print(CanvasScrollbar, 7, 6, "", LineClear, inversion_none);
  }
  void DrawA()
  {
    Clear();
    Print(CanvasScrollbar, 0, 0xFF, " A - SINGLE ", LineEmphasis, inversion_none);
    Print(CanvasScrollbar, 1, 1, "    Disabled", LineClear, inversion_all);
    Print(CanvasScrollbar, 2, 1, "01. Piano", LineClear, inversion_none);
    Print(CanvasScrollbar, 3, 1, "02. Erebus", LineClear, inversion_none);
    Print(CanvasScrollbar, 4, 1, "03. Typhon", LineClear, inversion_none);
    Print(CanvasScrollbar, 5, 1, "04. Prophet 6", LineClear, inversion_none);
  }
  void DrawUISettings()
  {
    Clear();
    Print(CanvasScrollbar, 0, 0xFF, " UI Settings ", LineEmphasis, inversion_none);
    Print(CanvasScrollbar, 1, 0, "Channel order      alpha", LineClear, inversion_none);
    Print(CanvasScrollbar, 2, 0, "Channel number      show", LineClear, {InvertGiven, 20, 23});
    Print(CanvasScrollbar, 3, 0, "Split inc/dec     octave", LineClear, inversion_none);
    Print(CanvasScrollbar, 4, 0, "Screen brightness     50", LineClear, {InvertGiven, 0, 16});
  }
*/
  
}

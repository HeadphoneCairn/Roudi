#include "DinMidiboy.h"

#include <Arduino.h>
#include <avr/pgmspace.h>

#include "Screen.h"

static const PROGMEM uint8_t logo_data[] =
{
  // block 0
  0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x03, 0x03, 0x03, 0x00, 0x00, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 0x03,
  0x01, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x01, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x3f, 0x3f, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x3f, 0x3f, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // block 1
  0xff, 0xff, 0xff, 0x18, 0x18, 0x18, 0x18, 0x18,
  0xff, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0xff,
  0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00,
  0x00, 0x7f, 0xff, 0xff, 0x86, 0x06, 0x06, 0x86,
  0xff, 0xff, 0x7f, 0x00, 0x00, 0xff, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x80, 0xff, 0xff, 0xff, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x30, 0x30,
  0x3f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x3f, 0x3f, 0x30, 0x30, 0x30, 0x30, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // block 2
  0xe0, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xe0, 0xe0, 0xe0, 0x00, 0x00, 0xe0, 0xe0, 0xe0,
  0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x60, 0x00,
  0x00, 0xe0, 0xe0, 0xe0, 0x00, 0x00, 0x00, 0x00,
  0xe0, 0xe0, 0xe0, 0x00, 0x00, 0xe0, 0xe0, 0xe0,
  0x60, 0x60, 0x60, 0xe0, 0xe0, 0xc0, 0x80, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x00, 0x00, 0x00, 0x00,
  0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0xfc, 0xfc, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0xfc,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xfc, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x03, 0x03,
  0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03,
  // block 3
  0x7f, 0x7f, 0x61, 0x61, 0x61, 0x73, 0x3f, 0x1e,
  0x00, 0x00, 0x7f, 0x7f, 0x03, 0x03, 0x03, 0x03,
  0x7f, 0x7f, 0x00, 0x00, 0x1f, 0x3f, 0x70, 0x60,
  0x60, 0x70, 0x3f, 0x1f, 0x00, 0x00, 0x7f, 0x7f,
  0x3c, 0x0f, 0x03, 0x00, 0x7f, 0x7f, 0x00, 0x00,
  0x7f, 0x7f, 0x63, 0x63, 0x63, 0x63, 0x60, 0x60,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01,
  0x03, 0x07, 0x06, 0x06, 0x07, 0x03, 0x01, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x01, 0x03, 0x07, 0x06, 0x06,
  0x07, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  // block 4
  0xf8, 0xf8, 0x80, 0x80, 0x80, 0x80, 0x00, 0x00,
  0x00, 0x00, 0xf8, 0xf8, 0x00, 0x00, 0x00, 0x00,
  0xf8, 0xf8, 0x00, 0x00, 0xe0, 0xf0, 0x38, 0x18,
  0x18, 0x38, 0xf0, 0xe0, 0x00, 0x00, 0xf8, 0xf8,
  0x00, 0x00, 0xc0, 0xf0, 0xf8, 0xf8, 0x00, 0x00,
  0xf8, 0xf8, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf0,
  0xf8, 0x1c, 0x0c, 0x0c, 0x1c, 0xf8, 0xf0, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0xf0, 0xf8, 0x1c, 0x0c, 0x0c,
  0x1c, 0xf8, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff,
  0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
  // block 5
  0x07, 0x0f, 0x1c, 0x18, 0x18, 0x1c, 0x0c, 0x04,
  0x00, 0x00, 0x03, 0x07, 0x0c, 0x18, 0x18, 0x0c,
  0x07, 0x03, 0x00, 0x00, 0x00, 0x18, 0x18, 0x1f,
  0x1f, 0x18, 0x18, 0x00, 0x00, 0x00, 0x1f, 0x1f,
  0x18, 0x18, 0x18, 0x1c, 0x0f, 0x07, 0x00, 0x00,
  0x1f, 0x1f, 0x0f, 0x03, 0x00, 0x00, 0x1f, 0x1f,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  0xc0, 0xc0, 0xc0, 0xc0, 0x00, 0x00, 0x00, 0x00,
  0x3f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x02, 0x06, 0x0c, 0x18, 0x18, 0x0c, 0x06,
  0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x3f, 0x3f, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0,
  0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0, 0xc0,
  // block 6
  0xf8, 0xfc, 0x0e, 0x06, 0x06, 0x0e, 0x0c, 0x08,
  0x00, 0x00, 0xfe, 0xfe, 0x30, 0x30, 0x30, 0x30,
  0xfe, 0xfe, 0x00, 0x00, 0x00, 0x06, 0x06, 0xfe,
  0xfe, 0x06, 0x06, 0x00, 0x00, 0x00, 0xfe, 0xfe,
  0x60, 0x60, 0x60, 0xf0, 0xde, 0x8e, 0x00, 0x00,
  0xfe, 0xfe, 0x00, 0xc0, 0xf0, 0x3c, 0xfe, 0xfe,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0xf0, 0xf0, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
  0x30, 0x30, 0x3f, 0x3f, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0x3f,
  0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30, 0x30,
  0xf0, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  // block 7
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0xfe, 0xfe, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0xfe,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
  0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x06,
};
static const uint8_t logo_number_blocks = 8;
static const uint8_t logo_horizontal_size = 128;

void ShowLogo()
{
  Screen::ShowBitmap(logo_data, logo_number_blocks, logo_horizontal_size, false);
}
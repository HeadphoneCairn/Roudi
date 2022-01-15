#pragma once

#include "Screen.h"
#include "Combiline.h"

// --- Page ---

class Page
{
public:
  Page();
  void SetNumberOfLines(uint8_t number_of_lines, uint8_t selected_line = 0, uint8_t first_line = 0xFF); // Must be called before first call to Start()!
//  uint8_t GetSelectedLine();
//  uint8_t GetFirstLine();
public:
  void Start(); // Call this when you want to first display the page
  void Stop(); // Call this when you want to exit the page
  void Up();
  void Down();
  void Left();
  void Right();
public:
  enum LineFunction { GET_NUMBER_OF_FIELDS, GET_TEXT, DO_SELECTED, DO_LEFT, DO_RIGHT };
  struct LineResult {
    uint8_t number_of_fields;
    const char* text;
    Screen::Inversion text_inversion;
    bool redraw;
  };
protected:// to implement
  // Number of virtual functions needs to be kept as small as possible since each function
  // requires an extra 2 bytes of dynamic memory, for each subclass! 
  virtual const char* GetTitle() = 0; // Returned string must be alive for as long the page is active
  // field is number of selected field, -1 if none is selected
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field);
  virtual void OnStart() { return; }
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) { return; }
  enum UpDownAction { UP, DOWN };
  virtual bool OnUpDown(UpDownAction action); // returns whether to redraw screen
private:
  void Draw(uint8_t from = 0, uint8_t to = 7);
private:
  uint8_t m_number_of_lines;
  uint8_t m_first_line; // the first displayed line
  uint8_t m_selected_line; // the selected line: 0..m_number_of_lines-1
  uint8_t m_selected_field; // the selected field on the line: 0..
};

#define gPageBufferSize 45
extern Page* g_page;

// --- Line help functions ---
Page::LineResult DoubleCombiline(
  Page::LineFunction func, uint8_t field, 
  NewCombiline& combiline_1, uint8_t len_1, uint8_t extra_padding_1, bool right_align_1, 
  NewCombiline& combiline_2, uint8_t len_2, uint8_t extra_padding_2, bool right_align_2 
);

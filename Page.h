#pragma once

#include "Screen.h"
#include "Combiline.h"

// --- Page ---

class Page
{
public:
  Page();
  // SetNumberOfLines() must be called in the OnStart()!
  // TODO: simplify or remove???
  void SetNumberOfLines(uint8_t number_of_lines, uint8_t selected_line = 0xFF, uint8_t selected_field = 0xFF, uint8_t first_line = 0xFF); 
//  uint8_t GetSelectedLine();
//  uint8_t GetFirstLine();
public:
  void Start(uint8_t parameter = 0xFF); // Call this when you want to first display the page
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
  // field is number of selected field, 0xFF if none is selected
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) = 0;
  virtual void OnStart(uint8_t parameter = 0xFF) { return; }
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
Page::LineResult DefaultLine(Page::LineFunction func);
Page::LineResult TextLine(Page::LineFunction func, const char* pstring);
Page::LineResult SingleCombiLine(
  Page::LineFunction func, 
  Combiline& combiline, uint8_t len, uint8_t extra_padding, bool right_align
);
Page::LineResult DoubleCombiline(
  Page::LineFunction func, uint8_t field, 
  Combiline& combiline_1, uint8_t len_1, uint8_t extra_padding_1, bool right_align_1, 
  Combiline& combiline_2, uint8_t len_2, uint8_t extra_padding_2, bool right_align_2 
);

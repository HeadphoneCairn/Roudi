#pragma once

#include "Data.h"
#include "Screen.h"

// --- Page ---

class Page
{
public:
  Page();
  // SetNumberOfLines() must be called in the OnStart()!
  // TODO: simplify or remove???
protected:
  void SetNumberOfLines(uint8_t number_of_lines, uint8_t selected_line = 0xFF, uint8_t selected_field = 0xFF, uint8_t first_line = 0xFF); 
  uint8_t GetSelectedLine();
  uint8_t GetSelectedField();
  uint8_t GetFirstLine();
public:
  void Start(uint8_t parameter = 0xFF); // Call this when you want to first display the page
  void Stop(); // Call this when you want to exit the page
  void Timeout(); // Will be called by the main routine when no buttons have been pushed for a certain time
  void Redraw();  // Will be called by the main routine when the page needs redrawing even if no button has been pushed
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
  virtual void OnStop() { return; }
  virtual void OnTimeout() { return; }
  enum UpDownAction { UP, DOWN };
  virtual bool OnUpDown(UpDownAction action); // returns whether to redraw screen
  virtual void Draw(uint8_t from = 0, uint8_t to = 7);
private:
  uint8_t m_number_of_lines;
  uint8_t m_first_line; // the first displayed line
  uint8_t m_selected_line; // the selected line: 0..m_number_of_lines-1
  uint8_t m_selected_field; // the selected field on the line: 0..
};


// --- Line help functions ---

// Returns the string representation of i_value, + the number of values possible in o_max_value.
typedef const char* (*ValueFunction)(uint8_t i_value, uint8_t& o_number_of_values);

const char* GetPTable(uint8_t i_value, uint8_t& o_number_of_values, const char *const * ptable, uint8_t ptable_size);

#define PTABLE_GETTER(function_name, ptab_name)                                     \
  const char* function_name(uint8_t i_value, uint8_t& o_number_of_values) {         \                                                   
    return GetPTable(i_value, o_number_of_values, ptab_name, ptab_name##_size);     \
  }

// This string can be used if the value function needs to return a string for an unknown value:
PSTRINGREF(PSTR_unknown_value);

Page::LineResult DefaultLine(Page::LineFunction func);
Page::LineResult TextLine(Page::LineFunction func, const char* pstring);
Page::LineResult SingleLine(
  Page::LineFunction func,
  const char* name,
  uint8_t& value, 
  ValueFunction value_function
);

Page::LineResult DoubleLine(
  Page::LineFunction func,
  uint8_t field, 
  const char* name,
  uint8_t value_width, 
  uint8_t& left_value, 
  ValueFunction left_function,
  uint8_t& right_value, 
  ValueFunction right_function
);


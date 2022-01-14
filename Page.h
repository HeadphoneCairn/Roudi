#pragma once

#include "Screen.h"

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


#define TypeCString  0b00000001 // The key is a ptr to a string
#define TypePString  0b00000010 // The key is a ptr to a PROGMEM string
#define TypePTable   0b00010000 // The values is a ptr to a PROGMEM table
#define TypeFunction 0b00100000 // The values is a function ptr of type CombiLineFunction 
#define TypeRAlign   0b10000000 // Align value to the right

struct NewParsPars {
  uint8_t types;
  const void* name;
  uint8_t number_of_values;
  const void* values;
};
typedef const char* (*NewCombiLineFunction) (uint8_t);
typedef void (*NewCombilineParameters)(NewParsPars& pars);

class NewCombiline
{
public:
  NewCombiline();
  void Init(NewCombilineParameters par_function, uint8_t* selected_value);
  void GetText(char* text, uint8_t text_len, Screen::Inversion& inversion, uint8_t start, uint8_t len, uint8_t extra_padding=0);
  Screen::Inversion GetInversion();
  uint8_t* GetSelectedValue();
  bool OnLeft();
  bool OnRight();
public:
  static void UnitTest();
private:
  NewCombilineParameters m_par_function;
  uint8_t* m_selected_value;
};

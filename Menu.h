#pragma once

#include "Screen.h"

class Menu
{
public:
  Menu();
  void SetNumberOfLines(uint8_t number_of_lines, uint8_t selected_line = 0, uint8_t first_line = 0xFF); // Must be called before first call to Start()!
//  uint8_t GetSelectedLine();
//  uint8_t GetFirstLine();
public:
  void Start(); // Call this when you want to first display the menu
  void Stop(); // Call this when you want to exit the menu
  void Up();
  void Down();
  void Left();
  void Right();
protected:// to implement
  // Number of virtual functions needs to be kept as small as possible since each function
  // requires an extra 2 bytes of dynamic memory, for each subclass! 
  virtual const char* GetTitle() = 0; // Returned string must be alive for as long the menu is active
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) = 0; // Returned string must be alive until next GetLine call. 
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) { return; }
  enum UpDownAction { UP, DOWN };
  virtual bool OnUpDown(UpDownAction action); // returns whether to redraw screen
  enum LineAction { LEFT, RIGHT, SELECTED };
  virtual bool OnLine(LineAction action, uint8_t line) { return false; } // returns whether to redraw the line
private:
  void Draw(uint8_t from = 0, uint8_t to = 7);
private:
  uint8_t m_number_of_lines;
  uint8_t m_first_line; // the first displayed line
  uint8_t m_selected_line; // the selected line: 0..m_number_of_lines-1
};


#define gMenuBufferSize 45
extern Menu* g_menu;


#define TypeCString  0b00000001 // The key is a ptr to a string
#define TypePString  0b00000010 // The key is a ptr to a PROGMEM string
#define TypePTable   0b00010000 // The values is a ptr to a PROGMEM table
#define TypeFunction 0b00100000 // The values is a function ptr of type CombiLineFunction 
#define TypeRAlign   0b10000000 // Align value to the right

struct ParsPars {
  uint8_t types;
  const void* name;
  uint8_t number_of_values;
  const void* values;
};
typedef const char* (*CombiLineFunction) (uint8_t);
typedef void (*CombilineParameters)(ParsPars& pars);

class Combiline
{
public:
  Combiline();
  void Init(CombilineParameters par_function, uint8_t* selected_value);
  const char* GetText();
  Screen::Inversion GetInversion();
  uint8_t* GetSelectedValue();
  bool OnLeft();
  bool OnRight();
private:
  CombilineParameters m_par_function;
  uint8_t* m_selected_value;
};

#pragma once

#include "Screen.h"

#define TypeCString  0b00000001 // The key is a ptr to a string
#define TypePString  0b00000010 // The key is a ptr to a PROGMEM string
#define TypePTable   0b00010000 // The values is a ptr to a PROGMEM table
#define TypeFunction 0b00100000 // The values is a function ptr of type CombiLineFunction 

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
  void GetText(char* text, uint8_t text_len, Screen::Inversion& inversion, uint8_t start, uint8_t len, uint8_t extra_padding=0, bool right_align=false);
  Screen::Inversion GetInversion();
  uint8_t GetSelectedValue();
  bool OnLeft();
  bool OnRight();
public:
  static void UnitTest();
private:
  CombilineParameters m_par_function;
  uint8_t* m_selected_value;
};

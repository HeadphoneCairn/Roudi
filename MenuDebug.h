#pragma once

#include "Menu.h"

class MenuDebug: public Menu
{
public:
  MenuDebug();
protected:
  virtual const char* GetTitle() override;
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
};



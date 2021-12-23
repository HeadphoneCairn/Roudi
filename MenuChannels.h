#pragma once

#include "Menu.h"

class MenuChannels: public Menu
{
public:
  MenuChannels();
protected:
  virtual const char* GetTitle() override;
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
  virtual bool OnLine(LineAction action, uint8_t line) override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
};



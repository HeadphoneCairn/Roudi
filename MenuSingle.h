#pragma once

#include "Menu.h"

class MenuSingle: public Menu
{
public:
  MenuSingle();
protected:
  virtual const char* GetTitle() override;
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
  virtual bool OnLine(LineAction action, uint8_t line) override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
private:
  uint8_t m_number_of_channels;
private:
  void SetMidiConfiguration(uint8_t selected_line);
};



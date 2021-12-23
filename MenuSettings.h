#pragma once

#include "Data.h"
#include "Menu.h"

class MenuSettings: public Menu
{
public:
  MenuSettings(); 
protected:
  virtual const char* GetTitle() override;
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
  virtual bool OnLine(LineAction action, uint8_t line) override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
private:
  bool ShowChannelMenu();
  bool Undo();
private:
  enum { m_number_of_combilines = 8};
  Combiline m_lines[m_number_of_combilines];
  struct SettingsValues m_values;
};
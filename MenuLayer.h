#pragma once

#include "Data.h"
#include "Menu.h"


class MenuLayer: public Menu
{
public:
  MenuLayer(); 
protected:
  virtual const char* GetTitle() override;
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
  //  virtual void OnLineSelection(uint8_t line) override { return; }
  virtual bool OnLine(LineAction action, uint8_t line) override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
private:
  enum { m_number_of_combilines = 4};
  Combiline m_lines[m_number_of_combilines];
private:
  bool ActualOnLine(LineAction action, uint8_t line);
  void SetMidiConfiguration();
};

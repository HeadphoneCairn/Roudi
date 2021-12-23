#pragma once

#include "Data.h"
#include "Menu.h"


class MenuSplit: public Menu
{
public:
  MenuSplit(); 
protected:
  virtual const char* GetTitle() override;
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
  virtual bool OnLine(LineAction action, uint8_t line) override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
private:
  enum { m_number_of_combilines = 5};
  Combiline m_lines[m_number_of_combilines];
  uint8_t m_split_delta_from_settings;
private:
  bool ChangeSplitNote(uint8_t& note_value, bool left);
  bool ActualOnLine(LineAction action, uint8_t line);
  void SetMidiConfiguration();
};

#pragma once

#include "Data.h"
#include "Page.h"


class PageMulti: public Page
{
public:
  PageMulti(); 
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
private:
  uint8_t m_split_delta_from_settings;
private:
  bool ChangeSplitNote(uint8_t& note_value, bool left);
  void SetMidiConfiguration();
};

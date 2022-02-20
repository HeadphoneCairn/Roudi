#pragma once

#include "Data.h"
#include "Page.h"

class PageMonitorSettings: public Page
{
public:
  PageMonitorSettings(); 
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
private:
  LineResult LineFilter(LineFunction func, const char* name, uint8_t& value);
  LineResult LineAllFilters(LineFunction func, FilterSettingsValues& filters);
private:
  Combiline m_ui_inout;
  MidiMonSettingsValues m_settings;
private:
  static uint8_t m_selected_line;
  static uint8_t m_first_line;
};

#pragma once

#include "Data.h"
#include "Page.h"

class PageSettings: public Page
{
public:
  PageSettings(); 
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
  virtual void OnTimeout() override;
private:
  LineResult ActualLine(LineFunction func, uint8_t line, uint8_t field);
  void SaveIfModified();
private:
  void SetMidiConfiguration();
private:
  static uint8_t m_selected_line;
  static uint8_t m_first_line;
};
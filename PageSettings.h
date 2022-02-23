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
private:
  LineResult LineInputChannel(LineFunction func);
  LineResult LineFilter(LineFunction func, const char* name, uint8_t& value);
  LineResult LineMessageFilter(LineFunction func);
private:
  static uint8_t m_selected_line;
  static uint8_t m_first_line;
};
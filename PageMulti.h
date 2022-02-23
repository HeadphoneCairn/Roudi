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
  virtual void OnStart(uint8_t which_multi = 0xFF) override;
  virtual void OnStop() override;
  virtual void OnTimeout() override;
private:
  uint8_t m_which; // number of this multi, 0 based 
  MultiValues m_values;
private:
  LineResult ActualLine(LineFunction func, uint8_t line, uint8_t field);
  void SaveIfModified();
  void SaveAs();
  void New();
private:
  void SetMidiConfiguration();
};

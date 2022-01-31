#pragma once

#include "Page.h"

class PageSingle: public Page
{
public:
  PageSingle();
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
  virtual void OnTimeout() override;
private:
  void SaveIfModified();
  void SetMidiConfiguration(uint8_t selected_line);
};



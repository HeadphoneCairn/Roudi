#pragma once

#include "Page.h"

class PageChannels: public Page
{
public:
  PageChannels();
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart() override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
};



#if 0 // No longer needed

#pragma once

#include "Page.h"

class PageChannels: public Page
{
public:
  PageChannels();
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
};


#endif
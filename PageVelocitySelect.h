#pragma once

#include "Page.h"

class PageVelocitySelect: public Page
{
public:
  PageVelocitySelect();
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t which_curve) override;
};



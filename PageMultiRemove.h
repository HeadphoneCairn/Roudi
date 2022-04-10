#pragma once

#include "PageName.h"
#include "Screen.h"

class PageMultiRemove: public Page
{
public:
  PageMultiRemove();
protected:
  virtual const char* GetTitle() override;
  virtual void OnStart(uint8_t which_multi) override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
private:
  uint8_t m_which_multi;
};



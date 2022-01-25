#pragma once

#include "PageName.h"

class PageNameMulti: public PageName
{
public:
  PageNameMulti();
protected:
  virtual const char* GetTitle() override;
  virtual void OnStart(uint8_t multi) override;
  virtual bool OnUpDown(UpDownAction action) override;
};


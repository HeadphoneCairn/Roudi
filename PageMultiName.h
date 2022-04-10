#pragma once

#include "PageName.h"

class PageMultiName: public PageName
{
public:
  PageMultiName();
protected:
  virtual const char* GetTitle() override;
  virtual void OnStart(uint8_t which_multi) override;
  virtual bool OnUpDown(UpDownAction action) override;
private:
  uint8_t m_which_multi;
};



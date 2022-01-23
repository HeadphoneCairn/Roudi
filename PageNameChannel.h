#pragma once

#include "PageName.h"

class PageNameChannel: public PageName
{
public:
  PageNameChannel();
protected:
  virtual const char* GetTitle() override;
  virtual void OnStart(uint8_t channel_value) override;
  virtual bool OnUpDown(UpDownAction action) override;
};



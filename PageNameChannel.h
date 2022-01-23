#pragma once

#include "PageName.h"

class PageNameChannel: public PageName
{
public:
  PageNameChannel(); //(uint8_t channel_value);
protected:
  virtual const char* GetTitle() override;
  virtual void OnStart() override;
  virtual bool OnUpDown(UpDownAction action) override;
};



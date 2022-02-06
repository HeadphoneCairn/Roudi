#pragma once

#include "Page.h"

struct midi_event_t;

class PageMonitor: public Page
{
public:
  PageMonitor();
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
private:
  LineResult LineDecode(const midi_event_t& event);
};



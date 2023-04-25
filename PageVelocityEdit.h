#pragma once

#include "Page.h"

class PageVelocityEdit: public Page
{
public:
  PageVelocityEdit();
  uint8_t m_velocity_of_last_note;
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual bool OnUpDown(UpDownAction action);
  virtual void OnStart(uint8_t which_map = 0xFF) override;
  virtual void OnStop() override;
  virtual void Draw(uint8_t from = 0, uint8_t to = 7) override;
private:
  uint8_t m_which; // number of this multi, 1..3, 0 not allowed, that one is fixed 
  uint8_t m_position;
  VelocityMap m_velocity_map;
};



#pragma once

#include "Page.h"

class PageVelocityEdit: public Page
{
public:
  PageVelocityEdit();
  uint8_t m_last_note_on;
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual bool OnUpDown(UpDownAction action);
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
  virtual void Draw(uint8_t from = 0, uint8_t to = 7) override;
private:
  void SetMidiConfiguration(uint8_t selected_line);
  uint8_t m_position;
};



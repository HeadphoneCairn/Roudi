#pragma once

#include "Data.h"
#include "Page.h"


class PageMulti: public Page
{
public:
  PageMulti();
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
private:
  uint8_t m_split_delta_from_settings;
private:
  void SetMidiConfiguration();
private:
  NewCombiline m_ui_channel_1, m_ui_octave_1;
  NewCombiline m_ui_pitchbend_1, m_ui_velocity_1;
  NewCombiline m_ui_channel_2, m_ui_octave_2;
  NewCombiline m_ui_pitchbend_2, m_ui_velocity_2;
  NewCombiline m_ui_mode, m_ui_split_note;

/*
01. Dreadbox    oct: +1 
    pb/cc: off  vel: 100   
05. 0123456789  oct: +1 
    pb/cc: on   vel: 0
split    split note: C4        of layer
*/

};

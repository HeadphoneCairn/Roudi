#pragma once

#include "Data.h"
#include "Page.h"

class PageMonitorSettings: public Page
{
public:
  PageMonitorSettings(); 
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
private:
  LineResult LineInputChannel(LineFunction func);
  bool ShowChannelMenu();
private:
  Combiline m_ui_input_channel;
  Combiline m_ui_velocity_curve; 
  Combiline m_ui_program_change; 
  Combiline m_ui_brightness;  
};
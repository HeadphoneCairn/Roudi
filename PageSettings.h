#pragma once

#include "Data.h"
#include "Page.h"

class PageSettings: public Page
{
public:
  PageSettings(); 
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart() override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
private:
  bool ShowChannelMenu();
private:
  NewCombiline m_ui_input_channel;  
  NewCombiline m_ui_output_channels;
  NewCombiline m_ui_velocity_curve; 
  NewCombiline m_ui_program_change; 
  NewCombiline m_ui_brightness;  

  struct SettingsValues m_values;
};
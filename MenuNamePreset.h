#if 0 // Due to the complexity of named presets, we are currently not using this menu

#pragma once

#include "Menu.h"
#include "Utils.h"

class MenuNamePreset: public Menu
{
public:
  MenuNamePreset(uint8_t preset_value);
protected:
  virtual const char* GetTitle() override;
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
  virtual bool OnUpDown(UpDownAction action) override;
  virtual bool OnLine(LineAction action, uint8_t line) override;
  virtual void OnStop(uint8_t selected_line, uint8_t first_line) override;
private:
  uint8_t GetMaxPositions();
  uint8_t GetPositionOfAccept();
  uint8_t GetPositionOfCancel();
private:
  NameEditor m_name;
  uint8_t m_preset_value;
  uint8_t m_position;
};

#endif
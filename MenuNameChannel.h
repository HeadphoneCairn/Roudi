#pragma once

#include "Menu.h"
#include "Utils.h"

class MenuNameChannel: public Menu
{
public:
  MenuNameChannel(uint8_t channel_value);
protected:
  virtual const char* GetTitle() override;
  virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
  virtual bool OnUpDown(UpDownAction action) override;
  virtual bool OnLine(LineAction action, uint8_t line) override;
private:
  uint8_t GetMaxPositions();
  uint8_t GetPositionOfChannel();
  uint8_t GetPositionOfAccept();
  uint8_t GetPositionOfCancel();
private:
  NameEditor m_name;
  uint8_t m_channel_value;
  uint8_t m_position;
};



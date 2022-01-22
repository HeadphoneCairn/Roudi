#pragma once

#include "Page.h"
#include "Utils.h"

class PageNameChannel: public Page
{
public:
  PageNameChannel(); //(uint8_t channel_value);
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;

  //virtual void GetLine(uint8_t line, const char*& text, Screen::Inversion& inversion) override;
  virtual bool OnUpDown(UpDownAction action) override;
  //virtual bool OnLine(LineAction action, uint8_t line) override;
  virtual void OnStart() override;
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



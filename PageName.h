#pragma once

#include "Page.h"
#include "Utils.h"

class PageName: public Page
{
public:
  PageName(); //(uint8_t channel_value);
protected:
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual const char* GetTitle() = 0;
  virtual bool OnUpDown(UpDownAction action) override;
  virtual void OnStart() override;
protected:
  enum Result { NONE, ACCEPT, REJECT };
  void SetValues(const char* name, uint8_t number, const char* number_prefix); // Both name and number_prefix are copied by this function
  Result GetResult();
  void GetNameAndValue(char* name, uint8_t& number); // copies current name into name string, and sets the current number to number
private:
  uint8_t GetMaxPositions();
  uint8_t GetPositionOfChannel();
  uint8_t GetPositionOfAccept();
  uint8_t GetPositionOfCancel();
private:
  NameEditor m_name;
  Result m_result;
  uint8_t m_number_prefix_len;
  uint8_t m_number;
  uint8_t m_position;
};



#pragma once

#include "Page.h"
#include "Utils.h"

class PageName: public Page
{
public:
  PageName();
protected:
  virtual const char* GetTitle() = 0;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual bool OnUpDown(UpDownAction action) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
protected:
  enum Result { NONE, ACCEPT, REJECT };
  void SetValues(const char* name, uint8_t number, uint8_t number_max, const char* number_prefix);
  Result GetResult();
  void GetNameAndValue(char* name, uint8_t& number); // copies current name into name string, and sets the current number to number
private:
  uint8_t GetMaxPositions();
  uint8_t GetPositionOfNumber();
  uint8_t GetPositionOfAccept();
  uint8_t GetPositionOfCancel();
private:
  uint8_t m_number;
  uint8_t m_number_max;
  uint8_t m_number_prefix_len;
private:
  NameEditor m_name;
  Result m_result;
  uint8_t m_position;
};



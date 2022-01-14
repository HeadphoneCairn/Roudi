#pragma once

#include <Arduino.h> // for uint8_t

// --- Name editor ---
class NameEditor
{
public:
  NameEditor();
  void Init(char* name_buffer, const char* name);
  void CopyName(char* name);
  uint8_t GetLength();
  uint8_t GetPositions();
  bool UpDown(uint8_t position, bool up);
private:
  char* m_name_buffer; // points to memory maintained by caller
};

// --- Bitfield ---
// Why use the standard C++ bitfield, when you can write your own?
class Bitfield
{
public:
  Bitfield();
  void SetAll(bool value);
  void Set(uint8_t position, bool value);
  bool Get(uint8_t position);
  bool HasAtLeastOne(bool value);
//  void Print();
private:
  void GetInternalPosition(uint8_t position, uint8_t& byte, uint8_t& byte_value);
  uint8_t m_values[16]; // 16 * 8 = 128 bits
};

// --- String functions ---
// Pad string 'text' with 'len' * 'c' characters
void PadRight(char* text, uint8_t len, char c = ' '); 

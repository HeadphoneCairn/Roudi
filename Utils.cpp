#include "Utils.h"

#include "Debug.h"
#include "Data.h"

#include <string.h>
#include <stdio.h>

//==============================================================================
//
//                           N A M E   E D I T O R
//
//==============================================================================

const uint8_t MaxLength = MaxNameLength; 

#define UNDERSCORE '_' // represents and empty postion that can be filled, lowest possible value

static const char PSTR_complete_char_set[] PROGMEM = {
  UNDERSCORE, UNDERSCORE,
  'a', 'z',
  'A', 'Z',
  '0', '9',
  '!', '#',
  '%', '/',
  ':', '@',
  '[', '^',
  ' ', ' ', 
  '\0' };

static const char PSTR_uppercase_char_set[] PROGMEM = {
  UNDERSCORE, UNDERSCORE, 
  'A', 'Z',
  '0', '9',
  '!', '#',
  '%', '/',
  ':', '@',
  '[', '^',
  ' ', ' ', 
  '\0' };

namespace {

  char GetNextCharacter(char c, const char* chars)

  // Function to iterate through a list of possible char values
  // Complex code to save data memory :-(
  {
    if (c == chars[strlen(chars) - 1])
      return c; // no further values;
    for (uint8_t i = 0; i < strlen(chars)/2; i++) {
      const char from = chars[i*2];
      const char to = chars[i*2 + 1]; 
      if (c == to)
        return chars[i*2 + 2];
      if (c >= from && c < to)
        return c + 1;
    }
    return chars[strlen(chars) - 1]; // just in case c was not found in chars
  }

  char GetPreviousCharacter(char c, const char* chars)
  // Function to iterate through a list of possible char values
  // Complex code to save data memory :-(
  {
    if (c == chars[0])
      return c; // no further values;
    for (uint8_t i = 0; i < strlen(chars)/2; i++) {
      const char from = chars[i*2];
      const char to = chars[i*2 + 1]; 
      if (c == from)
        return chars[i*2 - 1];
      if (c > from && c <= to)
        return c - 1;
    }
    return chars[strlen(chars) - 1]; // just in case c was not found in chars
  }

}


NameEditor::NameEditor(bool complete_char_set):
  m_name_buffer(nullptr),
  m_char_set(complete_char_set ? PSTR_complete_char_set : PSTR_uppercase_char_set)
{
}

void NameEditor::Init(char* name_buffer, const char* name)
// The name_buffer should be allocated by caller and should have enough place for MaxLength characters
// This constructor will put the name in the name_buffer suffixed with UNDERSCOREs
{
  m_name_buffer = name_buffer;
  strncpy(m_name_buffer, name, MaxLength);
  for (uint8_t i = strlen(name); i < MaxLength; ++i)
    m_name_buffer[i] = UNDERSCORE;
}

void NameEditor::CopyName(char* name)
{
  strncpy(name, m_name_buffer, GetLength());
  name[GetLength()] = 0;
}

uint8_t NameEditor::GetLength()
// Returns the length of the name
{
  for (uint8_t i = 0; i < MaxLength; i++) {
    if (m_name_buffer[i] == UNDERSCORE)
      return i;
  }
  return MaxLength;
}

uint8_t NameEditor::GetPositions()
// Returns the number of positions of the name that are selectable with left/right buttons.
{
  uint8_t num_positions = GetLength() + 1;
  return num_positions > MaxLength ? MaxLength : num_positions;
}

bool NameEditor::UpDown(uint8_t position, bool up)
// Decrements or increments the character a position position in the m_name_buffer
{
  if (position >= GetPositions()) 
    return false; // Just te be sure
  char old_character = m_name_buffer[position];
  bool use_underscore = (position + 1 == GetPositions()) || 
                        (position + 2 == GetPositions() && GetLength() != MaxLength);
  const char* char_set = GetPString(m_char_set) + (use_underscore ? 0 : 2);
  if (up) {
    m_name_buffer[position] = GetNextCharacter(old_character, char_set);
  } else {
    m_name_buffer[position] = GetPreviousCharacter(old_character, char_set);
  }
  return m_name_buffer[position] != old_character;
}


//==============================================================================
//
//                             B I T F I E L D
//
//==============================================================================


Bitfield::Bitfield()
{
  SetAll(false);
}

void Bitfield::SetAll(bool value)
{
  memset(m_values, value ? 0xFF : 0x00, sizeof(m_values));
}

void Bitfield::Set(uint8_t position, bool value)
{
  uint8_t byte, byte_value;
  GetInternalPosition(position, byte, byte_value);
  if (value) {
    m_values[byte] |= byte_value;
  } else {
    m_values[byte] &= ~byte_value;
  }
}

bool Bitfield::Get(uint8_t position)
{
  uint8_t byte, byte_value;
  GetInternalPosition(position, byte, byte_value);
  return m_values[byte] & byte_value;
}

bool Bitfield::HasAtLeastOne(bool value)
{
  uint8_t to_find = value ? 0x00 : 0xff;
  for (uint8_t i = 0; i < sizeof(m_values); i++) {
    if (m_values[i] != to_find)
      return true;
  }
  return false;
}

//void BitfieldPrint()
//{
//  for (uint8_t i = 0; i < sizeof(m_values) * 8; i++)
//    std::cout << Get(i);
//  std::cout << std::endl;
//}

void Bitfield::GetInternalPosition(uint8_t position, uint8_t& byte, uint8_t& byte_value) 
{
  if (position >= (sizeof(m_values) << 3)) // on overflow, use last bit
    position = (sizeof(m_values) << 3) - 1;
  byte = position >> 3;
  const uint8_t bit = 7 - (position - (byte << 3));
  byte_value = 1 << bit;
}

//==============================================================================
//
//                     S T R I N G   F U N C T I O N S
//
//==============================================================================

void PadRight(char* text, uint8_t len, char c)
{
  text = text + strlen(text);
  memset(text, c, len);
  text[len]=0;
}

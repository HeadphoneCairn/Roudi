/*
 * Roudi, a ROUter for miDI messages for Blokas Midiboy
 * Copyright (C) 2019-2023 Headphone Cairn Software
 *
 * This program is free software; you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation; version 2 of the License.
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with
 * this program; if not, write to the Free Software Foundation, Inc., 
 * 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */


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

PSTRING(PSTR_complete_char_set,  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-+,.:;!?()[]&@#|\"'^ ");

NameEditor::NameEditor(bool complete_char_set):
  m_name_buffer(nullptr)
{
  if (complete_char_set) {
    m_char_set = PSTR_complete_char_set;
    m_start_char = 'a';  
  } else { // We do not allow lowercase characters.
    m_char_set = PSTR_complete_char_set + 26;
    m_start_char = 'A';  
  }
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
  if (up) {
    m_name_buffer[position] = GetNextCharacter(old_character, use_underscore);
  } else {
    m_name_buffer[position] = GetPreviousCharacter(old_character, use_underscore);
  }
  return m_name_buffer[position] != old_character;
}

char NameEditor::GetNextCharacter(char c, bool use_underscore)
{
  // Prepend character set with underscore if needed
  if (use_underscore && c == UNDERSCORE)
    return m_start_char;
  // Search character c in the character set
  const char* p = strchr_P(m_char_set, c);
  // If c was not found, return the first character of the character set as a fallback
  if (p == nullptr)
    return m_start_char;
  // Get character after c in the character set
  p++;
  char out;
  strncpy_P(&out, p, 1);
  return out ? out : c; // if out == \0, we return c
}

char NameEditor::GetPreviousCharacter(char c, bool use_underscore)
{
  // Prepend character set with underscore if needed
  if (use_underscore && (c == m_start_char || c == UNDERSCORE))
    return UNDERSCORE;
  // Search character c in the character set
  const char* p = strchr_P(m_char_set, c);
  // If c was not found, return the first character of the character set as a fallback
  if (p == nullptr)
    return m_start_char;
  // If c is the first character of the character set
  if (p == m_char_set)
    return c;
  // Get character before c in the character set
  p--;
  char out;
  strncpy_P(&out, p, 1);
  return out;
}

#if 0
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
#endif


//==============================================================================
//
//                             C O U N T E R S
//
//==============================================================================

Counters::Counters()
{
  SetAll(0);
}

void Counters::SetAll(uint8_t value) {
  if (value > 15) value = 15;
  memset(m_values, (value << 4) + value, sizeof(m_values));
}

void Counters::Set(uint8_t position, uint8_t value) {
  position = RestrictPosition(position);
  if (value > 15) value = 15;
  RawSet(position, value);
}

uint8_t Counters::Get(uint8_t position) const {
  position = RestrictPosition(position);
  return RawGet(position);
}

void Counters::Increment(uint8_t position) {
  position = RestrictPosition(position);
  uint8_t value = RawGet(position);
  if (value < 15)
    RawSet(position, value + 1);
}

void Counters::Decrement(uint8_t position) {
  position = RestrictPosition(position);
  uint8_t value = RawGet(position);
  if (value > 0)
    RawSet(position, value - 1);
}

//void Counters::Print() const {
//  for (uint8_t i = 0; i < sizeof(m_values) * 2; i++)
//    std::cout << (int) Get(i) << ", ";
//  std::cout << std::endl;
//}

uint8_t Counters::RestrictPosition(uint8_t position) const {
  if (position >= sizeof(m_values) * 2)
    return sizeof(m_values) * 2 - 1;
  else
    return position;
}

void Counters::RawSet(uint8_t position, uint8_t value) {
  uint8_t byte = position >> 1;
  if (position & 0x1)
    m_values[byte] = m_values[byte] & 0xF0 | value;
  else
    m_values[byte] = m_values[byte] & 0x0F | (value << 4);
}

uint8_t Counters::RawGet(uint8_t position) const {
  uint8_t byte = position >> 1;
  if (position & 0x1)
    return m_values[byte] & 0x0F;
  else
    return (m_values[byte] & 0xF0) >> 4;
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

char* AddEllipsis(char* text)
{
  PadRight(text, Screen::buffer_len - 3 - strlen(text));
  PadRight(text, 3, '.');
  return(text);
}

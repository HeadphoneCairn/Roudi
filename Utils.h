#pragma once

#include <Arduino.h> // for uint8_t

// --- Name editor ---
class NameEditor
{
public:
  NameEditor(bool complete_char_set = true);
  void Init(char* name_buffer, const char* name);
  void CopyName(char* name);
  uint8_t GetLength();
  uint8_t GetPositions();
  bool UpDown(uint8_t position, bool up);
private:
  const char* m_char_set; // points to characters set
  char m_start_char; // contains first character of character set
  char* m_name_buffer; // points to memory maintained by caller
private:
  char GetNextCharacter(char c, bool use_underscore);
  char GetPreviousCharacter(char c, bool use_underscore);
};

#if 0
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
#endif

// --- Counters ---
class Counters
// Stores an array of 4-bit counters (so possible values 0..15).
{
public:
  Counters();
  void SetAll(uint8_t value);
  void Set(uint8_t position, uint8_t value);
  uint8_t Get(uint8_t position) const;
  void Increment(uint8_t position);
  void Decrement(uint8_t position);
  //void Print() const; {
private:
  uint8_t RestrictPosition(uint8_t position) const;
  void RawSet(uint8_t position, uint8_t value);
  uint8_t RawGet(uint8_t position) const;
  uint8_t m_values[64]; // so 128 counters
};


// --- String functions ---
// Pad string 'text' with 'len' * 'c' characters
void PadRight(char* text, uint8_t len, char c = ' '); 

// Resize the string to the width of the screen by padding with spaces and adding ... at the end 
char* AddEllipsis(char* text);

// --- Simple Circular Buffer ---
// Very simple circular buffer.
// Note that if IndexType is uint8_t, N should be <= 128
template <typename T, typename IndexType, const IndexType N>
class TCircularBuffer
{
  public:
    TCircularBuffer(): m_first(0) {
      // We consider the buffer as always full.
      // But we memset as to mark an element as kind of empty.
      memset(&m_buffer, 0, sizeof(m_buffer));
    }
    void Push(const T& element) {
      // Removes first element from buffer and inserts a last element
      m_buffer[m_first] = element;
      m_first = (m_first+1) % N;
    }
    const T& operator[](IndexType i) const { 
      return m_buffer[(m_first+i) % N];
      }
  private:
    IndexType m_first;
    T m_buffer[N];
};

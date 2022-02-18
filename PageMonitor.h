#pragma once

#include "Page.h"
#include <midi_serialization.h>


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

struct midi_msg_t {
  midi_event_t event;
  uint8_t input:1;
};

const uint8_t g_num_messages = 64;




class PageMonitor: public Page
{
public:
  PageMonitor();
public: // For the listening interface
  TCircularBuffer<midi_msg_t, uint8_t, g_num_messages> g_messages;
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
private:
  LineResult LineDecode(const midi_msg_t& event);
};



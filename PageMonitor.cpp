#include "PageMonitor.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"

#include <midi_serialization.h>

PSTRING(PSTR_page_monitor, " MIDI MONITOR "); 
/*
Options:
  Show: in, out, in+out
  Show: only from our input channel and its output channels
  Filter: sysex, ...
*/

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

namespace {

  const uint8_t g_num_messages = 64;
  TCircularBuffer<midi_event_t, uint8_t, g_num_messages> g_messages;

  void Listen(const midi_event_t& event)
  {
    g_messages.Push(event);
  }

}

PageMonitor::PageMonitor(): Page()
{
}

void PageMonitor::OnStart(uint8_t)
{
  memset(&g_messages, 0, sizeof(g_messages));
  SetNumberOfLines(g_num_messages, g_num_messages - 1);

  // Attach listener
  MidiProcessing::SetMidiInListener(Listen);
}

void PageMonitor::OnStop()
{
  // Detach listener
  MidiProcessing::SetMidiInListener(nullptr);
}

const char* PageMonitor::GetTitle()
{
  return GetPString(PSTR_page_monitor);
}

Page::LineResult PageMonitor::Line(LineFunction func, uint8_t line, uint8_t field)
{
  if (func == GET_TEXT)
    return LineDecode(g_messages[line]);
  else
    return { 1, nullptr, Screen::inversion_none, false };
}


Page::LineResult PageMonitor::LineDecode(const midi_event_t& event)
{
  char* text = Screen::buffer;
  if (event.m_event == 0)
    text[0] = 0;
  else {

    if (event.m_event == 0x8 || event.m_event == 0x9) {
      const uint8_t channel = event.m_data[0] & 0x0f;
      const uint8_t note = event.m_data[1];
      const uint8_t velocity = event.m_data[2];
      sprintf(text, "ch%02d: note %d %s, v=%d", 
        channel + 1, 
        note,
        event.m_event==0x8 ? "OFF" : "ON",
        velocity
        );
    } else
      strcpy(text, "unknown message");

  }

  return { 1, text, Screen::inversion_all, false };
}


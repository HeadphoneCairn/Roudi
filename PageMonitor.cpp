#include "PageMonitor.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Roudi.h"

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

  struct midi_msg_t {
    midi_event_t event;
    uint8_t input:1;
  };

  const uint8_t g_num_messages = 64;
  TCircularBuffer<midi_msg_t, uint8_t, g_num_messages> g_messages;

  void ListenIn(const midi_event_t& event)
  {
    g_messages.Push({event, 1});
    SetRedrawNext();
  }
  void ListenOut(const midi_event_t& event)
  {
    g_messages.Push({event, 0});
    SetRedrawNext();
  }

}

PageMonitor::PageMonitor(): Page()
{
}

void PageMonitor::OnStart(uint8_t)
{
  memset(&g_messages, 0, sizeof(g_messages));
  SetNumberOfLines(g_num_messages, g_num_messages - 1);

  // Attach listeners
  MidiProcessing::SetMidiInListener(ListenIn);
  MidiProcessing::SetMidiOutListener(ListenOut);
}

void PageMonitor::OnStop()
{
  // Detach listeners
  MidiProcessing::SetMidiInListener(nullptr);
  MidiProcessing::SetMidiOutListener(nullptr);
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



PSTRING(PSTR_mm_channel,           "%c%02d ");
PSTRING(PSTR_mm_no_channel,        "%c-- ");
PSTRING(PSTR_mm_unknown,           "unknown message");

PSTRING(PSTR_mm_note_off,          "note off %s, vel %d");   // 0x8. 
PSTRING(PSTR_mm_note_on,           "note on %s, vel %d");    // 0x9.
PSTRING(PSTR_mm_key_pressure,      "key pressure %s %d");    // 0xa.
PSTRING(PSTR_mm_cc,                "CC#%d %d");              // 0xb.
PSTRING(PSTR_mm_program_change,    "program change %d");     // 0xc.
PSTRING(PSTR_mm_channel_pressure,  "channel pressure %d");   // 0xd.
PSTRING(PSTR_mm_pitch_bend,        "pitch bend %lu");        // 0xe.

PSTRING(PSTR_mm_sysex_start,       "sysex start");           // 0xf0
PSTRING(PSTR_mm_time_code,         "time code %d");          // 0xf1           
PSTRING(PSTR_mm_song_position,     "song position %lu");     // 0xf2
PSTRING(PSTR_mm_song_select,       "song select %d");        // 0xf3
PSTRING(PSTR_mm_tune_request,      "tune request");          // 0xf6
PSTRING(PSTR_mm_sysex_end,         "sysex end");             // 0xf7

PSTRING(PSTR_mm_timing_clock,      "timing clock");          // 0xf8
PSTRING(PSTR_mm_start,             "start");                 // 0xfa
PSTRING(PSTR_mm_continue,          "continue");              // 0xfb
PSTRING(PSTR_mm_stop,              "stop");                  // 0xfc
PSTRING(PSTR_mm_active_sensing,    "active sensing");        // 0xfe
PSTRING(PSTR_mm_system_reset,      "reset");                 // 0xff         

/*
TODO 
- snprintf! to prevent buffer overrun
x note instead of key number!!!  
- type of CC: mod/volume/bank select/...
x all F.. thingies
- sysex, how to show
- running status ???
- implemen settings and filters
- check MSB/LSB order of pitch bend and song position
*/

Page::LineResult PageMonitor::LineDecode(const midi_msg_t& msg)
{
  // --- Shortcut to the event ---
  const midi_event_t& e = msg.event;
  // --- The output text ---
  char* text = Screen::buffer;
  text[0] = 0;
  // --- Temporary storage for note name ---
  const uint8_t max_note_name_len = 4; // max note name string length = 4 => C#-1
  char note_name[max_note_name_len + 1];
  note_name[max_note_name_len] = 0;

  // --- Add the decode message to text (if there is a message) ---
  if (e.m_event != 0) {
    // Add (I)nput/(O)utput and channel
    char in_or_out = msg.input ? 'I' : 'O';
    if (e.m_event >= 0x8 && e.m_event <= 0xe) {
      const uint8_t channel = e.m_data[0] & 0x0f;
      text += sprintf(text, GetPString(PSTR_mm_channel), in_or_out, channel + 1);
    } else {
      text += sprintf(text, GetPString(PSTR_mm_no_channel), in_or_out);
    }
    // Add event text for messages that are channel specific
    if (e.m_event >= 0x8 && e.m_event <= 0xe) {
      switch (e.m_event)
      { 
        case 0x8:
          strncpy(note_name, GetNoteName(e.m_data[1]), max_note_name_len);
          sprintf(text, GetPString(PSTR_mm_note_off), note_name, e.m_data[2]);
          break;
        case 0x9:
          strncpy(note_name, GetNoteName(e.m_data[1]), max_note_name_len);
          sprintf(text, GetPString(PSTR_mm_note_on), note_name, e.m_data[2]);
          break;
        case 0xa:
          strncpy(note_name, GetNoteName(e.m_data[1]), max_note_name_len);
          sprintf(text, GetPString(PSTR_mm_key_pressure), note_name, e.m_data[2]);
          break;
        case 0xb:
          sprintf(text, GetPString(PSTR_mm_cc), e.m_data[1], e.m_data[2]);
          break;
        case 0xc:
          sprintf(text, GetPString(PSTR_mm_program_change), e.m_data[1]);
          break;
        case 0xd:
          sprintf(text, GetPString(PSTR_mm_channel_pressure), e.m_data[1]);
          break;
        case 0xe:
          sprintf(text, GetPString(PSTR_mm_pitch_bend), static_cast<unsigned long>(e.m_data[2]) * 128ul + static_cast<unsigned long>(e.m_data[1])); // LSB, MSB
          break;
        default:
          strcpy(text, GetPString(PSTR_mm_unknown));
          break;
      }
    }
    // Add event text for messages that are channel independent
    else if (e.m_event == 0xf) {
      switch (e.m_data[0]) {
        case 0xf0:
          sprintf(text, GetPString(PSTR_mm_sysex_start));
          break;
        case 0xf1:
          sprintf(text, GetPString(PSTR_mm_time_code), e.m_data[1]);
          break;
        case 0xf2:
          sprintf(text, GetPString(PSTR_mm_song_position), static_cast<unsigned long>(e.m_data[1]) * 128ul + static_cast<unsigned long>(e.m_data[2]));  // MSB, LSB
          break;
        case 0xf3:
          sprintf(text, GetPString(PSTR_mm_song_select), e.m_data[1]);
          break;
        case 0xf6:
          sprintf(text, GetPString(PSTR_mm_tune_request));
          break;
        case 0xf7:
          sprintf(text, GetPString(PSTR_mm_sysex_end));
          break;
        case 0xf8:
          sprintf(text, GetPString(PSTR_mm_timing_clock));
          break;
        case 0xfa:
          sprintf(text, GetPString(PSTR_mm_start));
          break;
        case 0xfb:
          sprintf(text, GetPString(PSTR_mm_continue));
          break;
        case 0xfc:
          sprintf(text, GetPString(PSTR_mm_stop));
          break;
        case 0xfe:
          sprintf(text, GetPString(PSTR_mm_active_sensing));
          break;
        case 0xff:
          sprintf(text, GetPString(PSTR_mm_system_reset));
          break;
        default:
          strcpy(text, GetPString(PSTR_mm_unknown));
          break;
      }
    }
    // Unknown message
    else {
      strcpy(text, GetPString(PSTR_mm_unknown));
    }
  }

  return { 1, Screen::buffer, Screen::inversion_all, false };
}


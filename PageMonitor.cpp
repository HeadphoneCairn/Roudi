#include "PageMonitor.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"
#include "Roudi.h"


PSTRING(PSTR_page_monitor, " MONITOR "); 


namespace {

  // Could possibly turn this into one function by using the data pointer 
  void ListenIn(const midi_event_t& event, void* data)
  {
    PageMonitor* page_monitor = static_cast<PageMonitor*>(data);
    page_monitor->g_messages.Push({event, 1});
    SetRedrawNext();
  }
  void ListenOut(const midi_event_t& event, void* data)
  {
    PageMonitor* page_monitor = static_cast<PageMonitor*>(data);
    page_monitor->g_messages.Push({event, 0});
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
  MidiProcessing::SetMidiInListener({ListenIn, this});  
  MidiProcessing::SetMidiOutListener({ListenOut, this});
}

void PageMonitor::OnStop()
{
  // Detach listeners
  MidiProcessing::SetMidiInListener({nullptr, nullptr});
  MidiProcessing::SetMidiOutListener({nullptr, nullptr});
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



// List of more or less standard uses of CCs.
// Max 10 characters "0123456789"

PSTRING(PSTR_cc_0,   "bank selec");
PSTRING(PSTR_cc_1,   "mod wheel" );
PSTRING(PSTR_cc_2,   "breath"    );
PSTRING(PSTR_cc_4,   "foot pedal");
PSTRING(PSTR_cc_5,   "portamento");
PSTRING(PSTR_cc_6,   "data entry");
PSTRING(PSTR_cc_7,   "volume"    );
PSTRING(PSTR_cc_8,   "balance"   );
PSTRING(PSTR_cc_10,  "pan"       );
PSTRING(PSTR_cc_11,  "expression");
PSTRING(PSTR_cc_12,  "effect 1"  );
PSTRING(PSTR_cc_13,  "effect 2"  );

PSTRING(PSTR_cc_64,  "sustain p.");
PSTRING(PSTR_cc_65,  "portamento");
PSTRING(PSTR_cc_66,  "sostenuto" );
PSTRING(PSTR_cc_67,  "soft pedal");
PSTRING(PSTR_cc_68,  "legato"    );
PSTRING(PSTR_cc_69,  "hold pedal");
PSTRING(PSTR_cc_70,  "sound ct1" );
PSTRING(PSTR_cc_71,  "resonance" );
PSTRING(PSTR_cc_72,  "release"   );
PSTRING(PSTR_cc_73,  "attack"    );
PSTRING(PSTR_cc_74,  "cutoff"    );
PSTRING(PSTR_cc_75,  "sound ct6" );
PSTRING(PSTR_cc_76,  "Sound ct7" );
PSTRING(PSTR_cc_77,  "Sound ct8" );
PSTRING(PSTR_cc_78,  "Sound ct9" );
PSTRING(PSTR_cc_79,  "Sound ct10");
PSTRING(PSTR_cc_80,  "decay"     );
PSTRING(PSTR_cc_91,  "reverb"    );
PSTRING(PSTR_cc_92,  "tremolo"   );
PSTRING(PSTR_cc_93,  "chorus"    );
PSTRING(PSTR_cc_94,  "detune"    );
PSTRING(PSTR_cc_95,  "phaser"    );
PSTRING(PSTR_cc_96,  "data +"    );
PSTRING(PSTR_cc_97,  "data -"    );

PSTRING(PSTR_cc_120, "sounds off");
PSTRING(PSTR_cc_121, "ctrlrs off");
PSTRING(PSTR_cc_122, "local"     );
PSTRING(PSTR_cc_123, "notes off" );
PSTRING(PSTR_cc_124, "omni off"  );
PSTRING(PSTR_cc_125, "omni on"   );
PSTRING(PSTR_cc_126, "mono"      );
PSTRING(PSTR_cc_127, "poly"      );

PSTRING(PSTR_cc_default, "-");

// Convert cc number to cc "standard" name
// This function uses almost 1,000 bytes of my 28,672 bytes of program storage space!
static const char* GetCCName(uint8_t cc)
{
  switch(cc) {
    case   0 + 32:
    case   0: return GetPString(PSTR_cc_0);
    case   1 + 32:
    case   1: return GetPString(PSTR_cc_1);
    case   2 + 32:
    case   2: return GetPString(PSTR_cc_2);
    case   4 + 32:
    case   4: return GetPString(PSTR_cc_4);
    case   5 + 32:
    case   5: return GetPString(PSTR_cc_5);
    case   6 + 32:
    case   6: return GetPString(PSTR_cc_6);
    case   7 + 32:
    case   7: return GetPString(PSTR_cc_7);
    case   8 + 32:
    case   8: return GetPString(PSTR_cc_8);
    case  10 + 32:
    case  10: return GetPString(PSTR_cc_10);
    case  11 + 32:
    case  11: return GetPString(PSTR_cc_11);
    case  12 + 32:
    case  12: return GetPString(PSTR_cc_12);
    case  13 + 32:
    case  13: return GetPString(PSTR_cc_13);
    case  64: return GetPString(PSTR_cc_64);
    case  65: return GetPString(PSTR_cc_65);
    case  66: return GetPString(PSTR_cc_66);
    case  67: return GetPString(PSTR_cc_67);
    case  68: return GetPString(PSTR_cc_68);
    case  69: return GetPString(PSTR_cc_69);
    case  70: return GetPString(PSTR_cc_70);
    case  71: return GetPString(PSTR_cc_71);
    case  72: return GetPString(PSTR_cc_72);
    case  73: return GetPString(PSTR_cc_73);
    case  74: return GetPString(PSTR_cc_74);
    case  75: return GetPString(PSTR_cc_75);
    case  76: return GetPString(PSTR_cc_76);
    case  77: return GetPString(PSTR_cc_77);
    case  78: return GetPString(PSTR_cc_78);
    case  79: return GetPString(PSTR_cc_79);
    case  80: return GetPString(PSTR_cc_80);
    case  91: return GetPString(PSTR_cc_91);
    case  92: return GetPString(PSTR_cc_92);
    case  93: return GetPString(PSTR_cc_93);
    case  94: return GetPString(PSTR_cc_94);
    case  95: return GetPString(PSTR_cc_95);
    case  96: return GetPString(PSTR_cc_96);
    case  97: return GetPString(PSTR_cc_97);
    case 120: return GetPString(PSTR_cc_120);
    case 121: return GetPString(PSTR_cc_121);
    case 122: return GetPString(PSTR_cc_122);
    case 123: return GetPString(PSTR_cc_123);
    case 124: return GetPString(PSTR_cc_124);
    case 125: return GetPString(PSTR_cc_125);
    case 126: return GetPString(PSTR_cc_126);
    case 127: return GetPString(PSTR_cc_127);
    default:  return GetPString(PSTR_cc_default);
  }
}


PSTRING(PSTR_mm_channel,           "%c%02d ");
PSTRING(PSTR_mm_no_channel,        "%c-- ");
PSTRING(PSTR_mm_unknown,           "unknown message");

PSTRING(PSTR_mm_note_off,          "note off %s, vel %d");   // 0x8. 
PSTRING(PSTR_mm_note_on,           "note on %s, vel %d");    // 0x9.
PSTRING(PSTR_mm_key_pressure,      "key pressure %s %d");    // 0xa.
PSTRING(PSTR_mm_cc,                "CC#%d %s %d");           // 0xb.
PSTRING(PSTR_mm_program_change,    "program change %d");     // 0xc.
PSTRING(PSTR_mm_channel_pressure,  "channel pressure %d");   // 0xd.
PSTRING(PSTR_mm_pitch_bend,        "pitch bend %ld");        // 0xe.

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
x type of CC: mod/volume/bank select/...
x all F.. thingies
- sysex, how to show
- running status ???
- implement settings and filters
- check MSB/LSB order of pitch bend and song position
*/

Page::LineResult PageMonitor::LineDecode(const midi_msg_t& msg)
{
  // --- Shortcut to the event ---
  const midi_event_t& e = msg.event;
  // --- The output text ---
  char* text = Screen::buffer;
  text[0] = 0;
  // --- Temporary string storage ---
  const uint8_t max_temp_len = 10;
  char temp[max_temp_len + 1];
  temp[max_temp_len] = 0;

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
          strncpy(temp, GetNoteName(e.m_data[1]), max_temp_len);
          sprintf(text, GetPString(PSTR_mm_note_off), temp, e.m_data[2]);
          break;
        case 0x9:
          strncpy(temp, GetNoteName(e.m_data[1]), max_temp_len);
          sprintf(text, GetPString(PSTR_mm_note_on), temp, e.m_data[2]);
          break;
        case 0xa:
          strncpy(temp, GetNoteName(e.m_data[1]), max_temp_len);
          sprintf(text, GetPString(PSTR_mm_key_pressure), temp, e.m_data[2]);
          break;
        case 0xb:
          strncpy(temp, GetCCName(e.m_data[1]), max_temp_len);
          sprintf(text, GetPString(PSTR_mm_cc), e.m_data[1], temp, e.m_data[2]);
          break;
        case 0xc:
          sprintf(text, GetPString(PSTR_mm_program_change), e.m_data[1]);
          break;
        case 0xd:
          sprintf(text, GetPString(PSTR_mm_channel_pressure), e.m_data[1]);
          break;
        case 0xe:
          //sprintf(text, GetPString(PSTR_mm_pitch_bend), static_cast<long>(e.m_data[2]) * 128ul + static_cast<long>(e.m_data[1])); // LSB, MSB
          sprintf(text, GetPString(PSTR_mm_pitch_bend), static_cast<long>(e.m_data[2]) * 128L + static_cast<long>(e.m_data[1]) - 8192L); // LSB, MSB, centered around 0
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


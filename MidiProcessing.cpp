#include "MidiProcessing.h"

#include "Data.h"
#include "Debug.h"
#include "MidiFilter.h"
#include "Utils.h"

#include "DinMidiboy.h"
#include <fifo.h>
#include <midi_serialization.h>

namespace
{
  using namespace MidiProcessing;

  Configuration configuration;                 // currently active MIDI configuration
  Configuration g_next_configuration;          // next configuration, to be set if ...
  bool g_next_configuration_available = false; // ... next_confuration_available = true
  bool g_panic = false; // to be set if panic has to be sent to all channels 
  MidiListener g_midi_in_listener = {nullptr, nullptr};
  MidiListener g_midi_out_listener = {nullptr, nullptr};
  
  typedef TFifo<midi_event_t, uint8_t, 128> fifo_t; // uses 512 bytes of RAM: 4 x 128

  fifo_t g_output_queue;
  MidiToUsb g_decoder;

  class State
  {
  public:
    State() { Init(); }
    void Init() {
      for (uint8_t i = 0; i < Configuration::m_max_number_of_output_channels; i++)
        m_note_is_on[i].SetAll(false);
    }
    Bitfield m_note_is_on[Configuration::m_max_number_of_output_channels];
  };

  State g_state;


  // === V E L O C I T Y   C U R V E S =========================================

  enum VelocityCurve { 
    Linear = 0, 
    Exponential = 1,  
    Logarithmic = 2
  };

  // The arrays below contain velocity mappings. 
  // They contain the expected output velocity for input velocities
  // 0, 4, 8, 12, 16, ... 128 
  // Note: Also tried putting all data in one big array, but that didn't save
  //       any memory. So, chose separate arrays because those are slightly
  //       more user friendly.

	const PROGMEM uint8_t PVELMAP_linear[33] = {
      0,   4,   8,  12,  16,  20,  24,  28,
     32,  36,  40,  44,  48,  52,  56,  60,
     64,  68,  72,  76,  80,  84,  88,  92,
     96, 100, 104, 108, 112, 116, 120, 124,
    128
  };

	const PROGMEM uint8_t PVELMAP_exponential[33] = {   // TODO!!!
      0,   4,   8,  13,  16,  20,  24,  28,
     32,  36,  40,  44,  48,  52,  56,  60,
     64,  68,  72,  76,  80,  84,  88,  92,
     96, 100, 104, 108, 112, 116, 120, 124,
    128
  };

  const PROGMEM uint8_t PVELMAP_logarithmic[33] = {
      0,  18,  25,  30,  35,  40,  44,  48,
     52,  56,  59,  63,  66,  69,  73,  76,
     79,  82,  85,  89,  92,  95,  98, 101,
    104, 107, 110, 113, 116, 119, 122, 125,
    128
  };

  const uint8_t* g_velocities = PVELMAP_linear;

  void SwitchVelocityMap(VelocityCurve curve)
  {
    switch(curve) {
      case VelocityCurve::Linear:      g_velocities = PVELMAP_linear;      return;
      case VelocityCurve::Exponential: g_velocities = PVELMAP_exponential; return;
      case VelocityCurve::Logarithmic: g_velocities = PVELMAP_logarithmic; return;
    }
    return;
  }

  uint8_t MapVelocity(uint8_t v_in)
  {
    if (v_in == 0)
      return 0; // NOTE ON with velocity 0 has special NOTE OFF meaning, so never change!
    const uint8_t from = v_in >> 2;
    const uint8_t table_from = pgm_read_byte((const uint8_t *)g_velocities + from); 
    const uint8_t table_to   = pgm_read_byte((const uint8_t *)g_velocities + from + 1); 
    uint8_t v_out = (((table_to - table_from) * (v_in - (from << 2)))>>2) + table_from;  // (should be < 85) * (is max 3)
    return v_out;
  }


  // === P R O C E S S I N G ===================================================

  // Fix for UsbToMidi::process() and 1 byte system common
  unsigned FIX_UsbToMidi_process(midi_event_t in, uint8_t out[3])
  {
    if (in.m_event == 0x1)
      in.m_event = 0x5;
    return UsbToMidi::process(in, out);
  }

  // Write directly to the midi out port
  void WriteEventToOutput(const midi_event_t& event)
  {
    uint8_t msg[3];
    uint8_t n = FIX_UsbToMidi_process(event, msg);
    for (uint8_t i=0; i<n; ++i)
      DinMidiboy.dinMidi().write(msg[i]);
    if (g_midi_out_listener.call_back)
      g_midi_out_listener.call_back(event, g_midi_out_listener.data);
  }

  void ProcessChannel(uint8_t channel, midi_event_t event, fifo_t& output_queue)
  // This function takes an input event, transforms it and sends it to an output channel
  {
    // -- Init ---
    const OutputConfiguration& output_channel = configuration.m_output_channel[channel];    
    Bitfield& output_channel_note_is_on = g_state.m_note_is_on[channel];

    // --- Filter message ---
    FilterSettingsValues filter = configuration.m_default_filter;
    if (configuration.m_override_default_filter) {
      filter.pitch_bend = output_channel.m_allow_pitch_bend;
      filter.cc_mod_wheel = output_channel.m_allow_modulation;
      filter.cc_other = output_channel.m_allow_control_change;
    }
    if (!MidiFilter::AllowMessage(filter, event))
      return;

    // --- Update channel ---
    event.m_data[0] = (event.m_data[0] & 0xf0) | (output_channel.m_channel & 0xf); 
    
    // --- Note on/off ---
    // Normally, every note ON is followed by a note OFF.
    // This code uses filters and as such does not send all note ONs to the output.
    // When -a bit later- the note OFF for such an event comes in, it is better to not send 
    // it to the output. (At least I think that maybe some simple/older MIDI implementations
    // might have problem with this.) Solution: keep track of the sent note ONs and only
    // send a note OFF, when an ON was sent. 
    if (event.m_event == 0x9 || event.m_event == 0x8) { // Note ON or OFF
      const uint8_t note = event.m_data[1];
      if (note < output_channel.m_minimum_note || note > output_channel.m_maximum_note)
        return;
      const int16_t transposed_note_16 = static_cast<int16_t>(note) + static_cast<int16_t>(output_channel.m_transpose);
      if (transposed_note_16 < 0 || transposed_note_16 > 127)
        return;
      const uint8_t transposed_note = static_cast<uint8_t>(transposed_note_16);
      event.m_data[1] = transposed_note; // update the note in the event
      uint8_t velocity = event.m_data[2];
      if (event.m_event == 0x09 && velocity > 0) { // Note ON
        // Apply velocity curve (or should we do it after the velocity filter?)
        velocity = MapVelocity(velocity);
        event.m_data[2] = velocity;
        // Filter based on velocity
        if (velocity < output_channel.m_minimum_velocity)
          return;
        if (velocity > output_channel.m_maximum_velocity)
          return;
        output_channel_note_is_on.Set(transposed_note, true); // Mark the (transposed) note as ON
      } else { // Note Off (either 0x8 or 0x9 with velocity 0)
        if (!output_channel_note_is_on.Get(transposed_note))
          return; // The note is not ON, so we don't send an OFF
        output_channel_note_is_on.Set(transposed_note, false); // Mark the (transposed) note as OFF
      }
    }

    // --- Push the reformatted event
    output_queue.push(event);
  }

  void ProcessInputEvent(midi_event_t& event, fifo_t& output_queue)
  {
    if (event.m_event >= 0x8 && event.m_event <= 0xe) {
      // --- Event for a specific channel ---
      const uint8_t channel = event.m_data[0] & 0x0f;
      if (channel == configuration.m_input_channel) { // input channel
        for (uint8_t i = 0; i < configuration.m_nbr_output_channels; i++)
          ProcessChannel(i, event, output_queue);
      } else { // other channel
        if (!EE::Settings().block_other) // Should really also be in the Configuration, as should velocity_curve?????
          output_queue.push(event); // pass if not blocked in settings
      }
    } else {
      // --- Event for all channels ---
      if (!MidiFilter::AllowMessage(configuration.m_default_filter, event))
        return;
      output_queue.push(event); // just pass for the moment, later possible filtering
    }
  }

  void SwitchOffAllNotes()
  // This started out as sending an "All Notes Off" message to all active output channels.
  // But not all synths treat this message correctly (I am talking about you, Dreadbox Typhon)
  // So we now just send a NOTE off for each note that is on.
  {    
    // For each channel, send note offs to all notes that are on 
    midi_event_t note_off_event;
    note_off_event.m_event = 0x8; // Note OFF
    note_off_event.m_data[2] = 64; // Typically unused velocity
    for (uint8_t i = 0; i < configuration.m_nbr_output_channels; i++) {
      note_off_event.m_data[0] = 0x80 | (configuration.m_output_channel[i].m_channel & 0x0f); // Note off on correct channel
      Bitfield& output_channel_note_is_on = g_state.m_note_is_on[i];
      for (uint8_t note = 0; note < 128; note++) {
        if (output_channel_note_is_on.Get(note)) {
          note_off_event.m_data[1] = note;    // Note number
          WriteEventToOutput(note_off_event); // Send the note off event
        }
      }
      output_channel_note_is_on.SetAll(false);
    }
  }

}

namespace MidiProcessing
{

  //==============================================================================
  // 
  //  C O N F I G U R A T I O N   O F   T H E   M I D I   P R O C E S S I N G
  //
  //==============================================================================

  OutputConfiguration::OutputConfiguration()
  {
    SetDefaults();
  }
  
  void OutputConfiguration::SetDefaults()
  {
    m_channel = 0;
    m_minimum_note = 0;
    m_maximum_note = 127;
    m_minimum_velocity = 0;
    m_maximum_velocity = 127;
    m_allow_pitch_bend = true;
    m_allow_modulation = true;
    m_allow_control_change = true;
    m_transpose = 0;
  }

  Configuration::Configuration()
  {
    SetDefaults();
  }
  
  void Configuration::SetDefaults()
  {
    m_input_channel = 0;
    m_nbr_output_channels = 0;
    m_default_filter = EE::Settings().filter;
    m_override_default_filter = false;
    for (uint8_t i = 0; i < m_max_number_of_output_channels; ++i)
      m_output_channel[i].SetDefaults();
  }

  //==============================================================================
  // 
  //                      M I D I   P R O C E S S I N G
  //
  //==============================================================================

  void Init()
  {
    g_decoder.setCable(0); // is this really necessary?
    configuration.m_nbr_output_channels = 0;
/*
    configuration.m_nbr_output_channels = 2;
    configuration.m_output_channel[0].m_channel = 4; // 5
    configuration.m_output_channel[0].m_minimum_velocity = 80;
    configuration.m_output_channel[0].m_transpose = -2*12; // 10
    configuration.m_output_channel[1].m_channel = 9; // 10
    configuration.m_output_channel[1].m_minimum_note = 48; // C3
    configuration.m_output_channel[1].m_maximum_note = 72; // C5
    configuration.m_output_channel[1].m_transpose = 2*12; // 10
*/
  }

  const Configuration& GetConfiguration()
  {
    return configuration;
  }
   
  void SetNextConfiguration(const Configuration& next_configuration)
  {
    g_next_configuration_available = true;
    g_next_configuration = next_configuration;
  }

  bool ActivateNextConfigurationIfAvailable()
  {
    if (!g_next_configuration_available)
      return false;

    // TODO: We should return here if we're in the middle of sending SysEx.

    SwitchOffAllNotes();
    // The messages to switch of the notes on the output channels have been put on the output queue.
    // We can safely change the configuration.
    configuration = g_next_configuration;
    // Switch the velocity map
    SwitchVelocityMap(static_cast<VelocityCurve>(EE::Settings().velocity_curve));
    // We are done
    g_next_configuration_available = false;
    return true;
  }

  void SetPanic()
  {
    g_panic = true;
  }

  void SendPanicIfNeeded()
  {
    if (!g_panic)
      return;
 
    midi_event_t note_off_event;
    note_off_event.m_event = 0xb; // Control change
    note_off_event.m_data[1] = 0x78; // All Sound Off
    note_off_event.m_data[2] = 0x00; // unused
    for (uint8_t i = 0xb0; i < 0xc0; i++) {
      note_off_event.m_data[0] = i; // Control change on correct channel
      WriteEventToOutput(note_off_event); // Send the note off event
      }

    g_panic = false;
  }

  void SetMidiInListener(const MidiListener& midi_listener)
  {
    g_midi_in_listener = midi_listener;
  }

  void SetMidiOutListener(const MidiListener& midi_listener)
  {
    g_midi_out_listener = midi_listener;
  }

  /* 
  A word about g_decoder.process(), this function processes a MIDI input stream byte by byte.
  It returns eather false or true:
  - false: Not enough bytes have been treated to construct a complete midi_event_t. Please send more bytes.
  - true: The midi_event_t is ready, please treat the midi_event_t.
  To do this, it keep state!

  A midi_event_t consists of the following parts:
  - m_event: the type of message as interpreted by g_decoder.process() from the bytes of the MIDI stream
	- m_data[3]: the actual bytes of the MIDI stream, depending on the type 1 to 3 bytes are set
  
  The function returns the following values for the m_event:
  - 0x2:     2 byte system common (F1=time code, F3=song select)
    0x3:     3 byte system common (F2=song position pointer)
    0x4:     sysex data
    0x5:     1 byte system common (F4, F6, F6=tune request) 
             => DAMNED!!! Should have been 0x1 => made local workaround, don't want to touch Blokas libs 
    0x5:     end of sysex data, F7 in m_data[0]
    0x6:     end of sysex data, F7 in m_data[1]
    0x7:     end of sysex data, F7 in m_data[2]
    0x8-0xE: the "channel" messages, such as key on/off, identical to midi command byte
    0xF:     midi real time (F8, FA, FB, FC, FE, FF)
  Note that the m_event has the possibility to store cables (in case you have many MIDI ports), 
  but we do not use that. 

  A word about system exclusive. Since these messages don't have fixed length, they are split up
  into a series of midi_event_t:
    0x4: F0, ??, ??
    0x4: ??, ??, ??
    ...
    0x4: ??, ??, ??
  It ends with one of the following
    0x5: F7, 00, 00
    0x6: ??, F7, 00
    0x7: ??, ??, F7
  Note that the in between these midi_events_t, system real time messages (0xF) may arrive!
  */

  // Fix for g_decoder.process() and 1 byte system common
  bool FIX_g_decoder_process(uint8_t byte, midi_event_t &out)
  {
    // 0x5 can be either a 1 byte system common (F4, F6, F6=tune request) message
    // or the end of system exclusive!
    // This is confusing. Would be better to use 0x1 for these 1 byte system common
    // WARNING: Don't know what the impact on latency is of this !!!!!!
    bool event_is_complete = g_decoder.process(byte, out);
    if (event_is_complete && out.m_event == 0x5 && out.m_data[0] != 0xF7)
      out.m_event = 0x1;
    return event_is_complete;
  }

  void TreatInput()
  {
    while (g_output_queue.hasSpaceFor(Configuration::m_max_number_of_output_channels /*number of involved output channels*/) && DinMidiboy.dinMidi().available())
    {
      midi_event_t event;
      if (FIX_g_decoder_process(DinMidiboy.dinMidi().read(), event)) {
        ProcessInputEvent(event, g_output_queue);
        if (g_midi_in_listener.call_back)
          g_midi_in_listener.call_back(event, g_midi_in_listener.data);
      }
    }
  }
  
  void WriteToOutput()
  {
    midi_event_t event;
    while (g_output_queue.pop(event))
      WriteEventToOutput(event);
  }

}
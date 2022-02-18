#include "MidiProcessing.h"

#include "Debug.h"
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

  /*

  bool midi_is_note_on(const midi_event_t &event)
  {
    return (event.m_data[0] & 0xf0) == 0x90 && event.m_data[2] >= 0;
  }

  bool midi_is_note_off(const midi_event_t &event)
  {
    uint8_t status = event.m_data[0] & 0xf0;
    return (status == 0x80) || (status == 0x90 && event.m_data[2] == 0);
  }

  bool midi_is_all_notes_off(const midi_event_t &event)
  {
    return (event.m_data[0] & 0xf0) == 0xb0 && event.m_data[1] >= 123;
  }

  uint16_t g_channelsHadActivity = 0;

  void clearAllNotes(fifo_t &outputQueue)
  {
    midi_event_t allNotesOff;
    allNotesOff.m_event = 0x0b;
    allNotesOff.m_data[1] = 123;
    allNotesOff.m_data[2] = 0;
    for (uint16_t i=0; i<16; ++i)
    {
      if (g_channelsHadActivity & (1 << i))
      {
        allNotesOff.m_data[0] = 0xb0 | i;
        outputQueue.push(allNotesOff);
      }
    }
    g_channelsHadActivity = 0;
  }


  void processEvent(fifo_t &outputQueue, const midi_event_t &event)
  {
    if (midi_is_note_on(event) || midi_is_note_off(event))
    {
      g_channelsHadActivity |= 1 << (event.m_data[0] & 0x0f);
      //produceEvents(outputQueue, event, g_semitones, MAX_NOTES);
    }
    else if (midi_is_all_notes_off(event))
    {
      g_channelsHadActivity &= ~(1 << (event.m_data[0] & 0x0f));
      outputQueue.push(event);
    }
    else
    {
      outputQueue.push(event);
    }
  }

  void sendSysEx(fifo_t &outputQueue, char* s)
  // It would be better to create a stream and write bytes to it
  // and then use decode, I think
  {
    // --- SysEx Start ---
    midi_event_t sysex;
    sysex.m_event   = 0x0F;
    sysex.m_data[0] = 0xF0;
    sysex.m_data[1] = 0;
    sysex.m_data[2] = 0;
    outputQueue.push(sysex);

    // --- Blokas (00 21 3B) + midiboy (11) ---
    sysex.m_data[0] = 0x00;
    outputQueue.push(sysex);
    sysex.m_data[0] = 0x21;
    outputQueue.push(sysex);
    sysex.m_data[0] = 0x3B;
    outputQueue.push(sysex);
    sysex.m_data[0] = 0x11;
    outputQueue.push(sysex);

    // --- Sysex Data ---
    char* p = s;
    while (*p)
    {   
      sysex.m_data[0] = static_cast<uint8_t>(*p);
      outputQueue.push(sysex);
      p++;
    }

      // --- SysEx End ---
    sysex.m_data[0] = 0xF7;
    outputQueue.push(sysex);
  }

  */


/*
  inline uint8_t GetChannel(uint8_t command)
  {
    return command & 0x0F;
  }
  inline uint8_t GetChannel(const midi_event_t& event)
  {
    return GetChannel(event.m_data[0]);
  }

  inline void SetChannel(uint8_t& command, uint8_t channel)
  {
    command = command & 0xf0 + channel &0x0f;
  }
  inline void SetChannel(midi_event_t& event, uint8_t channel)
  {
    SetChannel(event.m_data[0], channel);
  }
*/

  void ProcessChannel(uint8_t channel, midi_event_t event, fifo_t& output_queue)
  // This function takes an input event, transforms it and sends it to an output channel
  {
    // -- Init ---
    const OutputConfiguration& output_channel = configuration.m_output_channel[channel];    
    Bitfield& output_channel_note_is_on = g_state.m_note_is_on[channel];

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
      if (event.m_event == 0x09) { // Note ON
        const uint8_t velocity = event.m_data[2];
        if (velocity < output_channel.m_minimum_velocity)
          return;
        output_channel_note_is_on.Set(transposed_note, true); // Mark the (transposed) note as ON
      } else { // Note Off
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
        output_queue.push(event); // just pass for the moment, later possible filtering
      }
    } else {
      // --- Event for all channels ---
      output_queue.push(event); // just pass for the moment, later possible filtering
    }
  }

  bool SwitchOffAllNotes(fifo_t& output_queue)
  // Sends All Notes Off on the config channels where notes were on.
  {
    if (g_output_queue.hasSpaceFor(Configuration::m_max_number_of_output_channels /*number of involved output channels*/)) {
      midi_event_t all_notes_off_event;
      all_notes_off_event.m_event = 0xb; // Control Change
      all_notes_off_event.m_data[1] = 0x7b; // All Notes Off
      all_notes_off_event.m_data[2] = 0x00; // ignored
      for (uint8_t i = 0; i < configuration.m_nbr_output_channels; i++) {
        if (g_state.m_note_is_on[i].HasAtLeastOne(true)) {
          // Only send all notes off, if some notes were on.
          // This is needed to prevent sending notes off to all channels we are iterating over in the menu
          // while moving to the channel we want.  
          all_notes_off_event.m_data[0] = 0xb0 | (configuration.m_output_channel[i].m_channel & 0x0f);
          output_queue.push(all_notes_off_event);
          g_state.m_note_is_on[i].SetAll(false);
        }
      }
      return true;
    } else {
      return false;
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
    m_allow_pitch_modulation = true;
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
      return true;

    if (SwitchOffAllNotes(g_output_queue)) {
      // The messages to switch of the notes on the output channels have been put on the output queue.
      // We can safely change the configuration.
      configuration = g_next_configuration;
      g_next_configuration_available = false;
      return true;
    } else {
      // Output queue is full, cannot add note offs
      // Please retry after flushing the output queue.  
      return false;
    }
  }

  void SetMidiInListener(const MidiListener& midi_listener)
  {
    g_midi_in_listener = midi_listener;
  }

  void SetMidiOutListener(const MidiListener& midi_listener)
  {
    g_midi_out_listener = midi_listener;
  }

  void TreatInput()
  {
    while (g_output_queue.hasSpaceFor(Configuration::m_max_number_of_output_channels /*number of involved output channels*/) && DinMidiboy.dinMidi().available())
    {
      midi_event_t event;
      if (g_decoder.process(DinMidiboy.dinMidi().read(), event)) {
        ProcessInputEvent(event, g_output_queue);
        if (g_midi_in_listener.call_back)
          g_midi_in_listener.call_back(event, g_midi_in_listener.data);
      }
    }
  }
  
  void WriteToOutput()
  {
    uint8_t msg[3];
    midi_event_t event;
    while (g_output_queue.pop(event))
    {
      uint8_t n = UsbToMidi::process(event, msg);
      for (uint8_t i=0; i<n; ++i)
        DinMidiboy.dinMidi().write(msg[i]);
      if (g_midi_out_listener.call_back)
        g_midi_out_listener.call_back(event, g_midi_out_listener.data);
    }
  }

}
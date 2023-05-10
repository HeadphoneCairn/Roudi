#pragma once

#include "Data.h"
#include "Page.h"
#include "Utils.h"

#include <midi_serialization.h> // for midi_event_t

class PageMonitor: public Page
{
public:
  PageMonitor();
public: // For the listening interface
  struct midi_msg_t {
    midi_event_t event;
    uint8_t input:1;
  };
  static const uint8_t m_num_messages = 128; // Used to be 64, but was able to free more memory.
  TCircularBuffer<midi_msg_t, uint8_t, m_num_messages> m_messages;
  MidiMonSettingsValues m_settings;
protected:
  virtual const char* GetTitle() override;
  virtual LineResult Line(LineFunction func, uint8_t line, uint8_t field) override;
  virtual void OnStart(uint8_t dummy = 0xFF) override;
  virtual void OnStop() override;
private:
  LineResult LineDecode(const midi_msg_t& event);
};

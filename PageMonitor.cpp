#include "PageMonitor.h"

#include "Debug.h"
#include "Data.h"
#include "MidiProcessing.h"

PSTRING(PSTR_page_monitor, " MIDI MONITOR "); 

namespace {
  const uint8_t max_num_messages = 10; 
  void Listen(const midi_event_t& event)
  {

  }

  //  typedef TFifo<midi_event_t, uint8_t, 128> fifo_t; // uses 512 bytes of RAM: 4 x 128

}

PageMonitor::PageMonitor(): Page()
{
}

void PageMonitor::OnStart(uint8_t)
{
  SetNumberOfLines(max_num_messages);

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
  return TextLine(func, PSTR_page_monitor);
}



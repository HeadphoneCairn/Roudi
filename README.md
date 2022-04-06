# Roudi

Roudi is a **ROU**ter for mi**DI** messages.

Created by Headphone Cairn for the [Blokas midiboy](https://blokas.io/midiboy/).

Roudi was created to make it easy to use a MIDI master keyboard with basic functionality, such as a digital piano, to control several MIDI instruments. It has the following functionality:
- SINGLE: Easy selection of output channel.
- MULTI: Layering and splitting with additional options such as velocity, pitch bend, ... filtering. Several MULTIs can be saved and maintained. 
- MONITOR: A MIDI monitor.
- SETTINGS: Allows for simple message filtering.

Disclaimer: Due to the limitations of the midiboy, Roudi is also somewhat limited in functionality and ease of use. Furthermore, most of the program storage space is used, as is the RAM memory, so not many improvements are still possible.

# Usage

## Keys

- **left**, **right**: Modify values.
- **up**, **down**: Switch between fields and/or scroll up and down.
- **A**: Go to previous page.
- **B**: Go to next page.
- combined **A + B** press: Switch to second level pages and back. Roudi is based on two levels of pages, the first level is for normal activity (playing music, SINGLE, MULTI, ...), while the second level is for monitoring and setting up roudi (MONITOR, SETTINGS)

## First Use

Roudi listens on a single input channel and converts its messages to output messages on other channels. It is probably best to start with setting that *Input channel*. When you switch on Roudi, you'll see the SINGLE page. Press and release A+B to switch to the second level of pages and press B until you are in the SETTINGS page. Now, set the *Input channel* to the output channel of you master keyboard connected to the midiboy.

## Level One Pages

These pages are used when you want to play music. 

Press and release A+B to switch to the level two pages.

### SINGLE page

- Use **up** and **down** to select output channel.
- Use **left** or **right** to switch to the naming of the channel.

### MULTI page

- If you select the same channel twice in LAYER of SPLIT mode, only the left channel will be used.
- The pitch bend, mod wheel, ... on/off overrides the same filters in the SETTINGS.

## Level Two Pages

These pages are used when you want to monitor the MIDI messages or setup Roudi. 

Press and release A+B to switch to the level one pages.

### SETTINGS page

### MONITOR page

### MONITOR SETTINGS page

## Concepts and Notes

- All changes you make are saved automatically.
- The MIDI configuration active on the level on page remains active in the level two page. So, e.g. if you are on a MULTI and you switch to the MONITOR, you'll see the output of the MULTI routing.
- There are two conventions for numbering notes in MIDI:
  1. MIDI note 60 = C3 
  2. MIDI note 60 = C4 = middle C = 261.63Hz, MIDI note 69 = A4 = 440.00Hz
  We chose the second convention. 
- If you change the MIDI channels of your MIDI instruments, its Roudi channel name will no longer correspond. The easiest way to correct is, is to go into the old name in SINGLE and save it to the new channel number.
- Every time you change the midi configuration such as: select a channel in single or multi, change octave, velocity, setting, ... note offs are sent to all active MIDI channels. So, basically when you change stuff, any note you are holding down an your keyboard will be terminated.

# Changelog

- version 0.1: Beta release




# TODO

Filters: program change overrides cc filter for CC0
         mod wheel overrides cc filter for CC1



# Programming stuff:

#define ENABLE_RAW exist in Debug.cpp
#define ENABLE_DEBUG exists in PageMonitor.cpp
#define SET_DEFAULT_CHANNEL_NAMES exists in Data.cpp

# Credits and License

- Roudi written by Headphone Cairn Software.

  ![Headphone Cairn Logo](README.png)

- MIDI libraries and more by [Blokas](https://blokas.io/).
- An stl for a case for the midiboy is available on [Thingiverse](https://www.thingiverse.com/thing:4878526).
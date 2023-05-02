# Roudi

Roudi is a **ROU**ter for mi**DI** messages, created for the [Blokas midiboy](https://blokas.io/midiboy/).

I made this to make it easier to use my digital piano as a master keyboard, without having to go through a computer.  

Highlights:
- Easily switch the output channel. Name the channels.
- Setup splits and layers.
- Filter out unwanted MIDI messages.
- Includes a MIDI monitor.
- Setup a velocity curve for your master keyboard.
- Auto save of settings.

Lowlights:
- Only DIN MIDI, no USB MIDI.
- Due to the limitations of the midiboy, Roudi is somewhat limited in functionality and ease of use. Because most of the program storage space has been used, it will be quite hard to add extra functionality.


# Changelog

## version 1.0

- Initial release (private)

## version 1.1

- First public release.
- Added page to edit velocity curves.
- The split note can now be set using the keyboard in an MULTI.


# Usage

## Keys

- **left**, **right**: Modify values.
- **up**, **down**: Switch between fields and/or scroll up and down.
- **A**: Go to previous page.
- **B**: Go to next page.
- combined **A + B** press: Switch to second level pages and back. Roudi is based on two levels of pages, the first level is for normal activity (playing music, SINGLE, MULTI, ...), while the second level is for monitoring and setting up Roudi (MONITOR, SETTINGS)

## First Use

Roudi listens on a single input channel and converts its messages to output messages on other channels. It is probably best to start with setting that *Input channel*. When you switch on Roudi, you'll see the SINGLE page. Press and release A+B to switch to the second level of pages and press B until you are in the SETTINGS page. Now, set the *Input channel* to the output channel of the master keyboard connected to the midiboy.

## Level One Pages

These pages are used when you want to play music.  
Press and release A+B to switch to the level two pages.

### SINGLE page

Use this page if you just want to directly play a single instrument.

**picture**

- Use **up** and **down** to select output channel.
- Use **left** or **right** to switch to the naming of the channel. In CHANNEL name you can supply a new name for the channel, or save the name to another channel by changing the channel number. 
  
  **picture**

### MULTI page

Use this page if you want to combine several instruments into a layer or a split. (Currently we only support two instruments.)  
MULTI also allows you to play a single instrument, like in SINGLE, but with additional functionality such as transposition, ... 

- If you select the same channel twice in LAYER of SPLIT mode, only the left channel will be used.
- The pitch bend, mod wheel, ... on/off overrides the same filters in the SETTINGS.

### ABOUT page

- Some information about Roudi
- On this page, the input channel is blocked. This is the same as "None" on the SINGLE page.

## Level Two Pages

These pages are used when you want to monitor the MIDI messages or setup Roudi.  
Press and release A+B to switch to the level one pages.

### SETTINGS page

- Block other channels: 

- Velocity curve. This allows you to change influence the output velocity. It can be useful if your keyboard is not as (or too) responsive as you would like. Note that the velocity mapping is applied **before** the velocity filter in MULTI.
  There are currently four velocity curves available 
  - **linear**: output is the same as input velocity 
  - **expon**ential: output has lower velocity than input
  - **logar**ithmic: output has higher velocity than input
  - **custom**: personalised velocity curve

  Graphical representation: 

  ![Velocity curves](bitmaps/VelocityCurves.png)

- Screen brightness: Self explanatory.

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



# TODO

Filters: program change overrides cc filter for CC0
         mod wheel overrides cc filter for CC1



# Programming stuff:

#define ENABLE_RAW exist in Debug.cpp
#define ENABLE_DEBUG exists in PageMonitor.cpp
#define SET_DEFAULT_CHANNEL_NAMES exists in Data.cpp

Memory usage


# Credits and License

- Roudi written by Headphone Cairn Software.

  ![Headphone Cairn Logo](bitmaps/Headphone%20Cairn%20Small.png)

- MIDI libraries and more by [Blokas](https://blokas.io/).
- An stl for a case for the midiboy is available on [Thingiverse](https://www.thingiverse.com/thing:4878526).

Created by Headphone Cairn Software for the [Blokas midiboy](https://blokas.io/midiboy/).

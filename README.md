# Roudi

Roudi is a **ROU**ter for mi**DI** messages, created for the [Blokas midiboy](https://blokas.io/midiboy/).

 ![Nice case](images/case.png)

I wrote this to make it easier to use my digital piano as a master keyboard.  
Without having to go through a computer.  

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


# Getting Started

Roudi listens on a single input channel and converts its messages to output messages on other channels.  
It is probably best to start with setting that *Input channel**. To do this:

1. Connect the DIN MIDI output of your master keyboard to the MIDI IN of your Midiboy.  
2. Connect the MIDI OUT of your Midiboy to your synths, MIDI thru box, ...
3. Switch on the Midiboy. You'll see the SINGLE page:  
  ![SINGLE init](screenshots/SINGLE_init_x2.png)

4. Press and release **A+B** to switch to the utility pages.
5. Press **B** until you are in the SETTINGS page:  
  ![SETTINGS init](screenshots/SETTINGS_init_x2.png)

6. Using the **left** or **right** buttons, set the *Input channel* to the output channel of your connected master keyboard.
7. There is no need to save anything. The setting is instantly activated.
8. Switch back to the SINGLE page by pressing **A+B**.
9. You can now switch the output channel using the **up** and **down** buttons.


# Button Bindings

Roudi is based on *pages*. These are screens that do stuff. Use 
- **A** to go to the previous page.
- **B** to go to the next page.

Roudi has two types of pages: 
  1. *Play pages*: pages for playing music ([SINGLE](#single-page), [MULTI](#multi-page), ...) 
  2. *Utility pages*: pages for monitoring and setting up Roudi ([SETTINGS](#settings-page), [MONITOR](#monitor-page), ...)

You can switch between the two types by pushing (and quickly releasing) **A + B** combined.  

Inside the pages, you can use:
- **left** and **right** to modify values.
- **up** and **down** to switch between fields and/or scroll up and down.


# Play Pages

These pages are used when you want to play music.  
Press and release A+B to switch to the utility pages.


# SINGLE page

Use this page if you just want to directly play a single instrument.

![SINGLE](screenshots/SINGLE_x2.png)

- Use **up** and **down** to move up and down through the list. Selecting a channel will route the output to that channel.
- A special value **none** is available that blocks the input channel. This is useful if you just want to play the built-in sound generator of your master keyboard without sending MIDI to another synth.
- At the bottom, there is a **> Panic!** entry which can be used like a MIDI Panic Button by pushing **left** or **right** on it. If at any time notes get stuck on your equipment, using panic should silence them.
- When you have selected one of the 16 channels you can change its name by pushing either **left** or **right**. You'll see the following page pop up:

  ![CHANNEL NAME](screenshots/CHANNEL_NAME_x2.png)

  - Use **left** and **right** to move the cursor.  
  - Use **up** and **down** on the text field to cycle through possible characters. You can use spaces (keep on pushing **up**). The name ends at the underscore (keep on pushing **down**). 
  - To save the name, push **up** or **down** on the **v**.  
  - To cancel the naming, push **up** or **down** on the **X**.  
  - An interesting feature is that you can change the channel to which you want to save the name. Do this by pushing **up** and **down** on the **ch??** field. This is particularly useful if you are moving a synth to a new channel.


# MULTI page

Use this page if you want to combine two instruments/sounds at the same time.  
You can also use it to have more control over a single instrument.  
As an added benefit, you can save several MULTIs, up to 12 to be exact. This can be useful as you might want to recall specific setup now and then. Every new MULTI becomes a new page. As with the other pages, you use the **A** and **B** buttons to switch between them. All MULTIes have a name and a number.

  ![MULTI](screenshots/MULTI_x2.png)

- Use **up** and **down** to jump between fields. **left** and **right** to change values.
- **Mode**:
  - _split_ splits the keyboard in two, the lower notes going to the **Left** channel, the higher to the **Right**.
  - _layer_ sends all notes to both **Left** and **Right** channel.
  - _left_ sends only to the **Left** channel.
  - _right_ sends only to the **Right** channel.
- **Split at**: Only available when **Mode** is set to _split_, this defines the key on the keyboard where the higher notes start. You can set it by selecting it and changing the value with the **left** and **right** buttons or by pressing a key on the keyboard.
  NOTE: you cannot play properly when you have selected **Split At**, because Roudi will be constantly updating the value 
- **Left**: The channel known as the left channel.
- **Right**: The channel known as the right channel. (If you select the same right channel as the left channel when in _layer_ of _split_ mode, only the left channel will be used.)
- The next fields are all double. The value to the left concerns the left channel, the other one the right channel. 
  - **Octave**: Transpose the channel up or down by the specified amount of octaves. (Sorry, no semitones.)
  - **Pitch bend**: Enable or disable routing pitch bend to the channel. Overrides the pitch bend filter in the [SETTINGS](#settings-page).
  - **Mod wheel**: Enable or disable routing mod wheel to the channel. Overrides the mod wheel filter in the [SETTINGS](#settings-page).
  - **Control change**: Enable or disable routing control changed messages to the channel. Overrides the cc filter in the [SETTINGS](#settings-page). (Sorry, no fine granular control.)
  - **Min velocity**, **Max velocity**: Specify the velocity interval of the notes that should be routed to the output channels. This is mostly used when **Mode** is set to _layer_, because it allows you to sent "silent" notes to one channel and "louder" notes to the other channel.
- More to the bottom you'll notice some specific commands that can be activated by pressing **left** or **right** button om them.
  - **New**: Create an new MULTI called "MULTI" and swith to it.
  - **Move left or right**: Is used to change the order of your MULTIes. Press the **left** or **right** button to move the current MULTI up or down in the list. You'll notice its number changes.
  - **Save as ...**: Pops up a new menu that allows you to (a) name the MULTI, (b) copy it to an existing MULTI or (c) copy it to a new MULTI.

    ![SAVE MULTI AS](screenshots/SAVE_MULTI_AS_x2.png)

    See [SINGLE](#single-page) on how to edit and save the name. MULTIs cannot contain lowercase characters. 
    (b) and (c) are achieved by changing the **mul** value. If you have a total of four MULTIs, there will be a mul05 value that you can use to save to new slot.
  - **Remove ...**: Pops up a conformation dialogue to allow you to remove the current MULTI. (Note that you cannot remove the last remaining MULTI.)
- At the bottom, there is a **> Panic!** entry which can be used like a MIDI Panic Button by pushing **left** or **right** on it. If at any time notes get stuck on your equipment, using panic should silence them.


# ABOUT page

Some information about Roudi.  
And some very basic help.  
On this page, the input channel is blocked. This is the same as "None" on the SINGLE page.


# Utility Pages

These pages are used when you want to monitor the MIDI messages or setup Roudi.  
NOTE: When you are in a utility page, the MIDI routing defined by the play page from which you switched stays active.
Press and release A+B to switch back to the play pages.


# SETTINGS page

- Block other channels: 

- Velocity curve. This allows you to change influence the output velocity. It can be useful if your keyboard is not as (or too) responsive as you would like. Note that the velocity mapping is applied **before** the velocity filter in MULTI.
  There are currently four velocity curves available 
  - **linear**: output is the same as input velocity 
  - **expon**ential: output has lower velocity than input
  - **logar**ithmic: output has higher velocity than input
  - **custom**: personalised velocity curve

  ![VELOCITY CURVE EDIT](screenshots/VELOCITY_CURVE_EDIT_x2.png)


  Graphical representation: 

  ![Velocity curves](images/VelocityCurves.png)

- Screen brightness: Self explanatory.

# MONITOR page

# MONITOR SETTINGS page
  
# Concepts and Notes

- All changes you make are saved automatically. Most of the time, this is really great. Sometimes it is annoying, but this design choice was made to keep things easy and clear.
- The MIDI configuration active on the level on page remains active in the level two page. So, e.g. if you are on a MULTI and you switch to the MONITOR, you'll see the output of the MULTI routing.
- There are two conventions for numbering notes in MIDI:
  1. MIDI note 60 = C3 
  2. MIDI note 60 = C4 = middle C = 261.63Hz, MIDI note 69 = A4 = 440.00Hz
  We chose the second convention. 
- If you change the MIDI channels of your MIDI instruments, its Roudi channel name will no longer correspond. The easiest way to correct is, is to go into the old name in SINGLE and save it to the new channel number.
- Every time you change the midi configuration such as: select a channel in single or multi, change octave, velocity, setting, ... note offs are sent to all active MIDI channels. So, basically when you change stuff, any note you are holding down an your keyboard will be terminated.
- Panic  is implemented by sending an *All Sound Off* message to all 16 output channels. Sadly, not all synthesizers have a full MIDI implementation and might ignore the *All Sound Off* message.


# TODO

Filters: 
- program change overrides cc filter for CC0
- mod wheel overrides cc filter for CC1

Allow mode to map mod wheel to aftertouch


# Programming stuff:

#define ENABLE_RAW exist in Debug.cpp
#define ENABLE_DEBUG exists in PageMonitor.cpp
#define SET_DEFAULT_CHANNEL_NAMES exists in Data.cpp

Memory usage

# Cases

If you'd like to have a case for Midiboy and have access to a 3D printer, you can use one of the stls available in the **cases** folder of this repository.  
They are also available on [Thingiverse](https://www.thingiverse.com/thing:4878526).  
There is a basic (named A) and a more advanced case (named B, as seen in the image a the top of this document).  
Note that the repo also contains some stls to print out a stand to tilt the device.

 
# Credits and License

- Roudi was written by Headphone Cairn Software. 

  ![Headphone Cairn Logo](images/Headphone%20Cairn%20Small.png)

- MIDI libraries and more by [Blokas](https://blokas.io/).


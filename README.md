# Roudi

Second version of MIDI Router for Blokas Midiboy

Roudi = ROUter for miDIboy

## Menus

- All menus:
  The selected line is remembered when switching between menus, but never saved to storage.
- Single / Split / Layer
  Values are remembered when switching to other menus.
  Saves values to EEPROM automatically when the values haven't changed for 5 minutes
- Presets
  Does no keep values when switching to other menus.
  Values must be saved to EEPROM manually, using "Save" or "Save As".
- Settings
  Keeps values when switching to other menus.
  Values are saved to EEPROM on every change.
  
## Limitations

- Menus show names of channels. However, internally they are stored by their channel number.
  This can cause issues when you change the output channel settings:
  
    Say you have set up your synth named SYNTHI on channel 3. And you use SYNTHI in a preset.
    After some time you need to change the channel of SYNTHI to 5. You make the changes in
    the output channel settings. Now, the preset will keep using channel 3 (and show
    the new name for channel 3). If you want your preset to use the SYNTHI again, you have to
    manually change it and Save it. 
  


## Changelog


## Notes
- There are two conventions for numbering notes in MIDI:
  1. MIDI note 60 = C3 
  2. MIDI note 60 = C4 = middle C = 261.63Hz, MIDI note 69 = A4 = 440.00Hz
  We chose the second convention. 


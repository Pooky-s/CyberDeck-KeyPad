# CyberDeck KeyPad 
A simple 12-key programmable keypad using an ESP32-S2 for a CyberDeck project.

⚠️ README file under construction ⚠️

## Summary
- [Requirements](#requirements)
- [Deployment instructions](#deployment-instructions)
- [Images](#images)
- [Sources](#sources)

### Requirements 
To build your own Keypad you will need : 
- some diodes (one for each key) 
- Switches (here CherryMX switches without diodes nor LEDs)
- ESP32-S2 board (here ESP32-S2-DevKitM-1)
- a micro-USB female port
- a PCB or some wires. 

### Deployment Instructions
If you already have a working development environment :
- Install the Arduino core for ESP32 (arduino-esp32 -> [sources](#sources)) under `<arduino_installation_dir>/hardware/espressif/esp32`
- Copy/paste the EspTinyUSB library (EspTinyUSB -> [sources](#sources)) under `<arduino_installation_dir>/libraries`
- Compile and upload the `.ino` file located `./src/<name>.ino`

If you don't have a proper development environment, or need more informations go to `./docs/README.md` (-> to be added)

### Images

![schematic](schemes/png_files/kbd_preview_schematic.png)
Other pics coming soon...

### Sources 
- [EspTinyUSB](https://github.com/chegewara/EspTinyUSB) by [Chegewara](https://github.com/chegewara)
- [arduino-esp32](https://github.com/espressif/arduino-esp32) by [Espressif](https://github.com/espressif)
- [ESP32-S2 USB demo using Arduino IDE](https://www.youtube.com/watch?v=l3MmrVHMd94&ab_channel=LucaDentella) by [Luca Dentella](https://www.youtube.com/channel/UCQewD5sQ1WsJCIAuOMZ2V2w)



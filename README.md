# CyberDeck KeyPad 
A simple 12-key programmable keypad using an ESP32-S2 for a CyberDeck project.

## Disclaimer ⚠️

This project is my first one, also I'm an absolute beginner in pcb design. The project may have some flaws, bugs, errors... It's still under development and is subject to changes. If you have any suggestions, feel free to share them, it will help me learn !  

## Summary
- [Requirements](#requirements)
- [Deployment instructions](#deployment-instructions)
- [TODO](#todo)
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
- Copy/paste the EspTinyUSB library (EspTinyUSB, ESPAsyncWebServer and AsyncTCP  -> [sources](#sources)) under `<arduino_installation_dir>/libraries`
- Install libraries needed from the library manager (FastLED, Keypad -> [sources](#sources))
- Compile and upload the `.ino` file located at `./src/<name>.ino`

If you don't have a proper development environment, or need more informations go to `./docs/README.md` (-> to be added)

### TODO
- Software
    + Rewrite HID keys definitions 
    + Investigate "85" bug where writing 85 on key 12 does nothing 
    + Add function to read keys from storage once initialization check is done
    + Add function to convert website input to values as `HID_KEY_...` format
    + Add function to convert keys read from storage to human readable values 
    + Add multikey macro creation
    + Optimize website
    + Optimize code
    + Add comments
- Hardware 
    + Make a testing unit using PCB 
        * Fix ground pins size for Cherry MX swithes 
        * Bigger holes for mounting solutions 
        * Add names near pins (silkscreen)
    + Expose pins in v2 
    + Add 2 encoders for mouse control in v2
    + Add LEDs ? 
- Documentation
    + Add step-by-step installation guide
    + Add software description 
    + Add pictures of test and final units
    + Add BOM 
    
### Images

![schematic](schemes/png_files/kbd_preview_schematic.png)
Other pics coming soon...

### Sources 
Projects and Libraries used :
- [EspTinyUSB](https://github.com/chegewara/EspTinyUSB) by [Chegewara](https://github.com/chegewara)
- [arduino-esp32](https://github.com/espressif/arduino-esp32) by [Espressif](https://github.com/espressif)
- [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) by [me-no-dev](https://github.com/me-no-dev) 
- [AsyncTCP](https://github.com/me-no-dev/AsyncTCP) also by [me-no-dev](https://github.com/me-no-dev) 
- [FastLED](https://github.com/FastLED/FastLED) by Jason Coon and Mark Kriegsman 
- [Keypad](https://github.com/Chris--A/Keypad) by Mark Stanley and Alexander Brevig, maintained by [Chris--A](https://github.com/Chris--A)
- [A little sample code to format the SPIFFS](https://techtutorialsx.com/2019/02/24/esp32-arduino-formatting-the-spiffs-file-system/)

Tutos, demos and other projects that inspired the CyberDeck Keypad.
- [ESP32-S2 USB demo using Arduino IDE](https://www.youtube.com/watch?v=l3MmrVHMd94&ab_channel=LucaDentella) by [Luca Dentella](https://www.youtube.com/channel/UCQewD5sQ1WsJCIAuOMZ2V2w)
- [Input data from a WebServer and save it to SPIFFS](https://randomnerdtutorials.com/esp32-esp8266-input-data-html-form/)
- [Webserver and Access-Point on ESP32 tutorial](https://lastminuteengineers.com/creating-esp32-web-server-arduino-ide/)
- [Arduino Pro Micro Macro Kyeboard](https://www.youtube.com/watch?v=IDlcxLQ1SbY&ab_channel=RyanBates) by [Ryan Bates](https://www.youtube.com/channel/UC2cpPe45iWKwrZc0OK8k-Pw)


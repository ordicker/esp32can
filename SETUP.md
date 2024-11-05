# How to setup this project 

## project setup cmd
`pio project init --ide emacs --board esp32-c3-devkitc-02`

## Compile
`pio run`

## Upload
`pio run -t upload`

## Setup serial output
viewing the serial output @terminal
```
sudo minicom -b 115200 -o -D /dev/ttyUSB0
```

## Debugging
??

## Unit testing
??

## Adding new Library
[lib_deps](https://docs.platformio.org/en/latest/projectconf/sections/env/options/library/lib_deps.html#projectconf-lib-deps)
### example
- adding to platformio.ini:
```
lib_deps =
  ; CANBUS provide <ESP32-TWAI-CAN.hpp>
  handmade0octopus/ESP32-TWAI-CAN@^1.0.1
```
- Then run 
`pio pkg install`
- rerun compile_commands.json (for the LSP) TODO: make that automated
`pio run -t compiledb`

## Setting up LSP (clangd)

### generate "compile_commands.json" file
`pio run -t compiledb`
### make a soft link to project root
`ln -s .pio/build/esp32-c3-devkitc-02/compile_commands.json compile_commands.json`
### ESP32 compiler 
.platformio/packages/toolchain-riscv32-esp/bin/riscv32-esp-elf-g++


## Web server

### ESP side
using **ESPAsyncWebServer-esphome** package

### client side:
- PREACT
- gzip for memory save 

## Writing state value to the eeprom (and not to the flash)

## CANBUS (j1939)

### CANBUS analyzer 

#### installing drivers

[SocketCAN Driver for Microhip CANBUS analyzer](https://github.com/rkollataj/mcba_usb)
- didn't work on modern linux (giving up for now)


## Algorithm (by Zvika)



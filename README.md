[![Build Status](https://travis-ci.org/CODeRUS/GyverLampCpp.svg?branch=master)](https://travis-ci.org/CODeRUS/GyverLampCpp)
<br />
You can download latest build with artifacts from GitHub [Releases](https://github.com/CODeRUS/GyverLampCpp/releases) page
<br />
# GyverLampCpp

Rewrite of https://github.com/AlexGyver/GyverLamp in C++ and classes for PlatformIO: https://platformio.org/platformio-ide

Contains effects ported from:
- https://community.alexgyver.ru/threads/wifi-lampa-budilnik-obsuzhdenie-proshivki-ot-gunner47.2418/page-72#post-33652
- https://github.com/pixelmatix/aurora
- https://github.com/marcmerlin/FastLED_NeoMatrix_SmartMatrix_LEDMatrix_GFX_Demos

Contains DMX core ported from:
- https://github.com/Aircoookie/WLED

All original code is owned by it's authors

# How to build

0. Tested with envs: sonoff-r1, sonoff-r1-4m, nodemcu, esp32dev
1. build firmware: `pio run -e nodemcu`
2. build filesystem: `pio run --target buildfs -e nodemcu`

# Filesystem build

Manual steps:

- build https://github.com/CODeRUS/led-lamp-webui
- build https://github.com/CODeRUS/wifimanager-react-page
- copy gzipped artifacts without folder structure to data folder
- build filesystem in Platform IO

Easy steps:

- grab data.zip from latest release (https://github.com/CODeRUS/GyverLampCpp/releases)
- extract to project folder
- build filesystem in Platform IO

Then just upload built fs to module using `pio run --target uploadfs -e nodemcu`

## Configuration

src/MyMatrix.cpp - change led pin here
src/EffectsManager.cpp - comment out unused effects here

data/effects.json - default options for effects

    i - id of effect, should match one in src/EffectsManager.cpp
    n - name of eefect visible in ui and Home Assistant
    s - effect speed
    l - effect scale
    b - effect brightness

data/settings.json - main settings of firmware
matrix - settings of matrix

    type - match with Framebuffer GFX: https://github.com/marcmerlin/Framebuffer_GFX/blob/master/Framebuffer_GFX.h#L43
    values should be combined. Example: NEO_MATRIX_ZIGZAG + NEO_MATRIX_BOTTOM + NEO_MATRIX_RIGHT + NEO_MATRIX_ROWS (or NEO_MATRIX_COLUMNS) = 11 (or 15). Matrix test effect at boot: Pixels started from left bottom. Red is horizontal from left to right, Green is vertical from bottom to top.
    rotation - value from 0 to 3, each value by +90 degree.

connection - connection settings

    mdns - domain name in .local
    apName - access point name for initial configuration
    ntpServer - time server name
    ntpOffset - time offset in seconds
    hostname - hostname for local network

mqtt - mqtt settings

    host - server address
    port - server port
    username - username
    password - password
    uniqueId - unique identifier for entity light in Home Assistant
    name - device name in Home Assistant
    model - model name in Home Assistant

button - button settings

    pin - GPIO pin number
    type - 0 for PullTypeHigh or 1 for PullTypeLow
    state - 0 for DefaultStateOpen or 1 for DefaultStateClose


## Changes with original GyverLamp projects

- Rewritten in C++ and classes for easier maintenance
- ESP32 support
- Sonoff Basic support
- Asynchronous WiFi Manager
- FastLED_NeoMatrix library instead of Gyver handmade one (sorry)
- Settings moved to SPIFFS json file

## New features:

- Clock effects
- Spectrometer effect (requires microphone module or aux, should be uncommented in EffectsManager.cpp and added to effects.json)
- React wifi manager self-coded component
- React web with controls self-coded component. No need to install client applications
- Firmware update page, allowing to upload firmware bin, spiffs bin or settings json
- MQTT for Home Assistant integration
- Sonoff Basic relay and led are bound to led state
- DMX input via e131 and Art-Net protocols

## Missing features from original project

- Not compatible with any client application. You can connect to lamp directly using any web browser. Or use MQTT/HomeAssistant to control it
- Alarms not implemented

## TODO

- Alarms support

## Audio input wirings for Spectrometer effects

Please check by link: https://gist.github.com/CODeRUS/f025fa5c12d6eaad6878576e8255913a

![Github Actions CI Status](https://github.com/CODeRUS/GyverLampCpp/workflows/PlatformIO%20build/badge.svg)

<br />
You can download latest build with artifacts from GitHub [Releases](https://github.com/CODeRUS/GyverLampCpp/releases) page
<br />
# GyverLampCpp

Rewrite of https://github.com/AlexGyver/GyverLamp in C++ and classes for PlatformIO: https://platformio.org/platformio-ide

Discuss building, modifications, deployment, etc. in telegram group: [@GyverLampCpp](https://t.me/GyverLampCpp)

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

**PLEASE!** Do not forget to build or to download and extract data artifacts from releases page! It doesn't preset in clonned git repository!

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
    ssid - name of wifi network to connect
    bssid - mac of wifi network, use for connection to certain access point
    password - password of wifi network to connect
    login - login for connection to wifi network (WPA-E)

mqtt - mqtt settings

    host - server address
    port - server port
    username - username
    password - password
    uniqueId - unique identifier for entity light in Home Assistant
    name - device name in Home Assistant
    model - model name in Home Assistant

button - button settings

    pin - GPIO pin number, set to 255 if you have no button connected
    type - 0 if pin button is connected to 3v3, 0 if button connected to GND
    state - set to 1 to invert button state, 0 for normal operation

Please refer to https://github.com/esp8266/Arduino/blob/master/variants/nodemcu/pins_arduino.h#L40 for pin numbers if using nodemcu-like ESP8266 boards! This configuration uses numeric gpio pins, not the ones marked as D0-D10 on the board!

## Safe mode

If lamp is turned on while button is pressed, it will boot into safe mode. Lamp will try to connect to last known network, otherwise it will create access point. You can connect to lamp with your mobile device wia wifi and upload correct json settings or firmware files.

Please do not use GPIO0, GPIO2, GPIO15 D3, D4 and D8 on nodemcu boards) for button connection, your board may fail to boot at all if putton will be pressed during startup: https://www.forward.com.au/pfod/ESP8266/GPIOpins/index.html

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

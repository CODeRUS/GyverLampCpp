[![Build Status](https://travis-ci.org/CODeRUS/GyverLampCpp.svg?branch=master)](https://travis-ci.org/CODeRUS/GyverLampCpp)
<br />
You can download latest build with artifacts from GitHub [Releases](https://github.com/CODeRUS/GyverLampCpp/releases) page
<br />
# GyverLampCpp

fork of https://github.com/AlexGyver/GyverLamp for PlatformIO

## Changes

- Rewritten for easier maintenance
- ESP32 support
- Asynchronous WiFi Manager
- FastLED_NeoMatrix library instead of Gyver handmade one (sorry)
- Settings moved to SPIFFS json file

## New features:

- Clock effects
- Spectrometer effect (requires microphone module or aux)
- React web with controls (over websockets)
- Firmware update page
- SPIFFS update page

## Known issues

- Alarms not implemented

## TODO

- Alarms support
- Explain connection of microphone and aux
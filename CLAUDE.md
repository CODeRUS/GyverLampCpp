# GyverLampCpp

C++ firmware for ESP8266/ESP32 RGB LED matrix lamp with 70+ effects, web UI, MQTT (Home Assistant), and DMX support.

## Build

PlatformIO project. Default environment: `esp32dev`.

```bash
pio run -e esp32dev          # Build firmware
pio run --target buildfs -e esp32dev   # Build filesystem (web UI assets in data/)
pio run --target upload -e esp32dev    # Flash firmware
pio run --target uploadfs -e esp32dev  # Flash filesystem
```

Makefile shortcuts: `make`, `make clean`, `make buildfs`, `make upload`, `make uploadfs` (pass env via `ARGS="-e nodemcu"`).

### Environments

- `esp32dev` — ESP32 DevKit (default, recommended)
- `nodemcu` — ESP8266 NodeMCU 4MB
- `esp01s` — ESP-01S 1MB (no OTA)
- `sonoff-r1` — Sonoff Basic 1MB (no OTA)
- `sonoff-r1-4m` — Sonoff Basic 4MB

### Prerequisites

The `data/` directory must contain web UI assets (gzipped HTML/JS). These are not in git — download from GitHub releases.

## Architecture

### Singletons (accessed via macros)

- `Settings::instance()` → `mySettings`
- `EffectsManager::instance()` → `effectsManager`
- `MyMatrix::instance()` → `myMatrix`

### Effect system

All effects extend `Effect` base class (`src/effects/Effect.h`) with virtual methods: `activate()`, `deactivate()`, `tick()` (pure virtual), `initialize(JsonObject)`, `writeSettings(JsonObject)`. Effects are registered in `EffectsManager::Initialize()`.

Effect categories in `src/effects/`: `basic/`, `noise/`, `fractional/`, `aurora/`, `sound/`, `network/`.

### Key source files

- `src/main.cpp` — entry point, WiFi, button handling
- `src/Settings.cpp` — JSON settings management
- `src/EffectsManager.cpp` — effect registration and lifecycle
- `src/MyMatrix.cpp` — LED matrix wrapper (FastLED_NeoMatrix)
- `src/LampWebServer.cpp` — web UI and REST API
- `src/MqttClient.cpp` — MQTT Home Assistant integration

## Code conventions

- C++11/14 with Arduino framework
- `#pragma once` for header guards
- Classes: PascalCase. Methods: camelCase.
- `F()` macro for string literals (PROGMEM)
- JSON keys use single letters to save flash (`"i"`=id, `"n"`=name, `"s"`=speed, `"l"`=scale, `"b"`=brightness)
- Platform conditionals: `#if defined(ESP32)`, `#if defined(SONOFF)`
- `override` keyword on virtual methods
- Anonymous namespaces for file-scoped globals

## CI

GitHub Actions: `.github/workflows/build.yml` — builds all 5 environments, downloads UI assets, creates releases.

## No tests

No unit tests. CI verifies compilation only.
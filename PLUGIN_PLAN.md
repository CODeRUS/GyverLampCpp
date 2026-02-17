# Plan: Plugin-Based Effect Loading for GyverLampCpp

## Context

Currently all 50+ effects are compiled into the firmware as C++ classes extending `Effect`. Adding or changing effects requires recompiling and reflashing. This plan adds a runtime plugin system where effects written as position-independent C code can be loaded from SPIFFS into executable RAM on ESP32 at boot. Built-in effects remain untouched.

## Architecture

```
SPIFFS /plugins/sparkles.bin
        |
        v
  PluginLoader        --reads .bin, validates header-->  heap_caps_malloc(MALLOC_CAP_EXEC)
        |                                                        |
        v                                                        v
  PluginEffect         --C++ adapter extends Effect-->   plugin C functions
  (activate/tick/deactivate)                             (called via resolved offsets)
        |
        v
  EffectsManager       --registers alongside built-in effects-->  effectsMap
```

Key design: plugins contain **no function pointers, no globals, no static data, no string literals**. The plugin header at offset 0 contains byte offsets to `activate`/`tick`/`deactivate` functions, patched by a post-build script. The firmware resolves these to absolute addresses by adding the load base address.

All firmware services (matrix drawing, timing, memory, color) are accessed through a `plugin_api_t` function pointer table passed to every plugin call.

**ESP32-only** -- guarded by `#if defined(ESP32)`.

## New Files

| File | Purpose |
|------|---------|
| `src/plugin/plugin_api.h` | Pure C header: API table, header struct, callback typedefs, color types |
| `src/plugin/PluginApiImpl.h` | Declaration of `plugin_api_init()` |
| `src/plugin/PluginApiImpl.cpp` | Static C-linkage wrappers bridging API calls to MyMatrix/Settings/FastLED |
| `src/plugin/PluginLoader.h` | `PluginLoader` class + `LoadedPlugin` struct |
| `src/plugin/PluginLoader.cpp` | SPIFFS scanning, binary loading, header validation, IRAM allocation |
| `src/plugin/PluginEffect.h` | `PluginEffect` adapter class extending `Effect` |
| `src/plugin/PluginEffect.cpp` | Delegates activate/tick/deactivate to resolved plugin functions |
| `plugins/Makefile` | Compiles `.c` plugins to `.bin` via Xtensa toolchain |
| `plugins/plugin.ld` | Linker script: places `.plugin_header` first, then `.text`, `.rodata` |
| `plugins/patch_header.py` | Post-build script: patches function offsets into the binary header |
| `plugins/sparkles_plugin.c` | Example: port of SparklesEffect |
| `plugins/color_plugin.c` | Example: port of ColorEffect |

## Modified Files

| File | Changes |
|------|---------|
| `src/EffectsManager.cpp` | Add `#if defined(ESP32)` block at end of constructor to load plugins and register `PluginEffect` instances |
| `platformio.ini` | (Optional) No changes strictly required; ESP32 env already provides `SPIFFS` and `heap_caps` |

## Plugin API (`src/plugin/plugin_api.h`)

Pure C header, no Arduino includes.

### Color types (C-compatible, matching CRGB/CHSV memory layout)
```c
typedef struct { uint8_t r, g, b; } plugin_rgb_t;
typedef struct { uint8_t h, s, v; } plugin_hsv_t;
```

### Settings snapshot (rebuilt before each plugin call)
```c
typedef struct {
    uint8_t speed;       // from Effect::settings.speed
    uint8_t scale;       // from Effect::settings.scale
    uint8_t brightness;  // from Effect::settings.brightness
    uint8_t width;       // from mySettings->matrixSettings.width
    uint8_t height;      // from mySettings->matrixSettings.height
} plugin_settings_t;
```

### API function table (firmware -> plugin)
```c
typedef struct plugin_api {
    uint32_t abi_version;

    // Matrix drawing (wraps MyMatrix methods)
    void     (*draw_pixel)(uint8_t x, uint8_t y, plugin_rgb_t color);
    uint32_t (*get_pixel)(uint8_t x, uint8_t y);  // 0x00RRGGBB
    void     (*clear)(void);
    void     (*fill)(plugin_rgb_t color);
    void     (*dim_all)(uint8_t value);
    void     (*fader)(uint8_t step);
    void     (*fade_pixel)(uint8_t x, uint8_t y, uint8_t step);
    void     (*draw_line)(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, plugin_rgb_t color);
    void     (*fill_rect)(uint8_t x, uint8_t y, uint8_t w, uint8_t h, plugin_rgb_t color);
    void     (*blend_pixel)(uint8_t x, uint8_t y, plugin_rgb_t color, uint8_t amount);
    void     (*blur2d)(uint8_t amount);
    uint8_t  (*wrap_x)(int8_t x);
    uint8_t  (*wrap_y)(int8_t y);
    void     (*get_leds)(uint8_t **buf, uint16_t *count);  // direct buffer

    // Timing
    uint32_t (*millis)(void);
    void     (*delay_ms)(uint32_t ms);

    // Random
    uint32_t (*random)(uint32_t max);
    uint32_t (*random_range)(uint32_t min, uint32_t max);

    // Memory (plugins MUST use these, not malloc/free)
    void*    (*alloc)(size_t size);
    void     (*free)(void *ptr);

    // Color utilities
    plugin_rgb_t (*hsv_to_rgb)(plugin_hsv_t hsv);
    plugin_rgb_t (*color_from_palette)(uint8_t palette_id, uint8_t index, uint8_t brightness);
    uint8_t  (*inoise8_3d)(uint16_t x, uint16_t y, uint16_t z);

    // Math helpers
    uint8_t  (*scale8)(uint8_t val, uint8_t scale);
    uint8_t  (*qadd8)(uint8_t a, uint8_t b);
    uint8_t  (*qsub8)(uint8_t a, uint8_t b);

    // Debug logging (numeric only -- no string literals in plugins)
    void     (*log_val)(uint32_t code);       // log a numeric code to Serial
    void     (*log_val2)(uint32_t a, uint32_t b); // log two values

    // Reserved for future expansion
    void     (*_reserved[8])(void);
} plugin_api_t;
```

Note: `api->log(const char*)` is intentionally omitted. String literals create relocations in the plugin binary. Plugins use numeric codes for debugging (e.g. `api->log_val(0x01)` for "activate called").

### Plugin header (at binary offset 0)
```c
#define PLUGIN_MAGIC       0x4C504C47  // "GPLP"
#define PLUGIN_ABI_VERSION 1

typedef struct {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t header_size;       // sizeof(plugin_header_t)
    char     uuid[37];          // UUID string "550e8400-e29b-..." null-terminated
    uint8_t  _pad_uuid[3];      // pad to 4-byte alignment
    uint32_t version;           // plugin version (semver-packed or sequential)
    char     name[48];          // display name, null-terminated
    uint32_t activate_offset;   // byte offset from binary start -> patched by build script
    uint32_t tick_offset;       // byte offset from binary start -> patched by build script
    uint32_t deactivate_offset; // byte offset from binary start -> patched by build script
} plugin_header_t;
```

The `uuid` field uniquely identifies the effect across all plugins and built-in effects. Built-in effects can also be assigned UUIDs if needed for distinction. The `version` field is reserved for future use (e.g. plugin updates, compatibility checks).

### Plugin callback signatures
```c
typedef void* (*plugin_activate_fn)(const plugin_api_t *api, const plugin_settings_t *settings);
typedef void  (*plugin_tick_fn)(const plugin_api_t *api, const plugin_settings_t *settings, void *state);
typedef void  (*plugin_deactivate_fn)(const plugin_api_t *api, void *state);
```

Note: these function pointers are **never stored inside the plugin binary**. The firmware computes them from `base_address + offset` at load time. This avoids relocations entirely.

## PluginApiImpl (`src/plugin/PluginApiImpl.cpp`)

Static functions with C linkage that bridge each `plugin_api_t` entry to the corresponding singleton method. Example:

```cpp
static void api_draw_pixel(uint8_t x, uint8_t y, plugin_rgb_t c) {
    myMatrix->drawPixelXY(x, y, CRGB(c.r, c.g, c.b));
}
static plugin_rgb_t api_hsv_to_rgb(plugin_hsv_t hsv) {
    CRGB rgb;
    hsv2rgb_rainbow(CHSV(hsv.h, hsv.s, hsv.v), rgb);
    return {rgb.r, rgb.g, rgb.b};
}
// ... one wrapper per API function
```

`plugin_api_init(plugin_api_t *api)` fills all fields.

## PluginLoader (`src/plugin/PluginLoader.h/.cpp`)

### `LoadedPlugin` struct
```cpp
struct LoadedPlugin {
    String filePath;
    uint8_t *codeBase;          // IRAM allocation
    size_t codeSize;
    plugin_header_t header;     // copied from binary
    plugin_activate_fn activate;
    plugin_tick_fn tick;
    plugin_deactivate_fn deactivate;
    bool valid;
};
```

### Loading flow (`loadPlugin(path)`)
1. Open SPIFFS file, check size <= `PLUGIN_MAX_SIZE` (16 KB default)
2. Read first `sizeof(plugin_header_t)` bytes, validate:
   - `magic == PLUGIN_MAGIC`
   - `abi_version == PLUGIN_ABI_VERSION`
   - `header_size == sizeof(plugin_header_t)`
   - `uuid[0] != '\0'`, `uuid[36] == '\0'` (valid UUID string)
   - all three offsets are `>= header_size`, `< fileSize`, and 4-byte aligned
3. `heap_caps_malloc(fileSize, MALLOC_CAP_EXEC)` -- if fails, log and skip
4. Read entire file into allocated buffer
5. Resolve function pointers: `activate = (plugin_activate_fn)(codeBase + header.activate_offset)` etc.
6. Store in `LoadedPlugin`, add to vector

### Discovery (`discoverAndLoad()`)
Enumerate all SPIFFS files, filter by prefix `/plugins/` and suffix `.bin`.

## PluginEffect (`src/plugin/PluginEffect.h/.cpp`)

Extends `Effect`, delegates lifecycle to loaded plugin functions:

```cpp
class PluginEffect : public Effect {
public:
    PluginEffect(const String &id, LoadedPlugin *plugin);
    void activate() override;
    void deactivate() override;
    void tick() override;
private:
    LoadedPlugin *plugin;
    void *pluginState = nullptr;  // opaque, returned by plugin's activate
};
```

- `activate()`: builds `plugin_settings_t` from `settings` + `mySettings`, calls `plugin->activate(api, &ps)`, stores returned `void*` in `pluginState`
- `tick()`: builds `plugin_settings_t`, calls `plugin->tick(api, &ps, pluginState)`
- `deactivate()`: calls `plugin->deactivate(api, pluginState)`, sets `pluginState = nullptr`
- All calls null-check `plugin` and function pointers before invocation

## EffectsManager Changes (`src/EffectsManager.cpp`)

At the end of the constructor, after all `RegisterEffect<>` calls:

```cpp
#if defined(ESP32)
#include "plugin/PluginLoader.h"
#include "plugin/PluginEffect.h"

    // In constructor, after line 325:
    PluginLoader::Initialize();
    pluginLoader->discoverAndLoad();
    for (auto *lp : pluginLoader->getPlugins()) {
        if (lp->valid) {
            String uuid(lp->header.uuid);
            if (effectsMap.count(uuid) > 0) {
                Serial.printf_P(PSTR("Plugin UUID collision, skipping: %s\n"), lp->header.uuid);
                continue;
            }
            effectsMap[uuid] = new PluginEffect(uuid, lp);
            Serial.printf_P(PSTR("Registered plugin: %s [%s]\n"), lp->header.name, lp->header.uuid);
        }
    }
#endif
```

Plugin effects are keyed by UUID. The UUID is used in `effects.json` as the `"i"` field to reference plugin effects. Collision with any existing UUID is skipped with a warning.

## Plugin Build System (`plugins/`)

### Compiler flags
```
-Os -ffreestanding -fno-builtin -nostdlib -nostartfiles
-fno-exceptions -ffunction-sections -fdata-sections
-mtext-section-literals -I../src/plugin
```

### Linker script (`plugin.ld`)
```
SECTIONS {
    . = 0;
    .plugin_header : { *(.plugin_header) }
    .text          : { *(.text .text.* .literal .literal.*) }
    .rodata        : { *(.rodata .rodata.*) }
    /DISCARD/      : { *(.comment .note.* .eh_frame* .xtensa.info) }
}
```

### Build flow per plugin
1. `xtensa-esp32-elf-gcc $CFLAGS -c sparkles_plugin.c -o build/sparkles_plugin.o`
2. `xtensa-esp32-elf-gcc $LDFLAGS build/sparkles_plugin.o -o build/sparkles_plugin.elf`
3. `xtensa-esp32-elf-objcopy -O binary build/sparkles_plugin.elf build/sparkles_plugin.bin`
4. `python3 patch_header.py build/sparkles_plugin.elf build/sparkles_plugin.bin` -- uses `nm` to find function offsets, patches them into the header
5. Validate: `readelf -r build/sparkles_plugin.elf` must show no relocations; `nm -u` must show no undefined symbols
6. Copy to `data/plugins/sparkles_plugin.bin`

### `patch_header.py`
Reads the ELF with `nm`, finds addresses of functions matching naming convention (`*_activate`, `*_tick`, `*_deactivate`). Computes byte offsets of the three `*_offset` fields in `plugin_header_t` (accounting for uuid[37] + pad[3] + version + name[48]). Writes the function addresses as little-endian uint32 into the `.bin` at those positions.

## Example Plugin (`plugins/sparkles_plugin.c`)

```c
#include "plugin_api.h"

/* State: allocated via api->alloc in activate, freed in deactivate */
typedef struct { uint8_t dummy; } sparkles_state_t;

void* sparkles_activate(const plugin_api_t *api, const plugin_settings_t *s) {
    return api->alloc(sizeof(sparkles_state_t));
}

void sparkles_tick(const plugin_api_t *api, const plugin_settings_t *s, void *state) {
    uint8_t i;
    for (i = 0; i < s->scale; i++) {
        uint8_t x = (uint8_t)api->random((uint32_t)s->width);
        uint8_t y = (uint8_t)api->random((uint32_t)s->height);
        if (api->get_pixel(x, y) == 0) {
            plugin_hsv_t hsv = {(uint8_t)api->random(255), 255, 255};
            api->draw_pixel(x, y, api->hsv_to_rgb(hsv));
        }
    }
    api->fader(70);
}

void sparkles_deactivate(const plugin_api_t *api, void *state) {
    api->free(state);
}

__attribute__((section(".plugin_header"), used))
plugin_header_t __plugin_hdr = {
    .magic = PLUGIN_MAGIC,
    .abi_version = PLUGIN_ABI_VERSION,
    .header_size = sizeof(plugin_header_t),
    .uuid = "a1b2c3d4-e5f6-7890-abcd-ef1234567890",
    ._pad_uuid = {0},
    .version = 1,
    .name = "Sparkles (Plugin)",
    .activate_offset = 0,    /* patched by build script */
    .tick_offset = 0,        /* patched by build script */
    .deactivate_offset = 0   /* patched by build script */
};
```

Plugin rules: no globals, no static data, no string literals, no external calls. All services through `api->`. State via `void*` + `api->alloc/free`.

## Implementation Order

1. **`src/plugin/plugin_api.h`** -- the shared C header (foundation for everything)
2. **`src/plugin/PluginApiImpl.h/.cpp`** -- firmware-side bridge functions
3. **`src/plugin/PluginLoader.h/.cpp`** -- binary loader + validator
4. **`src/plugin/PluginEffect.h/.cpp`** -- Effect adapter
5. **`src/EffectsManager.cpp`** -- add plugin discovery block
6. **Compile firmware** -- verify it builds for `esp32dev` with no plugins on SPIFFS
7. **`plugins/plugin.ld`** -- linker script
8. **`plugins/patch_header.py`** -- offset patcher
9. **`plugins/Makefile`** -- build system
10. **`plugins/sparkles_plugin.c`** -- first example plugin
11. **`plugins/color_plugin.c`** -- second example plugin
12. **Build plugins, validate, test end-to-end**

## Verification

1. **Firmware builds**: `pio run -e esp32dev` succeeds with no errors
2. **No plugins gracefully**: boot with empty SPIFFS `/plugins/` -- all built-in effects work unchanged
3. **Plugin builds**: `cd plugins && make` produces `.bin` files
4. **Validation passes**: `make validate` shows no relocations, no undefined symbols
5. **Plugin loads**: upload plugins to SPIFFS, boot ESP32, Serial output shows "Registered plugin: Sparkles (Plugin)"
6. **Plugin runs**: add plugin UUID to `effects.json` as the `"i"` field, cycle to it -- LED matrix displays sparkles pattern
7. **Lifecycle works**: cycle away from plugin effect (deactivate), cycle back (activate), no crashes
8. **Memory check**: Serial output shows reasonable free heap after plugin loading
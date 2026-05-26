#ifndef PLUGIN_API_H
#define PLUGIN_API_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

/*
 * Color types — C-compatible, matching CRGB/CHSV memory layout.
 */
typedef struct { uint8_t r, g, b; } plugin_rgb_t;
typedef struct { uint8_t h, s, v; } plugin_hsv_t;

/*
 * Settings snapshot — rebuilt before each plugin call.
 */
typedef struct {
    uint8_t speed;
    uint8_t scale;
    uint8_t brightness;
    uint8_t width;
    uint8_t height;
} plugin_settings_t;

/*
 * API function table (firmware -> plugin).
 * Passed to every plugin call so plugins never need globals or relocations.
 */
typedef struct plugin_api {
    uint32_t abi_version;

    /* Matrix drawing */
    void     (*draw_pixel)(uint8_t x, uint8_t y, plugin_rgb_t color);
    uint32_t (*get_pixel)(uint8_t x, uint8_t y);
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
    void     (*get_leds)(uint8_t **buf, uint16_t *count);

    /* Timing */
    uint32_t (*millis)(void);
    void     (*delay_ms)(uint32_t ms);

    /* Random */
    uint32_t (*random)(uint32_t max);
    uint32_t (*random_range)(uint32_t min, uint32_t max);

    /* Memory — plugins MUST use these, not malloc/free */
    void*    (*alloc)(size_t size);
    void     (*free)(void *ptr);

    /* Color utilities */
    plugin_rgb_t (*hsv_to_rgb)(plugin_hsv_t hsv);
    plugin_rgb_t (*color_from_palette)(uint8_t palette_id, uint8_t index, uint8_t brightness);
    uint8_t  (*inoise8_3d)(uint16_t x, uint16_t y, uint16_t z);

    /* Math helpers (integer, from FastLED) */
    uint8_t  (*scale8)(uint8_t val, uint8_t scale);
    uint8_t  (*qadd8)(uint8_t a, uint8_t b);
    uint8_t  (*qsub8)(uint8_t a, uint8_t b);
    uint8_t  (*isin8)(uint8_t theta);
    uint8_t  (*icos8)(uint8_t theta);
    int16_t  (*isin16)(uint16_t theta);
    int16_t  (*icos16)(uint16_t theta);
    uint8_t  (*ibeatsin8)(uint8_t bpm, uint8_t lo, uint8_t hi, uint32_t timebase, uint8_t phase_offset);
    uint16_t (*ibeatsin16)(uint16_t bpm, uint16_t lo, uint16_t hi, uint32_t timebase, uint16_t phase_offset);

    /* Math helpers (floating point) */
    float    (*fsinf)(float x);
    float    (*fcosf)(float x);
    float    (*fsqrtf)(float x);
    float    (*fatan2f)(float y, float x);
    float    (*ffabsf)(float x);
    float    (*ffmodf)(float x, float y);

    /* Debug logging (numeric only — no string literals in plugins) */
    void     (*log_val)(uint32_t code);
    void     (*log_val2)(uint32_t a, uint32_t b);

    /* Reserved for future expansion */
    void     (*_reserved[8])(void);
} plugin_api_t;

/*
 * Plugin header — placed at binary offset 0 via .plugin_header section.
 * Function offsets are patched by the post-build script.
 */
#define PLUGIN_MAGIC       0x4C504C47  /* "GPLP" */
#define PLUGIN_ABI_VERSION 1

typedef struct {
    uint32_t magic;
    uint32_t abi_version;
    uint32_t header_size;
    char     uuid[37];
    uint8_t  _pad_uuid[3];
    uint32_t version;
    char     name[48];
    uint32_t activate_offset;
    uint32_t tick_offset;
    uint32_t deactivate_offset;
} plugin_header_t;

/*
 * Plugin callback signatures.
 * These are never stored in the plugin binary — the firmware computes
 * them from base_address + offset at load time.
 */
typedef void* (*plugin_activate_fn)(const plugin_api_t *api, const plugin_settings_t *settings);
typedef void  (*plugin_tick_fn)(const plugin_api_t *api, const plugin_settings_t *settings, void *state);
typedef void  (*plugin_deactivate_fn)(const plugin_api_t *api, void *state);

#ifdef __cplusplus
}
#endif

#endif /* PLUGIN_API_H */

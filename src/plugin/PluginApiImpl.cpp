#include "PluginApiImpl.h"
#include "MyMatrix.h"
#include "Settings.h"

#include <Arduino.h>
#include <FastLED.h>

extern "C" {

static void api_draw_pixel(uint8_t x, uint8_t y, plugin_rgb_t c) {
    myMatrix->drawPixelXY(x, y, CRGB(c.r, c.g, c.b));
}

static uint32_t api_get_pixel(uint8_t x, uint8_t y) {
    CRGB c = myMatrix->getPixColorXY(x, y);
    return (uint32_t(c.r) << 16) | (uint32_t(c.g) << 8) | uint32_t(c.b);
}

static void api_clear(void) {
    myMatrix->clear();
}

static void api_fill(plugin_rgb_t c) {
    myMatrix->fill(CRGB(c.r, c.g, c.b));
}

static void api_dim_all(uint8_t value) {
    myMatrix->dimAll(value);
}

static void api_fader(uint8_t step) {
    myMatrix->fader(step);
}

static void api_fade_pixel(uint8_t x, uint8_t y, uint8_t step) {
    myMatrix->fadePixelXY(x, y, step);
}

static void api_draw_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, plugin_rgb_t c) {
    myMatrix->drawLineXY(x0, y0, x1, y1, CRGB(c.r, c.g, c.b));
}

static void api_fill_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, plugin_rgb_t c) {
    myMatrix->fillRectXY(x, y, w, h, CRGB(c.r, c.g, c.b));
}

static void api_blend_pixel(uint8_t x, uint8_t y, plugin_rgb_t c, uint8_t amount) {
    myMatrix->blendPixelXY(x, y, CRGB(c.r, c.g, c.b), amount);
}

static void api_blur2d(uint8_t amount) {
    myMatrix->applyBlur2d(amount);
}

static uint8_t api_wrap_x(int8_t x) {
    return myMatrix->wrapX(x);
}

static uint8_t api_wrap_y(int8_t y) {
    return myMatrix->wrapY(y);
}

static void api_get_leds(uint8_t **buf, uint16_t *count) {
    *buf = reinterpret_cast<uint8_t*>(myMatrix->getLeds());
    *count = myMatrix->getNumLeds();
}

static uint32_t api_millis(void) {
    return millis();
}

static void api_delay_ms(uint32_t ms) {
    delay(ms);
}

static uint32_t api_random(uint32_t max) {
    return random(max);
}

static uint32_t api_random_range(uint32_t min, uint32_t max) {
    return random(min, max);
}

static void* api_alloc(size_t size) {
    return malloc(size);
}

static void api_free(void *ptr) {
    free(ptr);
}

static plugin_rgb_t api_hsv_to_rgb(plugin_hsv_t hsv) {
    CRGB rgb;
    hsv2rgb_rainbow(CHSV(hsv.h, hsv.s, hsv.v), rgb);
    plugin_rgb_t result = {rgb.r, rgb.g, rgb.b};
    return result;
}

static plugin_rgb_t api_color_from_palette(uint8_t palette_id, uint8_t index, uint8_t brightness) {
    const TProgmemRGBPalette16 *pal = myMatrix->GetColorPalette(palette_id);
    CRGB c = ColorFromPalette(*pal, index, brightness);
    plugin_rgb_t result = {c.r, c.g, c.b};
    return result;
}

static uint8_t api_inoise8_3d(uint16_t x, uint16_t y, uint16_t z) {
    return inoise8(x, y, z);
}

static uint8_t api_scale8(uint8_t val, uint8_t scl) {
    return scale8(val, scl);
}

static uint8_t api_qadd8(uint8_t a, uint8_t b) {
    return qadd8(a, b);
}

static uint8_t api_qsub8(uint8_t a, uint8_t b) {
    return qsub8(a, b);
}

static void api_log_val(uint32_t code) {
    Serial.printf_P(PSTR("[plugin] %u\n"), code);
}

static void api_log_val2(uint32_t a, uint32_t b) {
    Serial.printf_P(PSTR("[plugin] %u %u\n"), a, b);
}

} /* extern "C" */

void plugin_api_init(plugin_api_t *api) {
    memset(api, 0, sizeof(plugin_api_t));

    api->abi_version = PLUGIN_ABI_VERSION;

    api->draw_pixel    = api_draw_pixel;
    api->get_pixel     = api_get_pixel;
    api->clear         = api_clear;
    api->fill          = api_fill;
    api->dim_all       = api_dim_all;
    api->fader         = api_fader;
    api->fade_pixel    = api_fade_pixel;
    api->draw_line     = api_draw_line;
    api->fill_rect     = api_fill_rect;
    api->blend_pixel   = api_blend_pixel;
    api->blur2d        = api_blur2d;
    api->wrap_x        = api_wrap_x;
    api->wrap_y        = api_wrap_y;
    api->get_leds      = api_get_leds;

    api->millis        = api_millis;
    api->delay_ms      = api_delay_ms;

    api->random        = api_random;
    api->random_range  = api_random_range;

    api->alloc         = api_alloc;
    api->free          = api_free;

    api->hsv_to_rgb       = api_hsv_to_rgb;
    api->color_from_palette = api_color_from_palette;
    api->inoise8_3d       = api_inoise8_3d;

    api->scale8        = api_scale8;
    api->qadd8         = api_qadd8;
    api->qsub8         = api_qsub8;

    api->log_val       = api_log_val;
    api->log_val2      = api_log_val2;
}

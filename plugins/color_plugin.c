#include "plugin_api.h"

typedef struct {
    uint8_t hue;
} color_state_t;

void* color_activate(const plugin_api_t *api, const plugin_settings_t *s) {
    color_state_t *st = (color_state_t *)api->alloc(sizeof(color_state_t));
    st->hue = 0;
    return st;
}

void color_tick(const plugin_api_t *api, const plugin_settings_t *s, void *state) {
    color_state_t *st = (color_state_t *)state;
    plugin_hsv_t hsv = {st->hue, 255, 255};
    plugin_rgb_t rgb = api->hsv_to_rgb(hsv);
    api->fill(rgb);
    st->hue = st->hue + 1;
}

void color_deactivate(const plugin_api_t *api, void *state) {
    api->free(state);
}

__attribute__((section(".plugin_header"), used))
plugin_header_t __plugin_hdr = {
    .magic = PLUGIN_MAGIC,
    .abi_version = PLUGIN_ABI_VERSION,
    .header_size = sizeof(plugin_header_t),
    .uuid = "b2c3d4e5-f6a7-8901-bcde-f12345678901",
    ._pad_uuid = {0},
    .version = 1,
    .name = "Color Cycle (Plugin)",
    .activate_offset = 0,
    .tick_offset = 0,
    .deactivate_offset = 0
};

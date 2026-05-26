#include "plugin_api.h"

typedef struct {
    uint8_t dummy;
} sparkles_state_t;

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
    .activate_offset = 0,
    .tick_offset = 0,
    .deactivate_offset = 0
};

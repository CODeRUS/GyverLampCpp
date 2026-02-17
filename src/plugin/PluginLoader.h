#pragma once

#include <Arduino.h>
#include <vector>
#include "plugin_api.h"

#define PLUGIN_MAX_SIZE 16384

#define pluginLoader PluginLoader::instance()

struct LoadedPlugin {
    String filePath;
    uint8_t *codeBase = nullptr;
    size_t codeSize = 0;
    plugin_header_t header;
    plugin_activate_fn activate = nullptr;
    plugin_tick_fn tick = nullptr;
    plugin_deactivate_fn deactivate = nullptr;
    bool valid = false;
};

class PluginLoader {
public:
    static PluginLoader *instance();
    static void Initialize();

    void discoverAndLoad();
    bool loadPlugin(const String &path);

    const std::vector<LoadedPlugin*>& getPlugins() const;
    const plugin_api_t* getApi() const;

protected:
    PluginLoader();

private:
    std::vector<LoadedPlugin*> plugins;
    plugin_api_t api;
};

#pragma once
#if defined(ESP32)

#include "effects/Effect.h"

struct LoadedPlugin;
class PluginLoader;

class PluginEffect : public Effect
{
public:
    PluginEffect(const String &id, LoadedPlugin *plugin);
    void activate() override;
    void deactivate() override;
    void tick() override;

private:
    LoadedPlugin *plugin;
    void *pluginState = nullptr;
};

#endif

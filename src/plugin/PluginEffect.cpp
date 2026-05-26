#include "PluginEffect.h"
#include "PluginLoader.h"

PluginEffect::PluginEffect(const String &id, LoadedPlugin *plugin)
    : Effect(id)
    , plugin(plugin)
{
    settings.name = String(plugin->header.name);
}

void PluginEffect::activate()
{
    if (!plugin || !plugin->activate) {
        return;
    }

    plugin_settings_t ps;
    ps.speed      = settings.speed;
    ps.scale      = settings.scale;
    ps.brightness = settings.brightness;
    ps.width      = mySettings->matrixSettings.width;
    ps.height     = mySettings->matrixSettings.height;

    pluginState = plugin->activate(pluginLoader->getApi(), &ps);
}

void PluginEffect::deactivate()
{
    if (!plugin || !plugin->deactivate) {
        return;
    }

    plugin->deactivate(pluginLoader->getApi(), pluginState);
    pluginState = nullptr;
}

void PluginEffect::tick()
{
    if (!plugin || !plugin->tick) {
        return;
    }

    plugin_settings_t ps;
    ps.speed      = settings.speed;
    ps.scale      = settings.scale;
    ps.brightness = settings.brightness;
    ps.width      = mySettings->matrixSettings.width;
    ps.height     = mySettings->matrixSettings.height;

    plugin->tick(pluginLoader->getApi(), &ps, pluginState);
}

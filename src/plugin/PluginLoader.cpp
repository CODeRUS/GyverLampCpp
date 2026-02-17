#if defined(ESP32)

#include "PluginLoader.h"
#include "PluginApiImpl.h"

#include <SPIFFS.h>
#define FLASHFS SPIFFS

#include <esp_attr.h>

namespace {

PluginLoader *object = nullptr;

// Simple bump allocator in IRAM for plugin executable code.
// Plugins are loaded once at boot and never freed.
#define IRAM_POOL_SIZE 32768

IRAM_ATTR __attribute__((aligned(4)))
static uint8_t iram_pool[IRAM_POOL_SIZE];

static size_t iram_offset = 0;

void* iram_alloc(size_t size)
{
    size = (size + 3) & ~3;  // 4-byte align

    if (iram_offset + size > IRAM_POOL_SIZE) {
        return nullptr;
    }

    void *ptr = &iram_pool[iram_offset];
    iram_offset += size;
    return ptr;
}

} // namespace

PluginLoader *PluginLoader::instance()
{
    return object;
}

void PluginLoader::Initialize()
{
    if (object) {
        return;
    }
    Serial.println(F("Initializing PluginLoader"));
    object = new PluginLoader();
}

PluginLoader::PluginLoader()
{
    plugin_api_init(&api);
}

void PluginLoader::discoverAndLoad()
{
    File root = FLASHFS.open("/plugins");
    if (!root || !root.isDirectory()) {
        Serial.println(F("No /plugins directory on SPIFFS"));
        return;
    }

    File file = root.openNextFile();
    while (file) {
        String path = file.name();
        file.close();
        if (!path.startsWith("/plugins/")) {
            path = String("/plugins/") + path;
        }
        if (path.endsWith(".bin")) {
            loadPlugin(path);
        }
        file = root.openNextFile();
    }

    Serial.printf_P(PSTR("Loaded %u plugin(s)\n"), plugins.size());
}

bool PluginLoader::loadPlugin(const String &path)
{
    File file = FLASHFS.open(path, "r");
    if (!file) {
        Serial.printf_P(PSTR("Failed to open plugin: %s\n"), path.c_str());
        return false;
    }

    size_t fileSize = file.size();
    if (fileSize < sizeof(plugin_header_t)) {
        Serial.printf_P(PSTR("Plugin too small: %s (%u bytes)\n"), path.c_str(), fileSize);
        file.close();
        return false;
    }

    if (fileSize > PLUGIN_MAX_SIZE) {
        Serial.printf_P(PSTR("Plugin too large: %s (%u bytes, max %u)\n"), path.c_str(), fileSize, PLUGIN_MAX_SIZE);
        file.close();
        return false;
    }

    plugin_header_t hdr;
    if (file.read(reinterpret_cast<uint8_t*>(&hdr), sizeof(hdr)) != sizeof(hdr)) {
        Serial.printf_P(PSTR("Failed to read plugin header: %s\n"), path.c_str());
        file.close();
        return false;
    }

    if (hdr.magic != PLUGIN_MAGIC) {
        Serial.printf_P(PSTR("Bad magic in plugin: %s (0x%08X)\n"), path.c_str(), hdr.magic);
        file.close();
        return false;
    }

    if (hdr.abi_version != PLUGIN_ABI_VERSION) {
        Serial.printf_P(PSTR("ABI version mismatch in plugin: %s (got %u, need %u)\n"),
                        path.c_str(), hdr.abi_version, PLUGIN_ABI_VERSION);
        file.close();
        return false;
    }

    if (hdr.header_size != sizeof(plugin_header_t)) {
        Serial.printf_P(PSTR("Header size mismatch in plugin: %s\n"), path.c_str());
        file.close();
        return false;
    }

    if (hdr.uuid[0] == '\0' || hdr.uuid[36] != '\0') {
        Serial.printf_P(PSTR("Invalid UUID in plugin: %s\n"), path.c_str());
        file.close();
        return false;
    }

    if (hdr.activate_offset < sizeof(plugin_header_t) || hdr.activate_offset >= fileSize ||
        hdr.tick_offset < sizeof(plugin_header_t) || hdr.tick_offset >= fileSize ||
        hdr.deactivate_offset < sizeof(plugin_header_t) || hdr.deactivate_offset >= fileSize) {
        Serial.printf_P(PSTR("Invalid function offsets in plugin: %s\n"), path.c_str());
        file.close();
        return false;
    }

    if ((hdr.activate_offset & 3) != 0 || (hdr.tick_offset & 3) != 0 || (hdr.deactivate_offset & 3) != 0) {
        Serial.printf_P(PSTR("Misaligned function offsets in plugin: %s\n"), path.c_str());
        file.close();
        return false;
    }

    uint8_t *codeBase = static_cast<uint8_t*>(iram_alloc(fileSize));
    if (!codeBase) {
        Serial.printf_P(PSTR("Failed to allocate IRAM for plugin: %s (%u bytes, %u/%u used)\n"),
                        path.c_str(), fileSize, iram_offset, IRAM_POOL_SIZE);
        file.close();
        return false;
    }

    file.seek(0);
    if (file.read(codeBase, fileSize) != fileSize) {
        Serial.printf_P(PSTR("Failed to read plugin binary: %s\n"), path.c_str());
        file.close();
        return false;
    }
    file.close();

    LoadedPlugin *lp = new LoadedPlugin();
    lp->filePath = path;
    lp->codeBase = codeBase;
    lp->codeSize = fileSize;
    memcpy(&lp->header, &hdr, sizeof(hdr));

    lp->activate   = reinterpret_cast<plugin_activate_fn>(codeBase + hdr.activate_offset);
    lp->tick        = reinterpret_cast<plugin_tick_fn>(codeBase + hdr.tick_offset);
    lp->deactivate  = reinterpret_cast<plugin_deactivate_fn>(codeBase + hdr.deactivate_offset);
    lp->valid = true;

    plugins.push_back(lp);
    Serial.printf_P(PSTR("Loaded plugin: %s [%s] from %s (%u bytes)\n"),
                    hdr.name, hdr.uuid, path.c_str(), fileSize);

    return true;
}

const std::vector<LoadedPlugin*>& PluginLoader::getPlugins() const
{
    return plugins;
}

const plugin_api_t* PluginLoader::getApi() const
{
    return &api;
}

#endif

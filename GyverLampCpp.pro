win32 {
    HOMEDIR += $$(USERPROFILE)
}
else {
    HOMEDIR += $$(HOME)
}

INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\include"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\src"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\src\effects"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\src\effects\basic"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\src\effects\noise"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\EEPROM\src"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\lib\GyverButton"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\ArduinoOTA\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\Update\src"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\.pio\libdeps\esp32dev\NTPClient_ID551"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\.pio\libdeps\esp32dev\FastLED_ID126"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\.pio\libdeps\esp32dev\WifiManager"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\.pio\libdeps\esp32dev\Adafruit GFX Library"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\.pio\libdeps\esp32dev\FastLED NeoMatrix"
INCLUDEPATH += "$${HOMEDIR}\workplace\GyverLamp\firmware\GyverLampCpp\.pio\libdeps\esp32dev\Framebuffer GFX"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\ESPmDNS\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\WebServer\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\FS\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\DNSServer\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\WiFi\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\config"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\app_trace"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\app_update"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\asio"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\bootloader_support"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\bt"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\coap"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\console"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\driver"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp-tls"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp32"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp_adc_cal"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp_event"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp_http_client"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp_http_server"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp_https_ota"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp_ringbuf"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\ethernet"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\expat"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\fatfs"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\freemodbus"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\freertos"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\heap"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\idf_test"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\jsmn"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\json"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\libsodium"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\log"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\lwip"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\mbedtls"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\mdns"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\micro-ecc"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\mqtt"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\newlib"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\nghttp"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\nvs_flash"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\openssl"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\protobuf-c"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\protocomm"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\pthread"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\sdmmc"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\smartconfig_ack"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\soc"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\spi_flash"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\spiffs"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\tcp_transport"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\tcpip_adapter"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\ulp"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\vfs"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\wear_levelling"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\wifi_provisioning"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\wpa_supplicant"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\xtensa-debug-module"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp32-camera"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\esp-face"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\tools\sdk\include\fb_gfx"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\cores\esp32"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\variants\doitESP32devkitV1"
INCLUDEPATH += "$${HOMEDIR}\.platformio\lib\ArduinoJson_ID64\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\AsyncUDP\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\AzureIoT\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\BLE\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\BluetoothSerial\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\ESP32\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\FFat\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\HTTPClient\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\HTTPUpdate\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\NetBIOS\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\Preferences\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\SD\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\SD_MMC\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\SPI\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\SPIFFS\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\SimpleBLE\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\Ticker\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\WiFiClientSecure\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\framework-arduinoespressif32\libraries\Wire\src"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\toolchain-xtensa32\xtensa-esp32-elf\include"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\toolchain-xtensa32\xtensa-esp32-elf\include\c++\5.2.0"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\toolchain-xtensa32\xtensa-esp32-elf\include\c++\5.2.0\xtensa-esp32-elf"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\toolchain-xtensa32\lib\gcc\xtensa-esp32-elf\5.2.0\include"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\toolchain-xtensa32\lib\gcc\xtensa-esp32-elf\5.2.0\include-fixed"
INCLUDEPATH += "$${HOMEDIR}\.platformio\packages\tool-unity"

DEFINES += "PLATFORMIO=40000"
DEFINES += "ARDUINO_ESP32_DEV"
DEFINES += "ESP32"
DEFINES += "ESP_PLATFORM"
DEFINES += "F_CPU=240000000L"
DEFINES += "HAVE_CONFIG_H"
DEFINES += "MBEDTLS_CONFIG_FILE=&quot;mbedtls/esp_config.h&quot;"
DEFINES += "ARDUINO=10805"
DEFINES += "ARDUINO_ARCH_ESP32"
DEFINES += "ARDUINO_VARIANT=&quot;doitESP32devkitV1&quot;"
DEFINES += "ARDUINO_BOARD=&quot;DOIT ESP32 DEVKIT V1&quot;"

OTHER_FILES += platformio.ini

SOURCES += \
    lib/GyverButton/*.cpp \
    src/main.cpp \
    src/GyverTimer.cpp \
    src/GyverUdp.cpp \
    src/Settings.cpp \
    src/EffectsManager.cpp \
    src/LocalDNS.cpp \
    src/MyLED.cpp \
    src/UpdateWebServer.cpp \
    src/WifiServer.cpp \
    src/effects/Effect.cpp \
    src/effects/basic/SnowEffect.cpp \
    src/effects/basic/SparklesEffect.cpp \
    src/effects/basic/VerticalRainbowEffect.cpp \
    src/effects/basic/FireEffect.cpp \
    src/effects/basic/ColorEffect.cpp \
    src/effects/basic/ColorsEffect.cpp \
    src/effects/basic/HorizontalRainbowEffect.cpp \
    src/effects/basic/LightersEffect.cpp \
    src/effects/basic/MatrixEffect.cpp \
    src/effects/noise/NoiseEffect.cpp \
    src/effects/noise/CloudNoiseEffect.cpp \
    src/effects/noise/ForestNoiseEffect.cpp \
    src/effects/noise/LavaNoiseEffect.cpp \
    src/effects/noise/MadnessNoiseEffect.cpp \
    src/effects/noise/OceanNoiseEffect.cpp \
    src/effects/noise/PlasmaNoiseEffect.cpp \
    src/effects/noise/RainbowNoiseEffect.cpp \
    src/effects/noise/RainbowStripeNoiseEffect.cpp \
    src/effects/noise/ZebraNoiseEffect.cpp

HEADERS += \
    lib/GyverButton/*.h \
    src/Settings.h \
    src/GyverTimer.h \
    src/GyverUdp.h \
    src/EffectsManager.h \
    src/FireEffect.h \
    src/LocalDNS.h \
    src/MyLED.h \
    src/UpdateWebServer.h \
    src/WifiServer.h \
    src/effects/Effect.h \
    src/effects/basic/SnowEffect.h \
    src/effects/basic/SparklesEffect.h \
    src/effects/basic/VerticalRainbowEffect.h \
    src/effects/basic/FireEffect.h \
    src/effects/basic/ColorEffect.h \
    src/effects/basic/ColorsEffect.h \
    src/effects/basic/HorizontalRainbowEffect.h \
    src/effects/basic/LightersEffect.h \
    src/effects/basic/MatrixEffect.h \
    src/effects/noise/NoiseEffect.h \
    src/effects/noise/CloudNoiseEffect.h \
    src/effects/noise/ForestNoiseEffect.h \
    src/effects/noise/LavaNoiseEffect.h \
    src/effects/noise/MadnessNoiseEffect.h \
    src/effects/noise/OceanNoiseEffect.h \
    src/effects/noise/PlasmaNoiseEffect.h \
    src/effects/noise/RainbowNoiseEffect.h \
    src/effects/noise/RainbowStripeNoiseEffect.h \
    src/effects/noise/ZebraNoiseEffect.h

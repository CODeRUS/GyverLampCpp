win32 {
    HOMEDIR += $$(USERPROFILE)
}
else {
    HOMEDIR += $$(HOME)
}

INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/include"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/src"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/lib/GyverButton"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/Update/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/ESPmDNS/src"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/arduinoFFT_ID1651/src"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/NTPClient_ID551"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/FastLED NeoMatrix"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/Framebuffer GFX"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/Adafruit GFX Library_ID13"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/FastLED_ID126"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/SPI/src"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/ESPReactWifiManager"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/SPIFFS/src"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/ESP Async WebServer/src"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/ArduinoJson_ID64/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/FS/src"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/AsyncTCP_ID1826/src"
INCLUDEPATH += "$${HOMEDIR}/workplace/GyverLamp/firmware/GyverLampCpp/.pio/libdeps/esp32dev/PubSubClient_ID89/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/DNSServer/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/WiFi/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/config"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/app_trace"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/app_update"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/asio"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/bootloader_support"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/bt"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/coap"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/console"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/driver"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp-tls"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp32"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp_adc_cal"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp_event"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp_http_client"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp_http_server"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp_https_ota"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp_ringbuf"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/ethernet"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/expat"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/fatfs"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/freemodbus"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/freertos"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/heap"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/idf_test"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/jsmn"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/json"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/libsodium"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/log"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/lwip"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/mbedtls"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/mdns"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/micro-ecc"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/mqtt"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/newlib"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/nghttp"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/nvs_flash"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/openssl"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/protobuf-c"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/protocomm"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/pthread"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/sdmmc"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/smartconfig_ack"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/soc"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/spi_flash"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/spiffs"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/tcp_transport"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/tcpip_adapter"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/ulp"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/vfs"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/wear_levelling"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/wifi_provisioning"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/wpa_supplicant"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/xtensa-debug-module"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp-face"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/esp32-camera"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/tools/sdk/include/fb_gfx"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/cores/esp32"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/variants/doitESP32devkitV1"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/ArduinoOTA/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/AsyncUDP/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/AzureIoT/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/BLE/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/BluetoothSerial/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/EEPROM/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/ESP32/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/FFat/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/HTTPClient/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/HTTPUpdate/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/NetBIOS/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/Preferences/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/SD/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/SD_MMC/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/SimpleBLE/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/Ticker/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/WebServer/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/WiFiClientSecure/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/framework-arduinoespressif32/libraries/Wire/src"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-xtensa32/xtensa-esp32-elf/include"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-xtensa32/xtensa-esp32-elf/include/c++/5.2.0"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-xtensa32/xtensa-esp32-elf/include/c++/5.2.0/xtensa-esp32-elf"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-xtensa32/lib/gcc/xtensa-esp32-elf/5.2.0/include"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/toolchain-xtensa32/lib/gcc/xtensa-esp32-elf/5.2.0/include-fixed"
INCLUDEPATH += "$${HOMEDIR}/.platformio/packages/tool-unity"

DEFINES += "PLATFORMIO=40201"
DEFINES += "ARDUINO_ESP32_DEV"
DEFINES += "CORE_DEBUG_LEVEL=0"
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

SOURCES += src/EffectsManager.cpp \
    src/effects/aurora/AuroraEffect.cpp \
    src/effects/aurora/WaveEffect.cpp \
    src/effects/basic/AnimationEffect.cpp \
    src/effects/basic/BouncingBallsEffect.cpp \
    src/effects/aurora/FlockEffect.cpp \
    src/effects/basic/Fire12Effect.cpp \
    src/effects/basic/Fire18Effect.cpp \
    src/effects/basic/LightBallsEffect.cpp \
    src/effects/basic/Matrix2Effect.cpp \
    src/effects/basic/MetaBallsEffect.cpp \
    src/effects/basic/MovingCubeEffect.cpp \
    src/effects/basic/PrismataEffect.cpp \
    src/effects/basic/PulseCirclesEffect.cpp \
    src/effects/basic/RainEffect.cpp \
    src/effects/basic/RainNeoEffect.cpp \
    src/effects/basic/SinusoidEffect.cpp \
    src/effects/basic/SpiralEffect.cpp \
    src/effects/basic/StormEffect.cpp \
    src/effects/basic/TrackingLightersEffect.cpp \
    src/effects/basic/TwinklesEffect.cpp \
    src/effects/basic/TwirlRainbowEffect.cpp \
    src/effects/basic/WaterfallPaletteEffect.cpp \
    src/effects/aurora/WhirlEffect.cpp \
    src/effects/basic/WhiteColorEffect.cpp \
    src/effects/fractional/ColorCometEffect.cpp \
    src/effects/fractional/DoubleCometsEffect.cpp \
    src/effects/fractional/FractionalEffect.cpp \
    src/effects/fractional/FractorialFireEffect.cpp \
    src/effects/fractional/MovingFlameEffect.cpp \
    src/effects/fractional/PulsingCometEffect.cpp \
    src/effects/fractional/RainbowCometEffect.cpp \
    src/effects/fractional/RainbowKiteEffect.cpp \
    src/effects/fractional/TripleCometsEffect.cpp
HEADERS += src/EffectsManager.h \
    src/effects/aurora/AuroraEffect.h \
    src/effects/aurora/Boid.h \
    src/effects/aurora/Vector.h \
    src/effects/aurora/WaveEffect.h \
    src/effects/basic/AnimationEffect.h \
    src/effects/basic/BouncingBallsEffect.h \
    src/effects/aurora/FlockEffect.h \
    src/effects/basic/Fire12Effect.h \
    src/effects/basic/Fire18Effect.h \
    src/effects/basic/LightBallsEffect.h \
    src/effects/basic/Matrix2Effect.h \
    src/effects/basic/MetaBallsEffect.h \
    src/effects/basic/MovingCubeEffect.h \
    src/effects/basic/PrismataEffect.h \
    src/effects/basic/PulseCirclesEffect.h \
    src/effects/basic/RainEffect.h \
    src/effects/basic/RainNeoEffect.h \
    src/effects/basic/SinusoidEffect.h \
    src/effects/basic/SpiralEffect.h \
    src/effects/basic/StormEffect.h \
    src/effects/basic/TrackingLightersEffect.h \
    src/effects/basic/TwinklesEffect.h \
    src/effects/basic/TwirlRainbowEffect.h \
    src/effects/basic/WaterfallPaletteEffect.h \
    src/effects/aurora/WhirlEffect.h \
    src/effects/basic/WhiteColorEffect.h \
    src/effects/fractional/ColorCometEffect.h \
    src/effects/fractional/DoubleCometsEffect.h \
    src/effects/fractional/FractionalEffect.h \
    src/effects/fractional/FractorialFireEffect.h \
    src/effects/fractional/MovingFlameEffect.h \
    src/effects/fractional/PulsingCometEffect.h \
    src/effects/fractional/RainbowCometEffect.h \
    src/effects/fractional/RainbowKiteEffect.h \
    src/effects/fractional/TripleCometsEffect.h
SOURCES += src/GyverTimer.cpp
HEADERS += src/GyverTimer.h
SOURCES += src/LampWebServer.cpp
HEADERS += src/LampWebServer.h
SOURCES += src/LocalDNS.cpp
HEADERS += src/LocalDNS.h
SOURCES += src/main.cpp
SOURCES += src/MqttClient.cpp
HEADERS += src/MqttClient.h
SOURCES += src/MyMatrix.cpp
HEADERS += src/MyMatrix.h
SOURCES += src/Settings.cpp
HEADERS += src/Settings.h
SOURCES += src/Spectrometer.cpp
HEADERS += src/Spectrometer.h
SOURCES += src/effects/Effect.cpp
HEADERS += src/effects/Effect.h
SOURCES += src/effects/basic/ClockEffect.cpp
HEADERS += src/effects/basic/ClockEffect.h
SOURCES += src/effects/basic/ClockHorizontal1Effect.cpp
HEADERS += src/effects/basic/ClockHorizontal1Effect.h
SOURCES += src/effects/basic/ClockHorizontal2Effect.cpp
HEADERS += src/effects/basic/ClockHorizontal2Effect.h
SOURCES += src/effects/basic/ClockHorizontal3Effect.cpp
HEADERS += src/effects/basic/ClockHorizontal3Effect.h
SOURCES += src/effects/basic/ColorEffect.cpp
HEADERS += src/effects/basic/ColorEffect.h
SOURCES += src/effects/basic/ColorsEffect.cpp
HEADERS += src/effects/basic/ColorsEffect.h
SOURCES += src/effects/basic/DiagonalRainbowEffect.cpp
HEADERS += src/effects/basic/DiagonalRainbowEffect.h
SOURCES += src/effects/basic/FireEffect.cpp
HEADERS += src/effects/basic/FireEffect.h
SOURCES += src/effects/basic/HorizontalRainbowEffect.cpp
HEADERS += src/effects/basic/HorizontalRainbowEffect.h
SOURCES += src/effects/basic/LightersEffect.cpp
HEADERS += src/effects/basic/LightersEffect.h
SOURCES += src/effects/basic/MatrixEffect.cpp
HEADERS += src/effects/basic/MatrixEffect.h
SOURCES += src/effects/basic/SnowEffect.cpp
HEADERS += src/effects/basic/SnowEffect.h
SOURCES += src/effects/basic/SparklesEffect.cpp
HEADERS += src/effects/basic/SparklesEffect.h
SOURCES += src/effects/basic/StarfallEffect.cpp
HEADERS += src/effects/basic/StarfallEffect.h
SOURCES += src/effects/basic/VerticalRainbowEffect.cpp
HEADERS += src/effects/basic/VerticalRainbowEffect.h
SOURCES += src/effects/noise/CloudNoiseEffect.cpp
HEADERS += src/effects/noise/CloudNoiseEffect.h
SOURCES += src/effects/noise/ForestNoiseEffect.cpp
HEADERS += src/effects/noise/ForestNoiseEffect.h
SOURCES += src/effects/noise/LavaNoiseEffect.cpp
HEADERS += src/effects/noise/LavaNoiseEffect.h
SOURCES += src/effects/noise/MadnessNoiseEffect.cpp
HEADERS += src/effects/noise/MadnessNoiseEffect.h
SOURCES += src/effects/noise/NoiseEffect.cpp
HEADERS += src/effects/noise/NoiseEffect.h
SOURCES += src/effects/noise/OceanNoiseEffect.cpp
HEADERS += src/effects/noise/OceanNoiseEffect.h
SOURCES += src/effects/noise/PlasmaNoiseEffect.cpp
HEADERS += src/effects/noise/PlasmaNoiseEffect.h
SOURCES += src/effects/noise/RainbowNoiseEffect.cpp
HEADERS += src/effects/noise/RainbowNoiseEffect.h
SOURCES += src/effects/noise/RainbowStripeNoiseEffect.cpp
HEADERS += src/effects/noise/RainbowStripeNoiseEffect.h
SOURCES += src/effects/noise/ZebraNoiseEffect.cpp
HEADERS += src/effects/noise/ZebraNoiseEffect.h
SOURCES += src/effects/sound/SoundEffect.cpp
HEADERS += src/effects/sound/SoundEffect.h
SOURCES += src/effects/sound/SoundStereoEffect.cpp
HEADERS += src/effects/sound/SoundStereoEffect.h

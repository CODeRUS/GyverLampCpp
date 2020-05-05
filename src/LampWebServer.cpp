#include "EffectsManager.h"
#include "LampWebServer.h"
#include "MyMatrix.h"
#include "Settings.h"
#include "effects/Effect.h"

#if defined(ESP32)
#include <Update.h>
#include <SPIFFS.h>
#else
#include <Updater.h>
#include <FS.h>
#endif

#ifndef U_FS
#define U_FS U_SPIFFS
#endif

#include <ESPAsyncWebServer.h>
#include <ESPReactWifiManager.h>

#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <AsyncJson.h>
#include <ArduinoJson.h>


namespace  {

size_t updateSize = 0;
bool isUpdatingFlag = false;

LampWebServer *instance = nullptr;
AsyncWebServer *webServer = nullptr;
AsyncWebSocket *socket = nullptr;
ESPReactWifiManager *wifiManager = nullptr;
bool wifiConnected = false;

void (*onConnectedCallback)(bool) = nullptr;

uint16_t httpPort = 80;

uint32_t restartTimer = 0;

void parseTextMessage(const String &message)
{
    Serial.print(F("<< "));
    Serial.println(message);

    mySettings->ProcessConfig(message);
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf_P(PSTR("ws[%s][%u] connect\n"), server->url(), client->id());
        //        client->printf("Hello Client %u :)", client->id());
        client->ping();
        lampWebServer->SendConfig(server, client);
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf_P(PSTR("ws[%s][%u] disconnect\n"), server->url(), client->id());
    } else if (type == WS_EVT_ERROR) {
        Serial.printf_P(PSTR("ws[%s][%u] error(%u): %s\n"), server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    } else if (type == WS_EVT_PONG) {
        Serial.printf_P(PSTR("ws[%s][%u] pong[%zu]: %s\n"), server->url(), client->id(), len, (len)?(char*)data:"");
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo * info = (AwsFrameInfo*)arg;
        String msg = "";
        if (info->final && info->index == 0 && info->len == len) {
            //the whole message is in a single frame and we got all of it's data
            Serial.printf_P(PSTR("ws[%s][%u] %s-message[%zu]: "),
                            server->url(),
                            client->id(),
                            info->opcode == WS_TEXT ? PSTR("text") : PSTR("binary"),
                            info->len);

            if (info->opcode == WS_TEXT) {
                for(size_t i=0; i < info->len; i++) {
                    msg += (char) data[i];
                }
            } else {
                char buff[3];
                for(size_t i=0; i < info->len; i++) {
                    sprintf(buff, "%02x ", (uint8_t) data[i]);
                    msg += buff ;
                }
            }
            Serial.println(msg);

            if (info->opcode == WS_TEXT) {
                //                client->text("I got your text message");
                parseTextMessage(msg);
            } else {
                //                client->binary("I got your binary message");
                Serial.println(F("Received binary message"));
            }
        } else {
            //message is comprised of multiple frames or the frame is split into multiple packets
            if (info->index == 0) {
                if (info->num == 0)
                    Serial.printf_P(PSTR("ws[%s][%u] %s-message start\n"),
                                    server->url(),
                                    client->id(),
                                    info->message_opcode == WS_TEXT ? PSTR("text") : PSTR("binary"));
                Serial.printf_P(PSTR("ws[%s][%u] frame[%u] start[%zu]\n"),
                                server->url(),
                                client->id(),
                                info->num,
                                info->len);
            }

            Serial.printf_P(PSTR("ws[%s][%u] frame[%u] %s[%zu - %zu]: "),
                            server->url(),
                            client->id(),
                            info->num,
                            info->message_opcode == WS_TEXT ? PSTR("text") : PSTR("binary"),
                            info->index,
                            info->index + len);

            if (info->opcode == WS_TEXT) {
                for(size_t i=0; i < len; i++) {
                    msg += (char) data[i];
                }
            } else {
                char buff[3];
                for(size_t i=0; i < len; i++) {
                    sprintf(buff, "%02x ", (uint8_t) data[i]);
                    msg += buff ;
                }
            }
            Serial.println(msg);

            if ((info->index + len) == info->len) {
                Serial.printf_P(PSTR("ws[%s][%u] frame[%u] end[%zu]\n"),
                                server->url(),
                                client->id(),
                                info->num,
                                info->len);
                if (info->final) {
                    Serial.printf(PSTR("ws[%s][%u] %s-message end\n"),
                                  server->url(),
                                  client->id(),
                                  info->message_opcode == WS_TEXT ? PSTR("text") : PSTR("binary"));
                    if (info->message_opcode == WS_TEXT) {
                        //                        client->text("I got your text message");
                        parseTextMessage(msg);
                    } else {
                        //                        client->binary("I got your binary message");
                        Serial.println(F("Received binary message"));
                    }
                }
            }
        }
    } else {
        Serial.print(F("Unknown event: "));
        Serial.printf_P(PSTR("%d, length: %zu\n"), type, len);
    }
}

void drawProgress(size_t progress)
{
    double pcs;
    if (updateSize == 0) {
        pcs = 0.5;
    } else {
        pcs = static_cast<double>(progress) / updateSize;
    }
    myMatrix->fillProgress(pcs);
}

void updateSizeHandler(AsyncWebServerRequest *request)
{
    const String fileSize = request->arg(F("fileSize"));
    updateSize = static_cast<size_t>(fileSize.toInt());
    Serial.printf_P(PSTR("Update size: %zu\n"), updateSize);
    request->send(200);
}

void updateRequestHandler(AsyncWebServerRequest *request)
{
    Serial.println(F("updateRequestHandler"));
    request->send(200);
    restartTimer = millis() + 2000;
}

void updateHandler(uint8_t *data, size_t len, size_t index, size_t total, bool final)
{
    static File settings;
    if (index == 0) {
        isUpdatingFlag = true;
        Serial.println(F("Update started!"));
        FastLED.clear();
        if (data[0] == '{') {
            if (settings) {
                settings.close();
            }
            settings = SPIFFS.open("/settings.json", "w");
            if (!settings) {
                Serial.println(F("SPIFFS Error opening settings file for write"));
                return;
            }
            Serial.println(F("Uploading settings started!"));
        } else {
            int command = U_FLASH;
            if (data[0] == 0) {
                command = U_FS;
                Serial.println(F("Uploading SPIFFS started!"));
            } else {
                Serial.println(F("Uploading FLASH started!"));
            }
            if (updateSize == 0) {
                updateSize = total;
            }
#if defined(ESP8266)
            Update.runAsync(true);
#endif
            if (!Update.begin(updateSize, command)) {
                Update.printError(Serial);
                myMatrix->fill(CRGB::Red, true);
                isUpdatingFlag = false;
                return;
            }
        }
        myMatrix->setBrightness(80);
        myMatrix->setRotation(myMatrix->GetRotation());
        myMatrix->setTextWrap(false);
    }
    drawProgress(index + len);
    if (settings) {
        settings.write(data, len);
    } else if (Update.write(data, len) != len) {
        Update.printError(Serial);
        myMatrix->fill(CRGB::Red, true);
        isUpdatingFlag = false;
        return;
    }
    if (final) {
        if (settings) {
            settings.close();
        } else if (!Update.end(true)) {
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
            isUpdatingFlag = false;
            return;
        } else {
            myMatrix->fill(CRGB::Green, true);
        }
        Serial.printf_P(PSTR("Update Success: %zd\nRebooting...\n"), index + len);
        return;
    }
#if defined(ESP8266)
    ESP.wdtFeed();
#else
    yield();
#endif
}

void updateBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    updateHandler(data, len, index, total, index + len == total);
}

void updateFileHandler(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final)
{
    updateHandler(data, len, index, request->contentLength(), final);
}

} // namespace

LampWebServer *LampWebServer::Instance()
{
    return instance;
}

void LampWebServer::Initialize(uint16_t webPort)
{
    if (instance) {
        return;
    }
#if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
    httpPort = webPort;
    Serial.printf_P(PSTR("Initializing web server at: %u\n"), webPort);
    instance = new LampWebServer(webPort);
}

bool LampWebServer::IsConnected()
{
    return wifiConnected;
}

void LampWebServer::AutoConnect()
{
    if (!webServer) {
        return;
    }

    wifiManager = new ESPReactWifiManager();
    wifiManager->setFallbackToAp(true);
    wifiManager->onFinished([](bool isAPMode) {
        webServer->begin();
        wifiConnected = !isAPMode;
        if (onConnectedCallback) {
            onConnectedCallback(wifiConnected);
        }
    });
    wifiManager->onNotFound([](AsyncWebServerRequest* request) {
        request->send(SPIFFS, F("index.html"));
    });
    wifiManager->setupHandlers(webServer);
    wifiManager->autoConnect(mySettings->connectionSettings.apName);
}

LampWebServer::LampWebServer(uint16_t webPort)
{
    webServer = new AsyncWebServer(webPort);

    socket = new AsyncWebSocket(F("/ws"));
    socket->onEvent(onWsEvent);

    webServer->addHandler(socket);

    configureHandlers();
}

void LampWebServer::Process()
{
    if (wifiManager) {
        wifiManager->loop();
    }

    if (restartTimer > 0 && (millis() > restartTimer)) {
        ESP.restart();
    }
}

void SendJsonToWs(const DynamicJsonDocument &json, AsyncWebSocketClient *client = nullptr)
{
    String buffer;
    serializeJson(json, buffer);
    if (client) {
        client->text(buffer);
    } else {
        socket->textAll(buffer);
    }
}

void LampWebServer::SendConfig(AsyncWebSocket *server, AsyncWebSocketClient *client)
{
    if (!socket) {
        return;
    }

    if (socket->count() == 0) {
        return;
    }

    DynamicJsonDocument json(512);
    JsonObject root = json.to<JsonObject>();
    root[F("activeEffect")] = effectsManager->ActiveEffectIndex();
    root[F("working")] = mySettings->generalSettings.working;
    SendJsonToWs(json, client);
}

bool LampWebServer::isUpdating()
{
    return isUpdatingFlag;
}

void LampWebServer::onConnected(void (*func)(bool))
{
    onConnectedCallback = func;
}

void LampWebServer::Update()
{
    SendConfig(socket, nullptr);
}

void LampWebServer::configureHandlers()
{
    webServer->serveStatic(PSTR("/static/js/"), SPIFFS, PSTR("/"))
        .setCacheControl(PSTR("max-age=86400"));
    webServer->serveStatic(PSTR("/static/css/"), SPIFFS, PSTR("/"))
        .setCacheControl(PSTR("max-age=86400"));
    webServer->serveStatic(PSTR("/"), SPIFFS, PSTR("/"))
        .setCacheControl(PSTR("max-age=86400"))
        .setDefaultFile(PSTR("index.html"));

    webServer->on(PSTR("/prettyJson"), HTTP_GET, [](AsyncWebServerRequest *request) {
        PrettyAsyncJsonResponse *response = new PrettyAsyncJsonResponse(false, mySettings->JsonSerializeSize());
        JsonObject root = response->getRoot();
        mySettings->BuildJson(root);
        response->setLength();
        request->send(response);
    });

    webServer->on(PSTR("/effects"), HTTP_GET, [](AsyncWebServerRequest *request) {
        AsyncJsonResponse *response = new AsyncJsonResponse(false, mySettings->JsonSerializeSize());
        JsonObject root = response->getRoot();
        mySettings->BuildEffectsJson(root);
        response->setLength();
        request->send(response);
    });

    webServer->on(PSTR("/effectJson"), HTTP_GET, [](AsyncWebServerRequest *request) {
        PrettyAsyncJsonResponse *response = new PrettyAsyncJsonResponse(false, 1024);
        JsonObject root = response->getRoot();
        mySettings->BuildJsonMqtt(root);
        response->setLength();
        request->send(response);
    });

    webServer->on(PSTR("/update"), HTTP_POST, updateRequestHandler, updateFileHandler, updateBodyHandler);
    webServer->on(PSTR("/updateSize"), HTTP_POST, updateSizeHandler);
}

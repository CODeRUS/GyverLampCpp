#include "EffectsManager.h"
#include "LampWebServer.h"
#include "MyMatrix.h"
#include "Settings.h"
#include "effects/Effect.h"

#if defined(ESP32)
#include <Update.h>
#include <SPIFFS.h>
#define FLASHFS SPIFFS
#else
#include <Updater.h>
#include <LittleFS.h>
#define FLASHFS LittleFS

extern "C" uint32_t _FS_start;
extern "C" uint32_t _FS_end;

#endif

#ifndef U_FS
#define U_FS U_SPIFFS
#endif

#include <ESPAsyncWebServer.h>
#include <ESPReactWifiManager.h>

#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <AsyncJson.h>
#include <ArduinoJson.h>
#include <Ticker.h>


namespace  {

bool setupMode = false;

size_t updateSize = 0;
bool isUpdatingFlag = false;

LampWebServer *object = nullptr;
AsyncWebServer *webServer = nullptr;
AsyncWebSocket *socket = nullptr;
ESPReactWifiManager *wifiManager = nullptr;
bool wifiConnected = false;

void (*onConnectedCallback)(bool) = nullptr;

uint16_t httpPort = 80;

uint32_t restartTimer = 0;

Ticker updateTimer;

void staticUpdate()
{
    if (!lampWebServer) {
        return;
    }

    lampWebServer->sendConfig();
}

const char upload_html[] PROGMEM = \
"<form method='POST' enctype='multipart/form-data'>\n"\
"<h1>Upload file</h1>\n"\
"<input id='file' type='file' name='update' onchange='sub(this)' style=display:none>\n"\
"<label id='file-input' for='file'>Click to choose file</label>\n"\
"<input id='btn' type='submit' class=btn value='Upload' disabled>\n"\
"</form>\n"\
"<script>\n"\
"function sub(obj) {\n"\
"    var fileName = obj.value.split('\\\\');\n"\
"    console.log(fileName);\n"\
"    document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];\n"\
"    document.getElementById('btn').disabled = false;\n"\
"};\n"\
"</script>\n"\
"<style>\n"\
"#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}\n"\
"input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}\n"\
"#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:center;display:block;cursor:pointer}\n"\
"form{background:#fff;max-width:258px;margin:75px auto;padding:10px;border-radius:5px;text-align:center}\n"\
".btn{background:#3498db;color:#fff;cursor:pointer}\n"\
".btn:disabled{background:#98342b;color:#fff;cursor:pointer}\n"\
"</style>\n";

void parseTextMessage(const String &message)
{
    mySettings->processConfig(message);
}

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf_P(PSTR("ws[%s][%u] connect\n"), server->url(), client->id());
        //        client->printf("Hello Client %u :)", client->id());
        client->ping();
        lampWebServer->sendConfig();
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf_P(PSTR("ws[%s][%u] disconnect\n"), server->url(), client->id());
    } else if (type == WS_EVT_ERROR) {
        Serial.printf_P(
            PSTR("ws[%s][%u] error(%u): %s\n"),
            server->url(),
            client->id(),
            *(reinterpret_cast<uint16_t*>(arg)),
            reinterpret_cast<char*>(data));
    } else if (type == WS_EVT_PONG) {
        Serial.printf_P(
            PSTR("ws[%s][%u] pong[%zu]: %s\n"),
            server->url(),
            client->id(),
            len,
            len ? reinterpret_cast<char*>(data) : "");
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo * info = reinterpret_cast<AwsFrameInfo*>(arg);
        String msg = "";
        if (info->final && info->index == 0 && info->len == len) {
            //the whole message is in a single frame and we got all of it's data
            Serial.printf_P(
                PSTR("ws[%s][%u] %s-message[%llu]: "),
                server->url(),
                client->id(),
                info->opcode == WS_TEXT ? PSTR("text") : PSTR("binary"),
                info->len);

            if (info->opcode == WS_TEXT) {
                for(size_t i=0; i < info->len; i++) {
                    msg += static_cast<char>(data[i]);
                }
            } else {
                char buff[3];
                for(size_t i=0; i < info->len; i++) {
                    sprintf(buff, "%02x ", data[i]);
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
                    Serial.printf_P(
                        PSTR("ws[%s][%u] %s-message start\n"),
                        server->url(),
                        client->id(),
                        info->message_opcode == WS_TEXT ? PSTR("text") : PSTR("binary"));
                Serial.printf_P(
                    PSTR("ws[%s][%u] frame[%u] start[%llu]\n"),
                    server->url(),
                    client->id(),
                    info->num,
                    info->len);
            }

            Serial.printf_P(
                PSTR("ws[%s][%u] frame[%u] %s[%llu - %llu]: "),
                server->url(),
                client->id(),
                info->num,
                info->message_opcode == WS_TEXT ? PSTR("text") : PSTR("binary"),
                info->index,
                info->index + len);

            if (info->opcode == WS_TEXT) {
                for(size_t i=0; i < len; i++) {
                    msg += static_cast<char>(data[i]);
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
                Serial.printf_P(
                    PSTR("ws[%s][%u] frame[%u] end[%llu]\n"),
                    server->url(),
                    client->id(),
                    info->num,
                    info->len);
                if (info->final) {
                    Serial.printf(
                        PSTR("ws[%s][%u] %s-message end\n"),
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
    static File json;
    if (index == 0) {
        isUpdatingFlag = true;
        Serial.println(F("Update started!"));
        Serial.printf_P(PSTR("Total size: %zu\n"), total);
        myMatrix->clear();
        if (data[0] == '{') {
            if (json) {
                json.close();
            }
            json = FLASHFS.open("/settings.json.save", "w");
            if (!json) {
                Serial.println(F("FLASHFS Error opening settings file for write"));
                return;
            }
            Serial.println(F("Uploading settings started!"));
        } else if (data[0] == '[') {
            if (json) {
                json.close();
            }
            json = FLASHFS.open("/effects.json.save", "w");
            if (!json) {
                Serial.println(F("FLASHFS Error opening effects file for write"));
                return;
            }
            Serial.println(F("Uploading effects started!"));
        } else {
            int command = U_FS;
            if (data[0] == 0xe9 || data[0] == 0x1f) {
                command = U_FLASH;
                Serial.println(F("Uploading FLASH started!"));
            } else {
                Serial.println(F("Uploading FS started!"));
            }
            if (updateSize == 0) {
#if defined(ESP32)
                updateSize = UPDATE_SIZE_UNKNOWN;
#else
                if (command == U_FS) {
                    updateSize = (uintptr_t)&_FS_end - (uintptr_t)&_FS_start;
                } else {
                    updateSize = total;
                }
#endif
            }
#if defined(ESP8266)
            Update.runAsync(true);
#endif
            if (!Update.begin(updateSize, command)) {
                Serial.print(F("Upload begin error: "));
                Update.printError(Serial);
                myMatrix->fill(CRGB::Red, true);
                isUpdatingFlag = false;
                return;
            }
        }
        myMatrix->setBrightness(80);
        myMatrix->setRotation(myMatrix->getRotation());
        myMatrix->setTextWrap(false);
    }
    drawProgress(index + len);
    if (json) {
        json.write(data, len);
    } else if (Update.write(data, len) != len) {
        Serial.print(F("Upload write error: "));
        Update.printError(Serial);
        myMatrix->fill(CRGB::Red, true);
        isUpdatingFlag = false;
        return;
    }
    if (final) {
        if (json) {
            json.close();
        } else if (!Update.end(true)) {
            Serial.print(F("Upload end error: "));
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

LampWebServer *LampWebServer::instance()
{
    return object;
}

void LampWebServer::Initialize(uint16_t webPort)
{
    if (object) {
        return;
    }
#if defined(ESP8266)
    WiFi.setSleepMode(WIFI_NONE_SLEEP);
#endif
    httpPort = webPort;
    Serial.printf_P(PSTR("Initializing web server at: %u\n"), webPort);
    object = new LampWebServer(webPort);
}

void LampWebServer::enterSetupMode()
{
    setupMode = true;
}

bool LampWebServer::isConnected()
{
    return wifiConnected;
}

void LampWebServer::autoConnect()
{
    if (!webServer) {
        return;
    }

    if (wifiManager) {
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
        if (setupMode) {
            request->redirect(String(F("http://"))
                + request->client()->localIP().toString()
                + String(F("/update")));
            return;
        }
        if (request->url() == String(F("/"))
                || request->url() == String(F("/wifi.html"))
                || request->url() == String(F("/index.html"))) {
            request->redirect(String(F("http://"))
                + request->client()->localIP().toString()
                + String(F("/update")));
        } else {
            request->send(FLASHFS, F("index.html"));
        }
    });
    wifiManager->onCaptiveRedirect([](AsyncWebServerRequest* request) {
        if (setupMode) {
            request->redirect(String(F("http://"))
                + request->client()->localIP().toString()
                + String(F("/update")));
            return true;
        }
        return false;
    });
    wifiManager->setupHandlers(webServer);
    wifiManager->setHostname(mySettings->connectionSettings.hostname);
    wifiManager->setApOptions(mySettings->connectionSettings.apName,
                              mySettings->connectionSettings.apPassword);
    wifiManager->setStaOptions(mySettings->connectionSettings.ssid,
                               mySettings->connectionSettings.password,
                               mySettings->connectionSettings.login,
                               mySettings->connectionSettings.bssid);
    wifiManager->autoConnect();
}

LampWebServer::LampWebServer(uint16_t webPort)
{
    webServer = new AsyncWebServer(webPort);

    socket = new AsyncWebSocket(F("/ws"));
    socket->onEvent(onWsEvent);

    webServer->addHandler(socket);

    webServer->rewrite(PSTR("/"), PSTR("/index.html"));
    webServer->rewrite(PSTR("/index.html"), PSTR("/index-cdn.html")).setFilter(ON_STA_FILTER);
    webServer->rewrite(PSTR("/wifi.html"), PSTR("/wifi-cdn.html")).setFilter(ON_STA_FILTER);
    webServer->serveStatic(PSTR("/static/js/"), FLASHFS, PSTR("/"), PSTR("max-age=86400"));
    webServer->serveStatic(PSTR("/static/css/"), FLASHFS, PSTR("/"), PSTR("max-age=86400"));
    webServer->serveStatic(PSTR("/effects.json"), FLASHFS, PSTR("/effects.json"), PSTR("no-cache"));
    webServer->serveStatic(PSTR("/settings.json"), FLASHFS, PSTR("/settings.json"), PSTR("no-cache"));
    webServer->serveStatic(PSTR("/"), FLASHFS, PSTR("/"), PSTR("max-age=86400"));

    webServer->on(PSTR("/effectJson"), HTTP_GET, [](AsyncWebServerRequest *request) {
        PrettyAsyncJsonResponse *response = new PrettyAsyncJsonResponse(false, 1024);
        JsonObject root = response->getRoot();
        mySettings->buildJsonMqtt(root);
        response->setLength();
        request->send(response);
    });

    webServer->on(PSTR("/reboot"), HTTP_GET, [](AsyncWebServerRequest *request) {
        if (mySettings->busy) {
            request->send(200, F("text/html"), F("Busy, try again later"));
        } else {
            request->send(200, F("text/html"), F("Rebooting"));
            restartTimer = millis() + 100;
        }
    });

    webServer->on(PSTR("/update"), HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send_P(200, F("text/html"), upload_html);
    });
    webServer->on(PSTR("/update"), HTTP_POST, updateRequestHandler, updateFileHandler, updateBodyHandler);
    webServer->on(PSTR("/updateSize"), HTTP_POST, updateSizeHandler);
}

void LampWebServer::loop()
{
    if (wifiManager) {
        wifiManager->loop();
    }

    if (restartTimer > 0 && (millis() > restartTimer)) {
        ESP.restart();
    }
}

void LampWebServer::sendConfig()
{
    if (!socket) {
        return;
    }

    if (socket->count() == 0) {
        return;
    }

    String buffer;
    {
        Serial.println(F("Sending state to ws clients"));

        DynamicJsonDocument json(512);
        JsonObject root = json.to<JsonObject>();
        root[F("activeEffect")] = effectsManager->activeEffectIndex();
        root[F("working")] = mySettings->generalSettings.working;
        serializeJson(json, buffer);

        serializeJsonPretty(json, Serial);
        Serial.println();
    }
    if (buffer.length() == 0) {
        return;
    }
    socket->textAll(buffer);
}

bool LampWebServer::isUpdating()
{
    return isUpdatingFlag;
}

void LampWebServer::onConnected(void (*func)(bool))
{
    onConnectedCallback = func;
}

void LampWebServer::update()
{
    updateTimer.once(2, staticUpdate);
}

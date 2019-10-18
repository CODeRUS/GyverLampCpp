#include "EffectsManager.h"
#include "LampWebServer.h"
#include "MyMatrix.h"
#include "Settings.h"

#if defined(ESP32)
#include <Update.h>
#include <SPIFFS.h>
#else
#include <Updater.h>
#include <FS.h>
#endif

#include <ESPAsyncWebServer.h>
#include <ESPAsyncWiFiManager.h>
#include <DNSServer.h>

#define ARDUINOJSON_ENABLE_PROGMEM 1
#include <AsyncJson.h>
#include <ArduinoJson.h>

namespace  {

size_t updateSize = 0;
bool isUpdatingFlag = false;

LampWebServer *instance = nullptr;
AsyncWebServer *webServer = nullptr;
AsyncWebSocket *socket = nullptr;
AsyncWiFiManager  *wifiManager = nullptr;
DNSServer *dnsServer = nullptr;
bool wifiConnected = false;

uint16_t httpPort = 80;

uint32_t restartTimer = 0;

void parseTextMessage(const String &message)
{
    Serial.print(F("<< "));
    Serial.println(message);

    mySettings->ProcessConfig(message);
}

String templateProcessor(const String &var)
{
    Serial.printf_P(PSTR("[TEMPLATE] %s\n"), var.c_str());
    if (var == F("ARG_HTTP_PORT")) {
        return String(httpPort);
    }
    return String();
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

void notFoundHandler(AsyncWebServerRequest *request) {
    Serial.print(F("NOT_FOUND: "));
    if (request->method() == HTTP_GET)
        Serial.print(F("GET"));
    else if (request->method() == HTTP_POST)
        Serial.print(F("POST"));
    else if (request->method() == HTTP_DELETE)
        Serial.print(F("DELETE"));
    else if (request->method() == HTTP_PUT)
        Serial.print(F("PUT"));
    else if (request->method() == HTTP_PATCH)
        Serial.print(F("PATCH"));
    else if (request->method() == HTTP_HEAD)
        Serial.print(F("HEAD"));
    else if (request->method() == HTTP_OPTIONS)
        Serial.print(F("OPTIONS"));
    else
        Serial.print(F("UNKNOWN"));
    Serial.printf_P(PSTR(" http://%s%s\n"), request->host().c_str(), request->url().c_str());

    if (request->contentLength()) {
        Serial.printf_P(PSTR("_CONTENT_TYPE: %s\n"), request->contentType().c_str());
        Serial.printf_P(PSTR("_CONTENT_LENGTH: %zu\n"), request->contentLength());
    }

    size_t headers = request->headers();
    size_t i;
    for(i=0; i<headers; i++) {
        AsyncWebHeader* h = request->getHeader(i);
        Serial.printf_P(PSTR("_HEADER[%s]: %s\n"), h->name().c_str(), h->value().c_str());
    }

    size_t params = request->params();
    for(i=0; i<params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isFile()) {
            Serial.printf_P(PSTR("_FILE[%s]: %s, size: %zu\n"), p->name().c_str(), p->value().c_str(), p->size());
        } else if (p->isPost()) {
            Serial.printf_P(PSTR("_POST[%s]: %s\n"), p->name().c_str(), p->value().c_str());
        } else {
            Serial.printf_P(PSTR("_GET[%s]: %s\n"), p->name().c_str(), p->value().c_str());
        }
    }

    if (request->url().endsWith(F(".map"))) {
        request->send(404);
    } else {
        request->send(SPIFFS, F("/index.html"), F("text/html"), false, templateProcessor);
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
                command = U_SPIFFS;
                Serial.println(F("Uploading SPIFFS started!"));
            } else {
                Serial.println(F("Uploading FLASH started!"));
            }
#if defined(ESP8266)
            Update.runAsync(true);
            if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000), command) {
#elif defined(ESP32)
            if (!Update.begin(UPDATE_SIZE_UNKNOWN, command)) {
#endif
                Update.printError(Serial);
                myMatrix->fill(CRGB::Red, true);
                isUpdatingFlag = false;
                return;
            } else {
                if (updateSize == 0) {
                    updateSize = total;
                }
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

void wifiConnectedCallback()
{
    Serial.println(F("Wifi connected!"));
    wifiConnected = true;
    Serial.print(F("Local ip: "));
    Serial.println(WiFi.localIP());

    ESP.restart();
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

    if (!dnsServer) {
        dnsServer = new DNSServer();
    }

    wifiManager = new AsyncWiFiManager(webServer, dnsServer);
    wifiManager->setSaveConfigCallback(wifiConnectedCallback);
    wifiConnected = wifiManager->tryToConnect();
    if (wifiConnected) {
        Serial.println(F("Wifi connected to saved AP!"));
        Serial.print(F("Local ip: "));
        Serial.println(WiFi.localIP());
    } else {
        Serial.println(F("Wifi not connected!"));
        wifiManager->setAPStaticIPConfig(IPAddress(8,8,8,8), IPAddress(8,8,8,8), IPAddress(255,255,255,0));
        wifiManager->startConfigPortalModeless(mySettings->connectionSettings.apName.c_str(), nullptr);
        Serial.print(F("AP ip: "));
        Serial.println(WiFi.softAPIP());
        wifiManager->loop();
    }
}

void LampWebServer::StartServer()
{
    Serial.println(F("Start WebSocket server"));

    socket = new AsyncWebSocket(F("/ws"));
    socket->onEvent(onWsEvent);

    webServer->addHandler(socket);

    configureHandlers();
    webServer->begin();
}

LampWebServer::LampWebServer(uint16_t webPort)
{
    webServer = new AsyncWebServer(webPort);
}

void LampWebServer::Process()
{
    if (!wifiConnected) {
        wifiManager->safeLoop();
//        wifiManager->criticalLoop();
    } else if (wifiManager) {
        delete wifiManager;
        wifiManager = nullptr;
        delete dnsServer;
        dnsServer = nullptr;
    }

    if (restartTimer > 0 && (millis() > restartTimer)) {
        ESP.restart();
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

    mySettings->WriteConfigTo(server, client);
}

bool LampWebServer::isUpdating()
{
    return isUpdatingFlag;
}

void LampWebServer::configureHandlers()
{
#if defined(ESP32)
    File root = SPIFFS.open("/");
    if (!root) {
        Serial.println("Error opening SPIFFS root!");
        return;
    }
    while (File file = root.openNextFile()) {
        String fileName = String(file.name());
#else
    Dir root = SPIFFS.openDir("/");
    while (root.next()) {
        String fileName = root.fileName();
#endif
        String resultName = String(fileName);
        if (resultName.endsWith(F(".css"))) {
            resultName = String(F("/static/css")) + fileName;
        } else if (resultName.endsWith(".js")) {
            resultName = String(F("/static/js")) + fileName;
        } else if (resultName.endsWith(F(".html"))) {
            resultName = resultName.substring(0, resultName.indexOf('.'));
        }
        Serial.printf_P(PSTR("Adding web handler from %s to %s\n"), resultName.c_str(), fileName.c_str());

        webServer->serveStatic(resultName.c_str(), SPIFFS, fileName.c_str());
#if defined(ESP32)
        file.close();
    }
    root.close();
#else
    }
#endif

    webServer->on(PSTR("/prettyJson"), HTTP_GET, [](AsyncWebServerRequest *request) {
        PrettyAsyncJsonResponse *response = new PrettyAsyncJsonResponse(false, 1024 * 5);
        JsonObject root = response->getRoot();
        mySettings->BuildJson(root);
        response->setLength();
        request->send(response);
    });

    webServer->on(PSTR("/update"), HTTP_POST, updateRequestHandler, updateFileHandler, updateBodyHandler);
    webServer->on(PSTR("/updateSize"), HTTP_POST, updateSizeHandler);

    if (wifiConnected) {
        webServer->on(PSTR("/"), HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, F("/index.html"), F("text/html"), false, templateProcessor);
        });

        webServer->onNotFound(notFoundHandler);
    } else {
        webServer->on(PSTR("/lamp"), HTTP_GET, [](AsyncWebServerRequest *request) {
            request->send(SPIFFS, F("/index.html"), F("text/html"), false, templateProcessor);
        });
    }
}

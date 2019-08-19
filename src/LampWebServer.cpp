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

namespace  {

size_t updateSize = 0;
bool isUpdatingFlag = false;

LampWebServer *instance = nullptr;
AsyncWebServer *webServer = nullptr;
AsyncWebServer *socketServer = nullptr;
AsyncWebSocket *socket = nullptr;

uint16_t httpPort = 80;
uint16_t websocketPort = 8000;

void parseTextMessage(const String &message)
{
    Serial.print("<< ");
    Serial.println(message);

    Settings::ApplyConfig(message);
}

String templateProcessor(const String &var)
{
    Serial.printf("[TEMPLATE] %s\n", var.c_str());
    if (var == "ARG_HTTP_PORT") {
        return String(httpPort);
    }
    if (var == "ARG_WS_PORT") {
        return String(websocketPort);
    }
    return String();
}

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len) {
    if (type == WS_EVT_CONNECT) {
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        //        client->printf("Hello Client %u :)", client->id());
        client->ping();
        client->text(Settings::GetCurrentConfig());
    } else if (type == WS_EVT_DISCONNECT) {
        Serial.printf("ws[%s][%u] disconnect\n", server->url(), client->id());
    } else if (type == WS_EVT_ERROR) {
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    } else if (type == WS_EVT_PONG) {
        Serial.printf("ws[%s][%u] pong[%zu]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo * info = (AwsFrameInfo*)arg;
        String msg = "";
        if (info->final && info->index == 0 && info->len == len) {
            //the whole message is in a single frame and we got all of it's data
            Serial.printf("ws[%s][%u] %s-message[%zu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

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
            Serial.printf("%s\n",msg.c_str());

            if (info->opcode == WS_TEXT) {
                //                client->text("I got your text message");
                parseTextMessage(msg);
            } else {
                //                client->binary("I got your binary message");
                Serial.println("Received binary message");
            }
        } else {
            //message is comprised of multiple frames or the frame is split into multiple packets
            if (info->index == 0) {
                if (info->num == 0)
                    Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                Serial.printf("ws[%s][%u] frame[%u] start[%zu]\n", server->url(), client->id(), info->num, info->len);
            }

            Serial.printf("ws[%s][%u] frame[%u] %s[%zu - %zu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

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
            Serial.printf("%s\n",msg.c_str());

            if ((info->index + len) == info->len) {
                Serial.printf("ws[%s][%u] frame[%u] end[%zu]\n", server->url(), client->id(), info->num, info->len);
                if (info->final) {
                    Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                    if (info->message_opcode == WS_TEXT) {
                        //                        client->text("I got your text message");
                        parseTextMessage(msg);
                    } else {
                        //                        client->binary("I got your binary message");
                        Serial.println("Received binary message");
                    }
                }
            }
        }
    } else {
        Serial.print("Unknown event: ");
        Serial.printf("%d, length: %zu\n", type, len);
    }
    Serial.flush();
}

void notFoundHandler(AsyncWebServerRequest *request) {
    Serial.printf("NOT_FOUND: ");
    if (request->method() == HTTP_GET)
        Serial.printf("GET");
    else if (request->method() == HTTP_POST)
        Serial.printf("POST");
    else if (request->method() == HTTP_DELETE)
        Serial.printf("DELETE");
    else if (request->method() == HTTP_PUT)
        Serial.printf("PUT");
    else if (request->method() == HTTP_PATCH)
        Serial.printf("PATCH");
    else if (request->method() == HTTP_HEAD)
        Serial.printf("HEAD");
    else if (request->method() == HTTP_OPTIONS)
        Serial.printf("OPTIONS");
    else
        Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if (request->contentLength()) {
        Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
        Serial.printf("_CONTENT_LENGTH: %zu\n", request->contentLength());
    }

    size_t headers = request->headers();
    size_t i;
    for(i=0; i<headers; i++) {
        AsyncWebHeader* h = request->getHeader(i);
        Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    size_t params = request->params();
    for(i=0; i<params; i++) {
        AsyncWebParameter* p = request->getParam(i);
        if (p->isFile()) {
            Serial.printf("_FILE[%s]: %s, size: %zu\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if (p->isPost()) {
            Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
            Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
    }

    if (request->url().endsWith(".map")) {
        request->send(404);
    } else {
        request->send(SPIFFS, "/index.html", "text/html", false, templateProcessor);
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
    const String fileSize = request->arg("fileSize");
    updateSize = static_cast<size_t>(fileSize.toInt());
    Serial.printf("Update size: %zu\n", updateSize);
    request->send(200);
}

void updateRequestHandler(AsyncWebServerRequest *request)
{
    Serial.println("updateRequestHandler");
    request->send(200);
#if defined(ESP8266)
    ESP.wdtFeed();
#else
    yield();
#endif
    ESP.restart();
}

void updateBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    if (index == 0) {
        Serial.println("Update started!");
#if defined(ESP8266)
        myMatrix->fill(CRGB(40, 40, 60), true);
        Update.runAsync(true);
        if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
#elif defined(ESP32)
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
#endif
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
            isUpdatingFlag = false;
            return;
        } else {
            isUpdatingFlag = true;
#if defined(ESP32)
            myMatrix->setRotation(3);
            myMatrix->setTextColor(myMatrix->Color(40, 0, 00));
            myMatrix->setBrightness(80);
#endif

            if (updateSize == 0) {
                updateSize = total;
            }
        }
    }
    if (Update.write(data, len) != len) {
        Update.printError(Serial);
        myMatrix->fill(CRGB::Red, true);
        isUpdatingFlag = false;
    } else {
#if defined(ESP32)
        drawProgress(index + len);
#endif
    }
    if (index + len == total) {
        if (!Update.end(true)) {
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
            isUpdatingFlag = false;
        } else {
            Serial.printf("Update Success: %zd\nRebooting...\n", index + len);
            myMatrix->fill(CRGB::Green, true);
        }
    }
#if defined(ESP8266)
    ESP.wdtFeed();
#else
    yield();
#endif
}

void updateFileHandler(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final)
{
    if (!index) {
        Serial.println("Update started!");
#if defined(ESP8266)
        myMatrix->fill(CRGB(40, 40, 60), true);
        Update.runAsync(true);
        if (!Update.begin((ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000)) {
#elif defined(ESP32)
        if (!Update.begin(UPDATE_SIZE_UNKNOWN, U_FLASH)) {
#endif
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
            isUpdatingFlag = false;
            return;
        } else {
            isUpdatingFlag = true;
#if defined(ESP32)
            myMatrix->setRotation(3);
            myMatrix->setTextColor(myMatrix->Color(40, 0, 00));
            myMatrix->setBrightness(80);
#endif

            if (updateSize == 0) {
                updateSize = request->contentLength();
            }
        }
    }

    if (Update.write(data, len) != len) {
        Update.printError(Serial);
        myMatrix->fill(CRGB::Red, true);
        isUpdatingFlag = false;
        return;
    } else {
#if defined(ESP32)
        drawProgress(index + len);
#endif
    }

    if (final) {
        if (!Update.end(true)) {
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
            isUpdatingFlag = false;
            return;
        } else {
            Serial.printf("Update Success: %zd\nRebooting...\n", index + len);
            myMatrix->fill(CRGB::Green, true);
        }
    }

#if defined(ESP8266)
    ESP.wdtFeed();
#else
    yield();
#endif
}

} // namespace

LampWebServer *LampWebServer::Instance()
{
    return instance;
}

void LampWebServer::Initialize(uint16_t webPort, uint16_t wsPort)
{
    if (instance) {
        return;
    }

    httpPort = webPort;
    websocketPort = wsPort;
    instance = new LampWebServer(webPort, wsPort);
}

LampWebServer::LampWebServer(uint16_t webPort, uint16_t wsPort)
{
    webServer = new AsyncWebServer(webPort);

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
        if (resultName.endsWith(".css")) {
            resultName = String("/static/css") + fileName;
        } else if (resultName.endsWith(".js")) {
            resultName = String("/static/js") + fileName;
        } else if (resultName.endsWith(".html")) {
            resultName = resultName.substring(0, resultName.indexOf('.'));
        }
        Serial.printf("Adding web handler from %s to %s\n", resultName.c_str(), fileName.c_str());

        webServer->serveStatic(resultName.c_str(), SPIFFS, fileName.c_str());
    }

    webServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(SPIFFS, "/index.html", "text/html", false, templateProcessor);
    });

    webServer->on("/update", HTTP_POST, updateRequestHandler, updateFileHandler, updateBodyHandler);
    webServer->on("/updateSize", HTTP_POST, updateSizeHandler);

    webServer->onNotFound(notFoundHandler);

    webServer->begin();

    Serial.println("Start WebSocket server");

    socket = new AsyncWebSocket("/");
    socket->onEvent(onWsEvent);

    socketServer = new AsyncWebServer(wsPort);
    socketServer->addHandler(socket);
    socketServer->begin();
}

void LampWebServer::Process()
{
}

void LampWebServer::SendConfig()
{
    if (!socket) {
        return;
    }

    if (socket->count() == 0) {
        return;
    }

    socket->textAll(Settings::GetCurrentConfig());
}

bool LampWebServer::isUpdating()
{
    return isUpdatingFlag;
}

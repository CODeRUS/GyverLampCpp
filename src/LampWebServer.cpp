#include "LampWebServer.h"

#include <SPIFFS.h>
#include <ESPAsyncWebServer.h>

namespace  {

LampWebServer *instance = nullptr;
AsyncWebServer *webServer = nullptr;
AsyncWebServer *socketServer = nullptr;
AsyncWebSocket *socket = nullptr;

void onWsEvent(AsyncWebSocket * server, AsyncWebSocketClient * client, AwsEventType type, void * arg, uint8_t *data, size_t len){
    if(type == WS_EVT_CONNECT){
        Serial.printf("ws[%s][%u] connect\n", server->url(), client->id());
        client->printf("Hello Client %u :)", client->id());
        client->ping();
    } else if(type == WS_EVT_DISCONNECT){
        Serial.printf("ws[%s][%u] disconnect: %u\n", server->url(), client->id());
    } else if(type == WS_EVT_ERROR){
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t*)arg), (char*)data);
    } else if(type == WS_EVT_PONG){
        Serial.printf("ws[%s][%u] pong[%u]: %s\n", server->url(), client->id(), len, (len)?(char*)data:"");
    } else if(type == WS_EVT_DATA){
        AwsFrameInfo * info = (AwsFrameInfo*)arg;
        String msg = "";
        if(info->final && info->index == 0 && info->len == len){
            //the whole message is in a single frame and we got all of it's data
            Serial.printf("ws[%s][%u] %s-message[%llu]: ", server->url(), client->id(), (info->opcode == WS_TEXT)?"text":"binary", info->len);

            if(info->opcode == WS_TEXT){
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

            if(info->opcode == WS_TEXT)
                client->text("I got your text message");
            else
                client->binary("I got your binary message");
        } else {
            //message is comprised of multiple frames or the frame is split into multiple packets
            if(info->index == 0){
                if(info->num == 0)
                    Serial.printf("ws[%s][%u] %s-message start\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                Serial.printf("ws[%s][%u] frame[%u] start[%llu]\n", server->url(), client->id(), info->num, info->len);
            }

            Serial.printf("ws[%s][%u] frame[%u] %s[%llu - %llu]: ", server->url(), client->id(), info->num, (info->message_opcode == WS_TEXT)?"text":"binary", info->index, info->index + len);

            if(info->opcode == WS_TEXT){
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

            if((info->index + len) == info->len){
                Serial.printf("ws[%s][%u] frame[%u] end[%llu]\n", server->url(), client->id(), info->num, info->len);
                if(info->final){
                    Serial.printf("ws[%s][%u] %s-message end\n", server->url(), client->id(), (info->message_opcode == WS_TEXT)?"text":"binary");
                    if(info->message_opcode == WS_TEXT)
                        client->text("I got your text message");
                    else
                        client->binary("I got your binary message");
                }
            }
        }
    }
}

void notFoundHandler(AsyncWebServerRequest *request){
    Serial.printf("NOT_FOUND: ");
    if(request->method() == HTTP_GET)
        Serial.printf("GET");
    else if(request->method() == HTTP_POST)
        Serial.printf("POST");
    else if(request->method() == HTTP_DELETE)
        Serial.printf("DELETE");
    else if(request->method() == HTTP_PUT)
        Serial.printf("PUT");
    else if(request->method() == HTTP_PATCH)
        Serial.printf("PATCH");
    else if(request->method() == HTTP_HEAD)
        Serial.printf("HEAD");
    else if(request->method() == HTTP_OPTIONS)
        Serial.printf("OPTIONS");
    else
        Serial.printf("UNKNOWN");
    Serial.printf(" http://%s%s\n", request->host().c_str(), request->url().c_str());

    if(request->contentLength()){
        Serial.printf("_CONTENT_TYPE: %s\n", request->contentType().c_str());
        Serial.printf("_CONTENT_LENGTH: %u\n", request->contentLength());
    }

    int headers = request->headers();
    int i;
    for(i=0;i<headers;i++){
        AsyncWebHeader* h = request->getHeader(i);
        Serial.printf("_HEADER[%s]: %s\n", h->name().c_str(), h->value().c_str());
    }

    int params = request->params();
    for(i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isFile()){
            Serial.printf("_FILE[%s]: %s, size: %u\n", p->name().c_str(), p->value().c_str(), p->size());
        } else if(p->isPost()){
            Serial.printf("_POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        } else {
            Serial.printf("_GET[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
    }

    // request->send(404);
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
    ESP.restart();
}

void updateBodyHandler(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t index, size_t total)
{
    Serial.printf("updateBodyHandler len: %zu index: %zu total: %zu\n", len, index, total);
}

void updateFileHandler(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final)
{
    uint32_t free_space = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
    if (!index){
        Serial.println("Update started!");
        if (!Update.begin(free_space)) {
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
            isUpdatingFlag = false;
        } else {
            isUpdatingFlag = true;
            myMatrix->setRotation(3);
            myMatrix->setTextColor(myMatrix->Color(40, 0, 00));
            myMatrix->setBrightness(80);
        }
    } else {
//        Serial.printf("Updating: %zu size: %zu\n", index, len);
    }

    if (Update.write(data, len) != len) {
        Update.printError(Serial);
        myMatrix->fill(CRGB::Red, true);
        isUpdatingFlag = false;
    } else {
        drawProgress(index + len);
    }

    if (final) {
        if (!Update.end(true)){
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
            isUpdatingFlag = false;
        } else {
            Serial.printf("Update Success: %zd\nRebooting...\n", index + len);
            myMatrix->fill(CRGB::Green, true);
        }
    }
    yield();
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

    instance = new LampWebServer(webPort, wsPort);
}

LampWebServer::LampWebServer(uint16_t webPort, uint16_t wsPort)
{
    webServer = new AsyncWebServer(webPort);

    webServer->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/index.html", "text/html");
    });

    webServer->on("/static/css/2.266e55a5.chunk.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/2.266e55a5.chunk.css", "text/css");
    });

    webServer->on("/static/css/main.2cce8147.chunk.css", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/main.2cce8147.chunk.css", "text/css");
    });

    webServer->on("/static/js/2.22103a46.chunk.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/2.22103a46.chunk.js", "text/javascript");
    });

    webServer->on("/static/js/main.4beb7e06.chunk.js", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/main.4beb7e06.chunk.js", "text/javascript");
    });

    webServer->on("/favicon.ico", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(SPIFFS, "/favicon.ico", "image/x-icon");
    });

    webServer->on("/update", HTTP_POST, updateRequestHandler, updateFileHandler, updateBodyHandler);
    webServer->on("/updateSize", HTTP_POST, updateSizeHandler);
    //file:///C:/Users/coderus/workplace/GyverLamp/firmware/GyverLampCpp/data/precache-manifest.js
    //file:///C:/Users/coderus/workplace/GyverLamp/firmware/GyverLampCpp/data/runtime~main.a8a9905a.js


    webServer->onNotFound(notFoundHandler);

    webServer->begin();

    Serial.println("Start WebSocket server");

    socket = new AsyncWebSocket("/");
    socket->onEvent(onWsEvent);

    socketServer = new AsyncWebServer(wsPort);
    socketServer->addHandler(socket);
    socketServer->onNotFound(notFoundHandler);
    socketServer->begin();
}

void LampWebServer::Process()
{
}

bool LampWebServer::isUpdating()
{
    return isUpdatingFlag;
}

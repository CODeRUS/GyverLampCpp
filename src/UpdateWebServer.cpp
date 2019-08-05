#include "UpdateWebServer.h"
#if defined(ESP8266)
#include <ESP8266WebServer.h>
#else
#include <WebServer.h>
#endif
#include <Update.h>

#include "MyMatrix.h"

namespace  {
/* Style */
String cssStyle =
        "<style>#file-input,input{width:100%;height:44px;border-radius:4px;margin:10px auto;font-size:15px}"
        "input{background:#f1f1f1;border:0;padding:0 15px}body{background:#3498db;font-family:sans-serif;font-size:14px;color:#777}"
        "#file-input{padding:0;border:1px solid #ddd;line-height:44px;text-align:left;display:block;cursor:pointer}"
        "#bar,#prgbar{background-color:#f1f1f1;border-radius:10px}#bar{background-color:#3498db;width:0%;height:10px}"
        "form{background:#fff;max-width:258px;margin:75px auto;padding:30px;border-radius:5px;text-align:center}"
        ".btn{background:#3498db;color:#fff;cursor:pointer}</style>";

/* Server Index Page */
String updateHtml =
        "<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
        "<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
        "<input type='file' name='update' id='file' onchange='sub(this)' style=display:none>"
        "<label id='file-input' for='file'>   Choose file...</label>"
        "<input id='btn' type='submit' class=btn value='Update' disabled>"
        "<br><br>"
        "<div id='prg'></div>"
        "<br><div id='prgbar'><div id='bar'></div></div><br></form>"
        "<script>"
        "function sub(obj){"
        "var fileName = obj.value.split('\\\\');"
        "document.getElementById('file-input').innerHTML = '   '+ fileName[fileName.length-1];"

        "var fs = file.files[0].size;"
        "var xhttp = new XMLHttpRequest();"
        "xhttp.onreadystatechange = function(){"
        "if(xhttp.readyState == 4 && xhttp.status == 200){"
        // ESP received fileSize enable the submit button
        "document.getElementById('btn').disabled = false;"
        "}"
        "};"
        "xhttp.open('POST', '/updateSize', true);"
        "xhttp.setRequestHeader('Content-type', 'application/x-www-form-urlencoded');"
        "xhttp.send('fileSize='+fs);"

        "};"
        "$('form').submit(function(e){"
        "e.preventDefault();"
        "var form = $('#upload_form')[0];"
        "var data = new FormData(form);"
        "$.ajax({"
        "url: '/update',"
        "type: 'POST',"
        "data: data,"
        "contentType: false,"
        "processData:false,"
        "xhr: function() {"
        "var xhr = new window.XMLHttpRequest();"
        "xhr.upload.addEventListener('progress', function(evt) {"
        "if (evt.lengthComputable) {"
        "var per = evt.loaded / evt.total;"
        "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
        "$('#bar').css('width',Math.round(per*100) + '%');"
        "}"
        "}, false);"
        "return xhr;"
        "},"
        "success:function(d, s) {"
        "console.log('success!') "
        "},"
        "error: function (a, b, c) {"
        "}"
        "});"
        "});"
        "</script>" + cssStyle;

WebServer *webServer = nullptr;

size_t updateSize = 0;
bool isUpdating = false;

void updateGetRequest()
{
    webServer->sendHeader("Connection", "close");
    webServer->send(200, "text/html", updateHtml);
}

void updateSizeRequest()
{
    const String fileSize = webServer->arg("fileSize");
    updateSize = static_cast<size_t>(fileSize.toInt());
    Serial.printf("Update size: %zu\n", updateSize);
    webServer->send(200, "text/html", "ok");
}

void updatePostRequest()
{
    webServer->sendHeader("Connection", "close");
    webServer->send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
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

void updatePostData()
{
    HTTPUpload& upload = webServer->upload();
    if (upload.status == UPLOAD_FILE_START) {
        isUpdating = true;
        myMatrix->clear();
        Serial.printf("Update: %s\n", upload.filename.c_str());
        if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
        }
        myMatrix->setRotation(3);
        myMatrix->setTextColor(myMatrix->Color(40, 0, 00));
        myMatrix->setBrightness(80);
        yield();
    } else if (upload.status == UPLOAD_FILE_WRITE) {
        /* flashing firmware to ESP*/
        if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
        } else {
//            Serial.printf("Uploaded: %zd\n", upload.totalSize);
            drawProgress(upload.totalSize);
        }
        yield();
    } else if (upload.status == UPLOAD_FILE_END) {
        if (Update.end(true)) { //true to set the size to the current progress
            Serial.printf("Update Success: %zd\nRebooting...\n", upload.totalSize);
            myMatrix->fill(CRGB::Green, true);
        } else {
            Update.printError(Serial);
            myMatrix->fill(CRGB::Red, true);
        }
        yield();
    }
}

} // namespace

void UpdateWebServer::Initialize(int port)
{
    if (webServer) {
        delete webServer;
    }

    webServer = new WebServer(port);
    webServer->on("/update", HTTP_GET, updateGetRequest);
    webServer->on("/update", HTTP_POST, updatePostRequest, updatePostData);
    webServer->on("/updateSize", HTTP_POST, updateSizeRequest);
    webServer->begin();
}

void UpdateWebServer::Process()
{
    if (webServer) {
        webServer->handleClient();
    }
}

bool UpdateWebServer::IsUpdating()
{
    return isUpdating;
}

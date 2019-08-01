#include "GyverUdp.h"
#include "Settings.h"
#include "GyverTimer.h"
#include "EffectsManager.h"
#include "MyMatrix.h"

#include <WiFiUdp.h>

#ifndef UDP_TX_PACKET_MAX_SIZE
#define UDP_TX_PACKET_MAX_SIZE 15
#endif

namespace  {

WiFiUDP *udp = nullptr;

void sendString(const String &inputBuffer) {
    Serial.print(">> ");
    Serial.println(inputBuffer);

    udp->beginPacket(udp->remoteIP(), udp->remotePort());
  #if defined(ESP8266)
    udp->write(reply);
    char* reply = new char[inputBuffer.length() + 1]();
    inputBuffer.toCharArray(reply, inputBuffer.length() + 1);
  #else
    udp->print(inputBuffer);
  #endif
    udp->endPacket();
}

void sendCurrent() {
    const uint8_t currentMode = Settings::currentEffect;
    const Settings::EffectSettings modeSettings = *Settings::CurrentEffectSettings();

    String inputBuffer = "CURR";
    inputBuffer += " ";
    inputBuffer += String(currentMode);
    inputBuffer += " ";
    inputBuffer += String(modeSettings.effectBrightness);
    inputBuffer += " ";
    inputBuffer += String(modeSettings.effectSpeed);
    inputBuffer += " ";
    inputBuffer += String(modeSettings.effectScale);
    inputBuffer += " ";
    inputBuffer += String(Settings::masterSwitch);

    sendString(inputBuffer);
}

void parseUDP() {
    char packetBuffer[UDP_TX_PACKET_MAX_SIZE + 1]; //buffer to hold incoming packet
    String inputBuffer;

    int packetSize = udp->parsePacket();
    if (packetSize <= 0) {
        return;
    }

    int n = udp->read(packetBuffer, UDP_TX_PACKET_MAX_SIZE);
    if (n <= 0) {
        return;
    }

    packetBuffer[n] = 0;
    inputBuffer = packetBuffer;

    Serial.print("<< ");
    Serial.println(inputBuffer);

    if (inputBuffer.startsWith("DEB")) {
        inputBuffer = "OK " + GyverTimer::FormattedTime();
        sendString(inputBuffer);
    } else if (inputBuffer.startsWith("GET")) {
        sendCurrent();
    } else if (inputBuffer.startsWith("EFF")) {
        EffectsManager::ChangeEffect(static_cast<uint8_t>(inputBuffer.substring(3).toInt()));
        sendCurrent();
    } else if (inputBuffer.startsWith("BRI")) {
        const uint8_t brightness = static_cast<uint8_t>(inputBuffer.substring(3).toInt());
        Settings::CurrentEffectSettings()->effectBrightness = brightness;
        myMatrix->setBrightness(brightness);
        Settings::SaveLater();
    } else if (inputBuffer.startsWith("SPD")) {
        Settings::CurrentEffectSettings()->effectSpeed = static_cast<uint8_t>(inputBuffer.substring(3).toInt());
        Settings::SaveLater();
    } else if (inputBuffer.startsWith("SCA")) {
        Settings::CurrentEffectSettings()->effectScale = static_cast<uint8_t>(inputBuffer.substring(3).toInt());
        Settings::SaveLater();
    } else if (inputBuffer.startsWith("P_ON")) {
        Settings::masterSwitch = true;
        sendCurrent();
    } else if (inputBuffer.startsWith("P_OFF")) {
        myMatrix->clear();
        myMatrix->show();
        Settings::masterSwitch = false;
        sendCurrent();
    } else if (inputBuffer.startsWith("ALM_SET")) {
//        byte alarmNum = (char)inputBuffer[7] - '0';
//        alarmNum -= 1;
//        if (inputBuffer.indexOf("ON") != -1) {
//            my_alarm[alarmNum].state = true;
//            inputBuffer = "alm #" + String(alarmNum + 1) + " ON";
//        } else if (inputBuffer.indexOf("OFF") != -1) {
//            my_alarm[alarmNum].state = false;
//            inputBuffer = "alm #" + String(alarmNum + 1) + " OFF";
//        } else {
//            int almTime = inputBuffer.substring(8).toInt();
//            my_alarm[alarmNum].time = almTime;
//            byte hour = floor(almTime / 60);
//            byte minute = almTime - hour * 60;
//            inputBuffer = "alm #" + String(alarmNum + 1) +
//                    " " + String(hour) +
//                    ":" + String(minute);
//        }
//        saveAlarm(alarmNum);
    } else if (inputBuffer.startsWith("ALM_GET")) {
//        sendAlarms();
    } else if (inputBuffer.startsWith("DAWN")) {
//        dawnMode = inputBuffer.substring(4).toInt() - 1;
//        saveDawnMmode();
    }
}

void sendAlarms() {
    String inputBuffer = "ALMS ";
//    for (byte i = 0; i < 7; i++) {
//        inputBuffer += String(my_alarm[i].state);
//        inputBuffer += " ";
//    }
//    for (byte i = 0; i < 7; i++) {
//        inputBuffer += String(my_alarm[i].time);
//        inputBuffer += " ";
//    }
//    inputBuffer += (dawnMode + 1);

//    sendString(inputBuffer);
}

} // namespace

void GyverUdp::Initiazlize(uint16_t port)
{
    udp = new WiFiUDP;
    udp->begin(port);
}

void GyverUdp::Process()
{
    parseUDP();
}

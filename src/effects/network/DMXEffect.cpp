#include "DMXEffect.h"
#include "MyMatrix.h"
#include "Settings.h"
#include "LampWebServer.h"
#include <ESPAsyncE131.h>

namespace {

uint8_t universeCount = 1;
ESPAsyncE131 *e131 = nullptr;

uint8_t* e131LastSequenceNumber = nullptr;       // to detect packet loss (9)

// settings

uint16_t e131Universe = 1;                       // settings for E1.31 (sACN) protocol (only DMX_MODE_MULTIPLE_* can span over consequtive universes)
uint16_t e131Port = 6454;                        // DMX in port. E1.31 default is 5568, Art-Net is 6454
uint16_t DMXAddress = 1;                         // DMX start address of fixture, a.k.a. first Channel [for E1.31 (sACN) protocol]
bool e131Multicast = true;                    // multicast or unicast
bool e131SkipOutOfSequence = true;            // freeze instead of flickering
bool fixUniverse = true;                         // round number of leds in one universe to rows/columns

void handleE131Packet(e131_packet_t* p, const IPAddress &clientIP, bool isArtnet)
{
    //E1.31 protocol support

    if (!mySettings->generalSettings.working || lampWebServer->isUpdating()) {
        return;
    }

    uint16_t uni = 0, dmxChannels = 0;
    uint8_t* e131_data = nullptr;
    uint8_t seq = 0;

    if (isArtnet) {
        uni = p->art_universe;
        dmxChannels = htons(p->art_length);
        e131_data = p->art_data;
        seq = p->art_sequence_number;
    } else {
        uni = htons(p->universe);
        dmxChannels = htons(p->property_value_count) -1;
        e131_data = p->property_values;
        seq = p->sequence_number;
    }

    // only listen for universes we're handling & allocated memory
    if (uni >= (e131Universe + universeCount)) {
        return;
    }

    uint16_t previousUniverses = uni - e131Universe;
    uint16_t possibleLEDsInCurrentUniverse;

    if (e131SkipOutOfSequence)
        if (seq < e131LastSequenceNumber[uni - e131Universe]
                && seq > 20
                && e131LastSequenceNumber[uni - e131Universe] < 250){
            Serial.print(F("skipping E1.31 frame (last seq="));
            Serial.print(e131LastSequenceNumber[uni-e131Universe]);
            Serial.print(F(", current seq="));
            Serial.print(seq);
            Serial.print(F(", universe="));
            Serial.print(uni);
            Serial.println(F(")"));
            return;
        }
    e131LastSequenceNumber[uni-e131Universe] = seq;

    if (previousUniverses == 0) {
        // first universe of this fixture
        possibleLEDsInCurrentUniverse = (dmxChannels - DMXAddress + 1) / 3;
        if (fixUniverse) {
            possibleLEDsInCurrentUniverse = possibleLEDsInCurrentUniverse / myMatrix->getDimension() * myMatrix->getDimension();
        }
        uint16_t count = min(possibleLEDsInCurrentUniverse, myMatrix->getNumLeds());
        memcpy(myMatrix->getLeds(),
               &e131_data[DMXAddress],
               count * 3);
    } else if (previousUniverses > 0 && uni < (e131Universe + universeCount)) {
        // additional universe(s) of this fixture
        uint16_t numberOfLEDsInPreviousUniverses = ((512 - DMXAddress + 1) / 3);     // first universe
        if (fixUniverse) {
            numberOfLEDsInPreviousUniverses = numberOfLEDsInPreviousUniverses / myMatrix->getDimension() * myMatrix->getDimension();
        }
        if (previousUniverses > 1) {
            numberOfLEDsInPreviousUniverses += (512 / 3) * (previousUniverses - 1);  // extended universe(s) before current
        }
        possibleLEDsInCurrentUniverse = dmxChannels / 3;
        uint16_t remainingLeds = myMatrix->getNumLeds() - numberOfLEDsInPreviousUniverses;
        uint16_t count = min(possibleLEDsInCurrentUniverse, remainingLeds);
        memcpy(&myMatrix->getLeds()[numberOfLEDsInPreviousUniverses],
               &e131_data[DMXAddress],
               count * 3);
    }

//    myMatrix->show();
}

}

DMXEffect::DMXEffect()
{

}

void DMXEffect::activate()
{
    universeCount = ceil(myMatrix->getNumLeds() * 3 / 512.0);
    e131LastSequenceNumber = new uint8_t[universeCount];
    e131 = new ESPAsyncE131(&handleE131Packet);
    e131->begin(e131Multicast, e131Port, e131Universe, universeCount);
}

void DMXEffect::deactivate()
{
    delete[] e131LastSequenceNumber;
    delete e131;
}

void DMXEffect::tick()
{

}

void DMXEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("uni"))) {
        e131Universe = json[F("uni")];
    }
    if (json.containsKey(F("port"))) {
        e131Port = json[F("port")];
    }
    if (json.containsKey(F("cha"))) {
        DMXAddress = json[F("cha")];
    }
    if (json.containsKey(F("mcast"))) {
        e131Multicast = json[F("mcast")];
    }
    if (json.containsKey(F("skip"))) {
        e131SkipOutOfSequence = json[F("skip")];
    }
    if (json.containsKey(F("fix"))) {
        fixUniverse = json[F("fix")];
    }
}

void DMXEffect::writeSettings(JsonObject &json)
{
    json[F("uni")] = e131Universe;
    json[F("port")] = e131Port;
    json[F("cha")] = DMXAddress;
    json[F("mcast")] = e131Multicast;
    json[F("skip")] = e131SkipOutOfSequence;
    json[F("fix")] = fixUniverse;
}

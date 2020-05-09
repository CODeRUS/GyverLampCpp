#include "RainNeoEffect.h"

namespace {

uint8_t** tempMatrix = nullptr;
uint8_t* splashArray = nullptr;

uint16_t noiseX = random16();
uint16_t noiseY = random16();
uint16_t noiseZ = random16();
CRGBPalette16 rainClouds_p( CRGB::Black, CRGB(15,24,24), CRGB(9,15,15), CRGB::Black );

uint8_t hue = 0;

// settigns
uint32_t rainColor = 0x3c505a;
uint32_t lightningColor = 0x484850;

byte backgroundDepth = 60;
byte maxBrightness = 200;
byte spawnFreq = 50;
byte tailLength = 30;
bool splashes = false;
bool clouds = false;
bool storm = false;

void rain()
{
    CRGBPalette16 rain_p( CRGB::Black, CRGB(rainColor) );

    myMatrix->fadeToBlackBy(255 - tailLength);

    // Loop for each column individually
    for (int x = 0; x < mySettings->matrixSettings.width; x++) {
        // Step 1.  Move each dot down one cell
        for (int i = 0; i < mySettings->matrixSettings.height; i++) {
            if (tempMatrix[x][i] >= backgroundDepth) {  // Don't move empty cells
                if (i > 0) tempMatrix[x][myMatrix->wrapY(i - 1)] = tempMatrix[x][i];
                tempMatrix[x][i] = 0;
            }
        }

        // Step 2.  Randomly spawn new dots at top
        if (random8() < spawnFreq) {
            tempMatrix[x][mySettings->matrixSettings.height - 1] = random(backgroundDepth, maxBrightness);
        }

        // Step 3. Map from tempMatrix cells to LED colors
        for (int y = 0; y < mySettings->matrixSettings.height; y++) {
            if (tempMatrix[x][y] >= backgroundDepth) {  // Don't write out empty cells
                myMatrix->drawPixelXY(x, y, ColorFromPalette(rain_p, tempMatrix[x][y]));
            }
        }

        // Step 4. Add splash if called for
        if (splashes) {
            // FIXME, this is broken
            byte j = splashArray[x];
            byte v = tempMatrix[x][0];

            if (j >= backgroundDepth) {
                myMatrix->drawPixelXY(myMatrix->wrapX(x - 2), 0, ColorFromPalette(rain_p, j/3));
                myMatrix->drawPixelXY(myMatrix->wrapX(x + 2), 0, ColorFromPalette(rain_p, j/3));
                splashArray[x] = 0;   // Reset splash
            }

            if (v >= backgroundDepth) {
                myMatrix->drawPixelXY(myMatrix->wrapX(x - 1), 1, ColorFromPalette(rain_p, v/2));
                myMatrix->drawPixelXY(myMatrix->wrapX(x + 1), 1, ColorFromPalette(rain_p, v/2));
                splashArray[x] = v; // Prep splash for next frame
            }
        }

        // Step 5. Add lightning if called for
        if (storm) {
            //uint8_t lightning[WIDTH][HEIGHT];
            // ESP32 does not like static arrays  https://github.com/espressif/arduino-esp32/issues/2567
            uint8_t *lightning = (uint8_t *) malloc(mySettings->matrixSettings.width * mySettings->matrixSettings.height);
            while (lightning == NULL) { Serial.println("lightning malloc failed"); }


            if (random16() < 72) {    // Odds of a lightning bolt
                lightning[scale8(random8(), mySettings->matrixSettings.width - 1) + (mySettings->matrixSettings.height - 1) * mySettings->matrixSettings.width] = 255;  // Random starting location
                for(int ly = mySettings->matrixSettings.height-1; ly > 1; ly--) {
                    for (int lx = 1; lx < mySettings->matrixSettings.width - 1; lx++) {
                        if (lightning[lx + ly * mySettings->matrixSettings.width] == 255) {
                            lightning[lx + ly * mySettings->matrixSettings.width] = 0;
                            uint8_t dir = random8(4);
                            switch (dir) {
                            case 0:
                                myMatrix->drawPixelXY(lx + 1, ly - 1, CRGB(lightningColor));
                                lightning[myMatrix->wrapX(lx + 1) + (ly - 1) * mySettings->matrixSettings.width] = 255; // move down and right
                                break;
                            case 1:
                                myMatrix->drawPixelXY(lx, ly - 1, CRGB(128,128,128));
                                lightning[lx + (ly-1) * mySettings->matrixSettings.width] = 255;    // move down
                                break;
                            case 2:
                                myMatrix->drawPixelXY(lx - 1, ly - 1, CRGB(128,128,128));
                                lightning[myMatrix->wrapX(lx - 1) + (ly - 1) * mySettings->matrixSettings.width] = 255; // move down and left
                                break;
                            case 3:
                                myMatrix->drawPixelXY(lx - 1, ly - 1, CRGB(128,128,128));
                                lightning[myMatrix->wrapX(lx-1) + (ly-1) * mySettings->matrixSettings.width] = 255; // fork down and left
                                myMatrix->drawPixelXY(lx - 1, ly - 1, CRGB(128,128,128));
                                lightning[myMatrix->wrapX(lx + 1) + (ly - 1) * mySettings->matrixSettings.width] = 255; // fork down and right
                                break;
                            }
                        }
                    }
                }
            }
            free(lightning);
        }

        // Step 6. Add clouds if called for
        if (clouds) {
            uint16_t noiseScale = 250;  // A value of 1 will be so zoomed in, you'll mostly see solid colors. A value of 4011 will be very zoomed out and shimmery
            //const uint16_t cloudHeight = (HEIGHT*0.2)+1;
            const uint8_t cloudHeight = mySettings->matrixSettings.height * 0.4 + 1; // это уже 40% c лишеним, но на высоких матрицах будет чуть меньше

            // This is the array that we keep our computed noise values in
            //static uint8_t noise[WIDTH][cloudHeight];
            static uint8_t *noise = (uint8_t *) malloc(mySettings->matrixSettings.width * cloudHeight);

            while (noise == NULL) { Serial.println("noise malloc failed"); }
            int xoffset = noiseScale * x + hue;

            for(uint8_t z = 0; z < cloudHeight; z++) {
                int yoffset = noiseScale * z - hue;
                uint8_t dataSmoothing = 192;
                uint8_t noiseData = qsub8(inoise8(noiseX + xoffset,noiseY + yoffset,noiseZ),16);
                noiseData = qadd8(noiseData,scale8(noiseData,39));
                noise[x * cloudHeight + z] = scale8( noise[x * cloudHeight + z], dataSmoothing) + scale8( noiseData, 256 - dataSmoothing);
                myMatrix->blendPixelXY(x, mySettings->matrixSettings.height - z - 1, ColorFromPalette(rainClouds_p, noise[x * cloudHeight + z]), (cloudHeight-z)*(250/cloudHeight));
            }
            noiseZ ++;
        }
    }
}

}

RainNeoEffect::RainNeoEffect()
{

}

void RainNeoEffect::activate()
{
    tempMatrix = new uint8_t*[mySettings->matrixSettings.width]();
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; ++i) {
        tempMatrix[i] = new uint8_t[mySettings->matrixSettings.height];
    }
    splashArray = new uint8_t[mySettings->matrixSettings.width]();
}

void RainNeoEffect::deactivate()
{
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        delete[] tempMatrix[i];
    }
    delete[] tempMatrix;
    delete[] splashArray;
}

void RainNeoEffect::tick()
{
    rain();
}

void RainNeoEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("rColor"))) {
        rainColor = json[F("rColor")];
    }
    if (json.containsKey(F("lColor"))) {
        lightningColor = json[F("lColor")];
    }
    if (json.containsKey(F("bDpth"))) {
        backgroundDepth = json[F("bDpth")];
    }
    if (json.containsKey(F("mBri"))) {
        maxBrightness = json[F("mBri")];
    }
    if (json.containsKey(F("freq"))) {
        spawnFreq = json[F("freq")];
    }
    if (json.containsKey(F("length"))) {
        tailLength = json[F("length")];
    }
    if (json.containsKey(F("splashes"))) {
        splashes = json[F("splashes")];
    }
    if (json.containsKey(F("clouds"))) {
        clouds = json[F("clouds")];
    }
    if (json.containsKey(F("storm"))) {
        storm = json[F("storm")];
    }
}

void RainNeoEffect::writeSettings(JsonObject &json)
{
    json[F("rColor")] = rainColor;
    json[F("lColor")] = lightningColor;
    json[F("bDpth")] = backgroundDepth;
    json[F("mBri")] = maxBrightness;
    json[F("freq")] = spawnFreq;
    json[F("length")] = tailLength;
    json[F("splashes")] = splashes;
    json[F("clouds")] = clouds;
    json[F("storm")] = storm;
}

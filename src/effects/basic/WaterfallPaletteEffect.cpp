#include "WaterfallPaletteEffect.h"

namespace {

uint8_t sparkling = 80;

uint8_t** noise3d = nullptr;

const TProgmemRGBPalette16 WaterfallColors4in1_p FL_PROGMEM = {
  CRGB::Black,
  CRGB::DarkSlateGray,
  CRGB::DimGray,
  CRGB::LightSlateGray,

  CRGB::DimGray,
  CRGB::DarkSlateGray,
  CRGB::Silver,
  CRGB::DarkCyan,

  CRGB::Lavender,
  CRGB::Silver,
  CRGB::Azure,
  CRGB::LightGrey,

  CRGB::GhostWhite,
  CRGB::Silver,
  CRGB::White,
  CRGB::RoyalBlue
};

}

WaterfallPaletteEffect::WaterfallPaletteEffect(const String &id)
    : Effect(id)
{

}

void WaterfallPaletteEffect::tick()
{
    uint8_t rCOOLINGNEW = constrain((uint16_t)(settings.scale % 16) * 32 / mySettings->matrixSettings.height + 16, 1, 255) ;
    // Array of temperature readings at each simulation cell
    //static byte heat[WIDTH][HEIGHT]; будет noise3d[WIDTH][HEIGHT]

    for (uint8_t x = 0; x < mySettings->matrixSettings.width; x++) {
        // Step 1.  Cool down every cell a little
        for (unsigned int i = 0; i < mySettings->matrixSettings.height; i++) {
            //noise3d[x][i] = qsub8(noise3d[x][i], random8(0, ((rCOOLINGNEW * 10) / HEIGHT) + 2));
            noise3d[x][i] = qsub8(noise3d[x][i], random8(0, rCOOLINGNEW));
        }

        // Step 2.  Heat from each cell drifts 'up' and diffuses a little
        for (int k = mySettings->matrixSettings.height - 1; k >= 2; k--) {
            noise3d[x][k] = (noise3d[x][k - 1] + noise3d[x][k - 2] + noise3d[x][k - 2]) / 3;
        }

        // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
        if (random8() < sparkling) {
            int y = random8(2);
            noise3d[x][y] = qadd8(noise3d[x][y], random8(160, 255));
        }

        // Step 4.  Map from heat cells to LED colors
        for (unsigned int j = 0; j < mySettings->matrixSettings.height; j++) {
            // Scale the heat value from 0-255 down to 0-240
            // for best results with color palettes.
            byte colorindex = scale8(noise3d[x][j], 240);
            if  (settings.scale < 16) {            // Lavafall
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(LavaColors_p, colorindex));
            } else if (settings.scale < 32) {      // Firefall
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(HeatColors_p, colorindex));
            } else if (settings.scale < 48) {      // Waterfall
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(WaterfallColors4in1_p, colorindex));
            } else if (settings.scale < 64) {      // Skyfall
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(CloudColors_p, colorindex));
            } else if (settings.scale < 80) {      // Forestfall
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(ForestColors_p, colorindex));
            } else if (settings.scale < 96) {      // Rainbowfall
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(RainbowColors_p, colorindex));
            } else {                      // Aurora
                myMatrix->drawPixelXY(x,
                                      mySettings->matrixSettings.height - 1 - j,
                                      ColorFromPalette(RainbowStripeColors_p, colorindex));
            }
        }
    }
}

void WaterfallPaletteEffect::activate()
{
    noise3d = new uint8_t*[mySettings->matrixSettings.width]();
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; ++i) {
        noise3d[i] = new uint8_t[mySettings->matrixSettings.height];
    }
}

void WaterfallPaletteEffect::deactivate()
{
    for (uint8_t i = 0; i < mySettings->matrixSettings.width; i++) {
        delete[] noise3d[i];
    }
    delete[] noise3d;
}

void WaterfallPaletteEffect::initialize(const JsonObject &json)
{
    Effect::initialize(json);
    if (json.containsKey(F("sparkling"))) {
        sparkling = json[F("sparkling")];
    }
}

void WaterfallPaletteEffect::writeSettings(JsonObject &json)
{
    json[F("sparkling")] = sparkling;
}

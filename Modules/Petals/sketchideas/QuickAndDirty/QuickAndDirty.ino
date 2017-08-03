#include "FastLED.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

//TODO: Move to struct, and use real pin numbers.
/*const int  SPAR_1_DATA_PIN = 25;
const int  SPAR_1_CLOCK_PIN = 24;
const int  SPAR_2_DATA_PIN = 23;
const int  SPAR_2_CLOCK_PIN = 22;
const int  SPAR_3_DATA_PIN = 21;
const int  SPAR_3_CLOCK_PIN = 20;
const int  SPAR_4_DATA_PIN = 19;
const int  SPAR_4_CLOCK_PIN = 18;
const int  SPAR_5_DATA_PIN = 17;
const int  SPAR_5_CLOCK_PIN = 16;
const int  SPAR_6_DATA_PIN = 15;
const int  SPAR_6_CLOCK_PIN = 14;*/

const int  PETAL_RING_0_CLOCK_PIN = 2;
const int  PETAL_RING_0_DATA_PIN = 3;
const int  PETAL_RING_1_CLOCK_PIN = 4;
const int  PETAL_RING_1_DATA_PIN = 5;
const int  PETAL_RING_2_CLOCK_PIN = 6;
const int  PETAL_RING_2_DATA_PIN = 7;

const int PETALS_PER_ROW = 6;
const int SPARS_PER_ROW = 6;

const int NUM_LEDS_PETAL_RING_0 = 900;
const int NUM_LEDS_PETAL_RING_1 = 900;
const int NUM_LEDS_PETAL_RING_2 = 900;

const int AVG_LEDS_PER_PETAL = 150;

const int TOTAL_LEDS = 2700; // All petals and spars
CRGBArray<TOTAL_LEDS> leds; // The master array of all LEDs

#define LED_TYPE    DOTSTAR
#define COLOR_ORDER BGR

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120

CRGBPalette16 currentNoisePalette;
CRGBPalette16 targetNoisePalette;
TBlendType    currentNoiseBlending;                                // NOBLEND or LINEARBLEND

uint8_t maxNoiseChanges = 24;      // Number of tweens between palettes.


void setup() {
  delay(3000); // 3 second delay for recovery
  
  // tell FastLED about the LED strip configuration
  // Sigh.  So hacky to specify ranges this way, but it's quick.
  FastLED.addLeds<LED_TYPE,PETAL_RING_0_DATA_PIN, PETAL_RING_0_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, 0, NUM_LEDS_PETAL_RING_0).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,PETAL_RING_1_DATA_PIN, PETAL_RING_1_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PETAL_RING_0, NUM_LEDS_PETAL_RING_1).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,PETAL_RING_2_DATA_PIN, PETAL_RING_2_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PETAL_RING_0 + NUM_LEDS_PETAL_RING_1, NUM_LEDS_PETAL_RING_2).setCorrection( TypicalLEDStrip );

  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // for the noise routine
  currentNoisePalette = PartyColors_p;
  currentNoiseBlending = LINEARBLEND;  
}


void noise16_2() {                                            // just moving along one axis = "lavalamp effect"

  uint8_t scale = 1000;                                       // the "zoom factor" for the noise

  for (uint16_t i = 0; i < TOTAL_LEDS; i++) {

    uint16_t shift_x = millis() / 10;                         // x as a function of time
    uint16_t shift_y = 0;

    uint32_t real_x = (i + shift_x) * scale;                  // calculate the coordinates within the noise field
    uint32_t real_y = (i + shift_y) * scale;                  // based on the precalculated positions
    uint32_t real_z = 4223;
    
    uint8_t noise = inoise16(real_x, real_y, real_z) >> 8;    // get the noise data and scale it down

    uint8_t index = sin8(noise*3);                            // map led color based on noise data
    uint8_t bri   = noise;

    leds[i] = ColorFromPalette(currentNoisePalette, index, bri, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.

  }
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { petalSizer, rainbow, rainbowWithGlitter, confetti, sinelon, juggle, bpm, beNoisy };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 15 ) { nextPattern(); } // change patterns periodically
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void petalSizer()
{
   fill_solid(leds, TOTAL_LEDS, CRGB::Blue);    
   for (int i = 0; i < TOTAL_LEDS; i += AVG_LEDS_PER_PETAL) {
    leds[i] = CRGB::Red;
   }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, TOTAL_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(TOTAL_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, TOTAL_LEDS, 10);
  int pos = random16(TOTAL_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, TOTAL_LEDS, 20);
  int pos = beatsin16(13,0,TOTAL_LEDS);
  leds[pos] += CHSV( gHue, 255, 192);
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < TOTAL_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, TOTAL_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,TOTAL_LEDS)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void beNoisy() {

  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(currentNoisePalette, targetNoisePalette, maxNoiseChanges);  // Blend towards the target palette
  }

  EVERY_N_SECONDS(5) {             // Change the target palette to a random one every 5 seconds.
    targetNoisePalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }

  noise16_2();
  
 
}

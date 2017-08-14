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
const int  SPAR_0_DATA_PIN = 4; //15;
const int  SPAR_0_CLOCK_PIN = 5; // 14;
const int  SPAR_1_DATA_PIN = 25;
const int  SPAR_1_CLOCK_PIN = 24;
const int  SPAR_2_DATA_PIN = 23;
const int  SPAR_2_CLOCK_PIN = 22;
const int  SPAR_3_DATA_PIN = 21;
const int  SPAR_3_CLOCK_PIN = 20;
const int  SPAR_4_DATA_PIN = 19;
const int  SPAR_4_CLOCK_PIN = 18;
const int  SPAR_5_DATA_PIN = 17;
const int  SPAR_5_CLOCK_PIN = 16;


const int  PETAL_RING_0_DATA_PIN = 2;
const int  PETAL_RING_0_CLOCK_PIN = 3;
const int  PETAL_RING_1_DATA_PIN = 20; //4;
const int  PETAL_RING_1_CLOCK_PIN = 21; //5;
const int  PETAL_RING_2_DATA_PIN = 6;
const int  PETAL_RING_2_CLOCK_PIN = 7;

const int SPARS_PER_ROW = 6;

const int PETALS_PER_ROW = 6;

const int NUM_LEDS_PETAL_RING_0 = 900;
const int NUM_LEDS_PETAL_RING_1 = 900;
const int NUM_LEDS_PETAL_RING_2 = 900;

const int AVG_LEDS_PER_SPAR = 150;
const int AVG_LEDS_PER_PETAL = 150;
const int AVG_LEDS_PER_RING = 900;

const int TOTAL_LEDS = 2700; // All petals
CRGBArray<TOTAL_LEDS> leds; // The master array of all petal LEDs
CRGBArray<AVG_LEDS_PER_SPAR> sparLeds[SPARS_PER_ROW];

#define LED_TYPE    DOTSTAR
#define COLOR_ORDER BGR

#define BRIGHTNESS          220
#define FRAMES_PER_SECOND  120


//
// For "catching fire" pattern
//
bool gReverseDirection = false;
CRGBPalette16 gPal;

CRGBPalette16 gPalHeat;
CRGBPalette16 gPalBlue;
CRGBPalette16 gPalForest;
CRGBPalette16 gPalRainbow;
CRGBPalette16 gPalLava;
CRGBPalette16 gPalWedding;

#define INITIAL_COOLING  75
#define MIN_COOLING  40
#define MAX_COOLING  120

#define INITIAL_SPARKING 10
#define MAX_SPARKING 140
#define MIN_SPARKING 30

#define INITIAL_SPAR_SPARKING 40
#define MAX_SPAR_SPARKING 180
#define MIN_SPAR_SPARKING 40

#define INITIAL_SPAR_COOLING  55
#define MIN_SPAR_COOLING  30
#define MAX_SPAR_COOLING  110

int cooling = INITIAL_COOLING;
int sparking = INITIAL_SPARKING;

int sparCooling = INITIAL_SPAR_COOLING;
int sparSparking = INITIAL_SPAR_SPARKING;


//
// For noise pattern
//
uint8_t maxNoiseChanges = 24;      // Number of tweens between palettes.
CRGBPalette16 currentNoisePalette;
CRGBPalette16 targetNoisePalette;
TBlendType    currentNoiseBlending;                                // NOBLEND or LINEARBLEND

//
// For "Christmas sparkles"
//
uint16_t ledsData[AVG_LEDS_PER_RING][4];  // array to store color data and an extra value
uint16_t pick;  // stores a temporary pixel number


//
// Sensor Data
//

// Audio
bool audioActive = true;
unsigned int micValue; // Between 0 and 1023
int frequencyValues[] = {345, 1023, 138, 0, 98, 101, 478}; // 7 Channels each between 0 and 1023


void setup() {
  delay(3000); // 3 second delay for recovery

  // tell FastLED about the LED strip configuration
  // Sigh.  So hacky to specify ranges this way, but it's quick.
  FastLED.addLeds<LED_TYPE, PETAL_RING_0_DATA_PIN, PETAL_RING_0_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, 0, NUM_LEDS_PETAL_RING_0).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, PETAL_RING_1_DATA_PIN, PETAL_RING_1_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PETAL_RING_0, NUM_LEDS_PETAL_RING_1).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, PETAL_RING_2_DATA_PIN, PETAL_RING_2_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PETAL_RING_0 + NUM_LEDS_PETAL_RING_1, NUM_LEDS_PETAL_RING_2).setCorrection( TypicalLEDStrip );

  FastLED.addLeds<LED_TYPE, SPAR_0_DATA_PIN, SPAR_0_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(sparLeds[0], 0, AVG_LEDS_PER_SPAR).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, SPAR_1_DATA_PIN, SPAR_1_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(sparLeds[1], 0, AVG_LEDS_PER_SPAR).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, SPAR_2_DATA_PIN, SPAR_2_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(sparLeds[2], 0, AVG_LEDS_PER_SPAR).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, SPAR_3_DATA_PIN, SPAR_3_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(sparLeds[3], 0, AVG_LEDS_PER_SPAR).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, SPAR_4_DATA_PIN, SPAR_4_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(sparLeds[4], 0, AVG_LEDS_PER_SPAR).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE, SPAR_5_DATA_PIN, SPAR_5_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(sparLeds[5], 0, AVG_LEDS_PER_SPAR).setCorrection( TypicalLEDStrip );


  // set master brightness control
  FastLED.setBrightness(BRIGHTNESS);

  // for the noise routine
  currentNoisePalette = PartyColors_p;
  currentNoiseBlending = LINEARBLEND;

  // For fire

  // This first palette is the basic 'black body radiation' colors,
  // which run from black to red to bright yellow to white.
 // gPalHeat = HeatColors_p;

  // These are other ways to set up the color palette for the 'fire'.
  // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
  gPalHeat = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);

  // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
  gPal = gPalHeat;
  gPalBlue =   CRGBPalette16( CRGB::Black, CRGB::Purple, CRGB::Aqua,  CRGB::White);

  const TProgmemRGBPalette16 MyForest_p FL_PROGMEM = {
    CRGB::Black, 
    CRGB::DarkGreen, 
    CRGB::DarkGreen, 
    CRGB::DarkOliveGreen,
    
    CRGB::Green,
    CRGB::ForestGreen,
    CRGB::OliveDrab,
    CRGB::SeaGreen,
    
    CRGB::MediumAquamarine,
    CRGB::MediumAquamarine,
    CRGB::LawnGreen,
    CRGB::LimeGreen,
    
    CRGB::YellowGreen,
    CRGB::LightGreen,
    CRGB::LightGreen,
    CRGB::White };
    
  gPalForest = MyForest_p;
    
  //gPalForest = ForestColors_p;

  gPalRainbow = RainbowColors_p;
  
  //gPalLava = LavaColors_p;
  const TProgmemRGBPalette16 MyLava_p FL_PROGMEM = {
    CRGB::Black,
    CRGB::Black,
    CRGB::Maroon,
    CRGB::Maroon,

    CRGB::DarkRed,
    CRGB::DarkRed,
    CRGB::DarkRed,
    CRGB::Red,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Orange,
    CRGB::Orange,

    CRGB::Orange,
    CRGB::Orange,
    CRGB::White,
    CRGB::White};

  gPalLava = MyLava_p;
  
  const TProgmemRGBPalette16 MyWedding_p FL_PROGMEM = {
    CRGB::Black,
    CRGB::Purple,
    CRGB::LavenderBlush,
    CRGB::Lavender,

    CRGB::MediumPurple,
    CRGB::MediumOrchid,
    CRGB::PowderBlue,
    CRGB::Thistle,
    
    CRGB::LavenderBlush,
    CRGB::Lavender,
    CRGB::CornflowerBlue,
    CRGB::Orchid,

    CRGB::CornflowerBlue,
    CRGB::LightBlue,
    CRGB::Ivory,
    CRGB::White};

  gPalWedding = MyWedding_p;
}

int normalizeFrequency150(int freqVal) {
  // always have a minimum, so there's something on the arm
  return (freqVal / 8) + 22;

}

void adjCooling(boolean cooldown) {
  if (cooldown) {
    int coolFactor = random8(5);
    cooling -= coolFactor;
    sparCooling -= (coolFactor + 2);
  } else {
    int coolFactor = random8(8);
    cooling += coolFactor;
    sparCooling += (coolFactor +2);
  }
  
  if (cooling < MIN_COOLING) {
    cooling = MIN_COOLING;
  } else if (cooling > MAX_COOLING) {
    cooling = MAX_COOLING;
  }

  if (sparCooling < MIN_SPAR_COOLING) {
    sparCooling = MIN_SPAR_COOLING;
  } else if (sparCooling > MAX_SPAR_COOLING) {
    sparCooling = MAX_SPAR_COOLING;
  }
}

void adjSparking(boolean cooldown) {
  if (cooldown) {
    int sparkFactor = random8(5);
    sparking += sparkFactor;
    sparSparking += sparkFactor;
  } else {
    int sparkFactor = random8(8);
    sparking -= sparkFactor;
    sparSparking -= sparkFactor;
  }
  
  if (sparking > MAX_SPARKING) {
    sparking = MAX_SPARKING;
  } else if (sparking < MIN_SPARKING) {
    sparking = MIN_SPARKING;
  }
  
  if (sparSparking > MAX_SPAR_SPARKING) {
    sparSparking = MAX_SPAR_SPARKING;
  } else if (sparSparking < MIN_SPAR_SPARKING) {
    sparSparking = MIN_SPAR_SPARKING;
  }
}

// Array of temperature readings at each simulation cell
static byte heat[AVG_LEDS_PER_PETAL / 2];
static byte sparheat[AVG_LEDS_PER_SPAR];

void Fire2012WithPalette()
{
  int numLeds = AVG_LEDS_PER_PETAL / 2; // we're going to mirror this from either end symmetrically

  // Step 1.  Cool down every cell a little
  for ( int i = 0; i < numLeds; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / AVG_LEDS_PER_PETAL) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( int k = numLeds - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < sparking ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( int j = 0; j < numLeds; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    byte colorindex = scale8( heat[j], 240);
    CRGB color = ColorFromPalette( gPal, colorindex);
    int pixelnumber;
    if ( gReverseDirection ) {
      pixelnumber = (numLeds - 1) - j;
    } else {
      pixelnumber = j;
    }

    leds[pixelnumber] = color;
    //leds[NUM_LEDS -1 - pixelnumber] = color;
  }
}


void Fire2012WithPaletteForSpars(int sparId)
{

    int numLeds = AVG_LEDS_PER_SPAR;

    // Step 1.  Cool down every cell a little
    for( int i = 0; i < numLeds; i++) {
     sparheat[i] = qsub8( sparheat[i],  random8(0, ((sparCooling * 10) / AVG_LEDS_PER_SPAR) + 2));
    }

    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= numLeds - 1; k >= 2; k--) {
     sparheat[k] = (sparheat[k - 1] + sparheat[k - 2] + sparheat[k - 2] ) / 3;
    }

    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < sparSparking ) {
     int y = random8(7);
     sparheat[y] = qadd8( sparheat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < numLeds; j++) {
     // Scale the heat value from 0-255 down to 0-240
     // for best results with color palettes.
     byte colorindex = scale8( sparheat[j], 240);
     CRGB color = ColorFromPalette( gPal, colorindex);
     int pixelnumber;
     if( gReverseDirection ) {
       pixelnumber = (numLeds-1) - j;
     } else {
       pixelnumber = j;
     }

     sparLeds[sparId][pixelnumber] = color;
    }
  
}


//
// Utilities to copy or mirror petals and rings
//
void copyPetalToPetal( int i ) {
  leds( i * AVG_LEDS_PER_PETAL, ((i + 1) *AVG_LEDS_PER_PETAL) - 1) = leds( (i - 1) * AVG_LEDS_PER_PETAL, (i * AVG_LEDS_PER_PETAL) - 1 );
}

void mirrorPetalToPetal( int i ) {
  leds( i * AVG_LEDS_PER_PETAL, ((i + 1) *AVG_LEDS_PER_PETAL) - 1) = leds( (i * AVG_LEDS_PER_PETAL) - 1, (i - 1) * AVG_LEDS_PER_PETAL);
}


void copyPetalToAll18() {
  // copy on all 18 petals
  for (int i = 1; i < 18; i++) {
    copyPetalToPetal(i);
  }
}

void mirrorImagePetalToAll18() {
  // copy on all 18 petals, mirroring on the even ones
  for (int i = 1; i < 18; i++) {
    if ( i % 2 == 1) {
      copyPetalToPetal(i);
    } else {
      mirrorPetalToPetal(i);
    }
  }
}

void copyRingToRing( int i ) {
  leds( i * AVG_LEDS_PER_RING, ((i + 1) * AVG_LEDS_PER_RING) - 1) = leds( (i - 1) * AVG_LEDS_PER_RING, (i * AVG_LEDS_PER_RING) - 1 );
}

void mirrorRingToRing( int i ) {
  leds( i * AVG_LEDS_PER_RING, ((i + 1) * AVG_LEDS_PER_RING) - 1) = leds( (i * AVG_LEDS_PER_RING) - 1, (i - 1) * AVG_LEDS_PER_RING);
}

void copyRingToAll3() {
  // copy on all 3 rings
  for (int i = 1; i < 3; i++) {
    copyRingToRing(i);
  }
}

void mirrorImageRingToAll3() {
  // copy on all 18 petals, mirroring on the even ones
  for (int i = 1; i < 3; i++) {
    if ( i % 2 == 1) {
      copyRingToRing(i);
    } else {
      mirrorRingToRing(i);
    }
  }
}

void copyToAllSpars() {
  for (int i = 1; i < SPARS_PER_ROW; i++) {
    sparLeds[i] = sparLeds[0];
  }
}


void noise16_2() {                                            // just moving along one axis = "lavalamp effect"

  uint8_t scale = 1000;                                       // the "zoom factor" for the noise

  for (uint16_t i = 0; i < AVG_LEDS_PER_RING; i++) {

    uint16_t shift_x = millis() / 10;                         // x as a function of time
    uint16_t shift_y = 0;

    uint32_t real_x = (i + shift_x) * scale;                  // calculate the coordinates within the noise field
    uint32_t real_y = (i + shift_y) * scale;                  // based on the precalculated positions
    uint32_t real_z = 4223;

    uint8_t noise = inoise16(real_x, real_y, real_z) >> 8;    // get the noise data and scale it down

    uint8_t index = sin8(noise * 3);                          // map led color based on noise data
    uint8_t bri   = noise;

    leds[i] = ColorFromPalette(currentNoisePalette, index, bri, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.

  }
}

void addGlitter( fract8 chanceOfGlitter, int multiple)
{
  if ( random8() < chanceOfGlitter) {
    for (int i = 0; i < multiple; i++) {
      leds[ random16(AVG_LEDS_PER_PETAL) ] += CRGB::White;
    }
  }
}

void addWeddingGlitter( fract8 chanceOfGlitter, int multiple)
{
  if ( random8() < chanceOfGlitter) {
    for (int i = 0; i < multiple; i++) {
      leds[ random16(TOTAL_LEDS) ] += CRGB::MediumOrchid;
      leds[ random16(TOTAL_LEDS) ] += CRGB::Lavender;
      leds[ random16(TOTAL_LEDS) ] += CRGB::Aqua;
      leds[ random16(TOTAL_LEDS) ] += CRGB::CornflowerBlue;
    }
  }
}


// *************************
// List of patterns to cycle through.  Each is defined as a separate function below. They can appear more than once, which gives us a really
// hacky way of making some run longer than others.
//
// TODO: tweak or remove the ones we think suck :)
//
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = {
  rain18Wedding,
  fire18Wedding,
  fire18Heat,
  fire18Lava,
  bpmGlitter, // rocking rainbow ok for  bit, needs variation - audio?
  fire18Blue,
  fire18Forest,
  bpm, // rocking rainbow ok for  bit, needs variation - audio?
  rain18Rainbow,
  beNoisy, // fast cycle rainbow, not bad, could be brighter
  rain18Forest,
  rain18Heat,
  beNoisyGlitter, // fast cycle rainbow, not bad, could be brighter
  rain18Lava,
  rain18Blue,
  //rainbow,
  rainbowWithGlitter,
  fire18Rainbow,
  juggle, // multicolor sparkles,
  
};


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns

bool freqInc = true;
void fakeAudioFrequencyChange() {
  
  int delta = random8(24);
  
  for (int i = 0; i < 7; i++) {
    if (freqInc) { 
      frequencyValues[i] += delta;
      if (frequencyValues[i] > 1023) {
        frequencyValues[i] = 1023;
        freqInc = false;
      } 
    } else {
      frequencyValues[i] -= delta;
      if (frequencyValues[i] < 0) {
        frequencyValues[i] = 0;
        freqInc = true;
      }    
    }
  }
}


const int SECONDS_PER_COOLDOWN_DELTA = 5;
const int SECONDS_PER_SPARKING_DELTA = 2;
const int SECONDS_HEAT_COOL_CYCLE = 60;
  
const int PATTERN_LENGTH_SECONDS = SECONDS_HEAT_COOL_CYCLE * 4 ;  // Best as an even mmultiple of SECONDS_HEAT_COOL_CYCLE for even fire


//
// Main control loop
//
void loop()
{

  // Call the current pattern function once, updating the 'leds' array.  May overwrite spars
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();

  // make the audio freq seem to change
  EVERY_N_MILLISECONDS( 40 ) {
    fakeAudioFrequencyChange();
  }
  
  // do some periodic updates  TODO: maybe we should move this into the patterns that care?
  EVERY_N_MILLISECONDS( 200 ) {
    gHue++;  // slowly cycle the "base color" through the rainbow
  }
  EVERY_N_SECONDS( PATTERN_LENGTH_SECONDS ) {
    nextPattern();  // change patterns periodically
  }
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

//
// Animation routines here
//

void petalSizer()
{
  fill_solid(leds, TOTAL_LEDS, CHSV( gHue , 200, 255));
  for (int i = 0; i < TOTAL_LEDS; i += AVG_LEDS_PER_PETAL) {
    leds[i] = CRGB::Red;
  }
}

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, AVG_LEDS_PER_RING, gHue, 7);

  // Mirror to all
  mirrorImageRingToAll3();
  // default spars to audio
  sparToAudio();

}

void rainbowWithGlitter()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, AVG_LEDS_PER_PETAL, gHue, 7);

  addGlitter(80, 3);

  // Mirror to all
  copyPetalToAll18();
  // default spars to audio
  sparToAudio();

}


void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, AVG_LEDS_PER_PETAL, 10);
  int pos = random16(AVG_LEDS_PER_PETAL);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  // Mirror to all
  copyPetalToAll18();
  // default spars to audio
  sparToAudio();

}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, AVG_LEDS_PER_PETAL, 20);
  int pos = beatsin16(13, 0, AVG_LEDS_PER_PETAL);
  leds[pos] += CHSV( gHue, 255, 192);
  // Mirror to all
  copyPetalToAll18();
  // default spars to audio
  sparToAudio();

}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < AVG_LEDS_PER_PETAL; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  // Mirror to all
  copyPetalToAll18();
  // default spars to audio
  sparToAudio();

}

void bpmGlitter()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for ( int i = 0; i < AVG_LEDS_PER_PETAL; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue + (i * 2), beat - gHue + (i * 10));
  }
  
  addGlitter(80, 3);
  
  // Mirror to all
  copyPetalToAll18();
  // default spars to audio
  sparToAudio();

}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, AVG_LEDS_PER_PETAL, 20);
  byte dothue = 0;
  for ( int i = 0; i < 8; i++) {
    leds[beatsin16(i + 7, 0, AVG_LEDS_PER_PETAL)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  // Mirror to all
  copyPetalToAll18();
  // default spars to audio
  sparToAudio();

}

void beNoisy() {

  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(currentNoisePalette, targetNoisePalette, maxNoiseChanges);  // Blend towards the target palette
  }

  EVERY_N_SECONDS(5) {             // Change the target palette to a random one every 5 seconds.
    targetNoisePalette = CRGBPalette16(CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)), CHSV(random8(), 192, random8(128, 255)), CHSV(random8(), 255, random8(128, 255)));
  }

  noise16_2();

  // Mirror to all
  copyRingToAll3();

  // default spars to audio
  sparToAudio();

}

void beNoisyGlitter() {
  beNoisy();
  addGlitter(80, 3);
}



void fire18Heat() {
  gReverseDirection = false;  // fire, not rain
  gPal = gPalHeat;
  fire18();
}

void rain18Heat() {
  gReverseDirection = true;
  gPal = gPalHeat;
  fire18();
}

void fire18Blue() {
  gReverseDirection = false;  // fire, not rain
  gPal = gPalBlue;
  fire18();
}

void rain18Blue() {
  gReverseDirection = true;
  gPal = gPalBlue;
  fire18();
}

void fire18Wedding() {
  gReverseDirection = false;  // fire, not rain
  gPal = gPalWedding;
  fire18();
  addWeddingGlitter(80, 12);
}

void rain18Wedding() {
  gReverseDirection = true;
  gPal = gPalWedding;
  fire18();
  addWeddingGlitter(80, 12);
}

void fire18Rainbow() {
  gReverseDirection = false;  // fire, not rain
  gPal = gPalRainbow;
  fire18();
}

void rain18Rainbow() {
  gReverseDirection = true;
  gPal = gPalRainbow;
  fire18();
}

void fire18Forest() {
  gReverseDirection = false;  // fire, not rain
  gPal = gPalForest;
  fire18();
}

void rain18Forest() {
  gReverseDirection = true;
  gPal = gPalForest;
  fire18();
}


void fire18Lava() {
  gReverseDirection = false;  // fire, not rain
  gPal = gPalLava;
  fire18();
}

void rain18Lava() {
  gReverseDirection = true;
  gPal = gPalLava;
  fire18();
}


void fire18Change() {
  gReverseDirection = false;  // fire, not rain
  EVERY_N_SECONDS(120) {
    if (gPal == gPalHeat) {
      gPal = gPalBlue;
    } else {
      gPal = gPalHeat;
    }
  }
  fire18();
}

void rain18Change() {
  gReverseDirection = true;
  EVERY_N_SECONDS(120) {
    if (gPal == gPalHeat) {
      gPal = gPalBlue;
    } else {
      gPal = gPalHeat;
    }
  }
  fire18();
}


void fire18() {

  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());

  // Fourth, the most sophisticated: this one sets up a new palette every
  // time through the loop, based on a hue that changes every time.
  // The palette is a gradient from black, to a dark color based on the hue,
  // to a light color based on the hue, to white.
  //
  //   static uint8_t hue = 0;
  //   hue++;
  //   CRGB darkcolor  = CHSV(hue,255,192); // pure hue, three-quarters brightness
  //   CRGB lightcolor = CHSV(hue,128,255); // half 'whitened', full brightness
  //   gPal = CRGBPalette16( CRGB::Black, darkcolor, lightcolor, CRGB::White);

  boolean cooldown = false;

  EVERY_N_SECONDS(SECONDS_PER_COOLDOWN_DELTA) {
    adjCooling(cooldown);
  }
  EVERY_N_SECONDS(SECONDS_PER_SPARKING_DELTA) {
    adjSparking(cooldown);
  }

  EVERY_N_SECONDS(SECONDS_HEAT_COOL_CYCLE) {
    cooldown = !cooldown;
  }

  fire18petals();
  fire6spars();
}

void fire18petals()
{
  Fire2012WithPalette(); // run simulation frame, using palette colors

  // mirror and blur the other side
  leds(AVG_LEDS_PER_PETAL / 2, AVG_LEDS_PER_PETAL - 1) = leds(AVG_LEDS_PER_PETAL / 2 - 1, 0);

  copyPetalToAll18();
}

void fire6spars()
{
  Fire2012WithPaletteForSpars(0); // run simulation frame, using palette colors
  copyToAllSpars();
}

void sparToAudio() {
  if (audioActive) {
    for (int i = 0; i < SPARS_PER_ROW; i++) {
      int level = normalizeFrequency150(frequencyValues[i]);
      fill_solid(sparLeds[i], AVG_LEDS_PER_SPAR, CRGB::Black);  // black
      fill_solid(sparLeds[i], level, CHSV( gHue , 200, (level/2) + 127));
    }
  }
}



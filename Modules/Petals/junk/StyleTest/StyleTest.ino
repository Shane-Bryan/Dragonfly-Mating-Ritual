#include "Arduino.h"
#include <FastLED.h>

//TODO: Move to struct, and use real pin numbers.  Check for more efficient type
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
const int  SPAR_6_DATA_PIN = 15;
const int  SPAR_6_CLOCK_PIN = 14;

const int  PETAL_RING_0_DATA_PIN = 13;
const int  PETAL_RING_0_CLOCK_PIN = 12;
const int  PETAL_RING_1_DATA_PIN = 11;
const int  PETAL_RING_1_CLOCK_PIN = 10;
const int  PETAL_RING_2_DATA_PIN = 9;
const int  PETAL_RING_2_CLOCK_PIN = 8;

const int PETALS_PER_ROW = 6;
const int SPARS_PER_ROW = 6;

const int NUM_LEDS_PETAL_RING_0 = 900;
const int NUM_LEDS_PETAL_RING_1 = 900;
const int NUM_LEDS_PETAL_RING_2 = 900;

const int TOTAL_LEDS = 3600; // All petals and spars
CRGBArray<TOTAL_LEDS> allLeds; // The master array of all LEDs

// TODO: Can we rely on all strips being the same color order?  TEST!!!
#define LED_TYPE    DOTSTAR
#define COLOR_ORDER BGR

// These should be classes that inherit from a common base class, but I'm tired of fighting C++ idiosyncracies
struct Petal {
  unsigned int ledCount;
  unsigned int offset;
  CRGBSet *leds;
};

struct PetalRing {
  unsigned int ledCount;
  unsigned int offset;
  CRGBSet *leds;
};

struct SparRing {
  unsigned int ledCount;
  unsigned int offset;
  CRGBSet *leds;
};

struct Blossom {
  unsigned int ledCount;
  unsigned int offset = 0;
  CRGBSet *leds;
  SparRing *spars;
  PetalRing *petalRings[3];
};

Blossom testbloom;

void setupStructs() {
  testbloom.ledCount = 150;
  
}

//The setup function is called once at startup of the sketch
void setup()
{
  // Sigh.  So hacky to specify ranges this way, but it's quick.
  FastLED.addLeds<LED_TYPE,PETAL_RING_0_DATA_PIN, PETAL_RING_0_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(allLeds, 0, NUM_LEDS_PETAL_RING_0).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,PETAL_RING_1_DATA_PIN, PETAL_RING_1_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(allLeds, NUM_LEDS_PETAL_RING_0, NUM_LEDS_PETAL_RING_1).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,PETAL_RING_2_DATA_PIN, PETAL_RING_2_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(allLeds, NUM_LEDS_PETAL_RING_0 + NUM_LEDS_PETAL_RING_1, NUM_LEDS_PETAL_RING_2).setCorrection( TypicalLEDStrip );
}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { blackout };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

unsigned int sparCounts[] = {148, 149, 150, 149, 148, 150};
unsigned int petalRing0Counts[] = {148, 149, 150, 149, 148, 150};
unsigned int petalRing1Counts[] = {148, 149, 150, 149, 148, 150};
unsigned int petalRing2Counts[] = {148, 149, 150, 149, 148, 150};

CRGBSet getLedSegment(int start, int end) {
  return allLeds(start, end);
}

CRGBSet getEverything() {
  return allLeds;
}

CRGBSet getPetal1() {
  // TODO: abstract this out so we can do all
  return allLeds(0, 150);
}

CRGBSet getPetalRing1() {
  // TODO: abstract this out so we can do all
  return allLeds(0, 900);
}

void blackout() {
 
  CRGBSet foo = getEverything();
  fill_solid(foo, 10, CRGB::Black);
}


// The loop function is called in an endless loop
void loop()
{
  blackout();
}




#include "Arduino.h"
#include <FastLED.h>
//#include <ledsubset.h>

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

const int SPARS_PER_ROW = 6;
const int PETALS_PER_ROW = 6;

const int TOTAL_LEDS = 3600; // All petals and spars
CRGBArray<TOTAL_LEDS> allLeds; // The master array of all LEDs

// TODO: Can we rely on all strips being the same color order?  TEST!!!
#define LED_TYPE    DOTSTAR
#define COLOR_ORDER BGR

//The setup function is called once at startup of the sketch
void setup()
{
// Add your initialization code here
}

// The loop function is called in an endless loop
void loop()
{
//Add your repeated code here
}



class LedSubset {
  protected:
    unsigned int numLeds;
  public:
  
    CRGBSet leds;
    
    virtual void init(CRGBSet masterLeds, int startLed, unsigned int numLeds) {
      this.numLeds = numLeds;
      leds = leds(masterLeds(startLed, startLed + numLeds -1)
    }
    ~LedSubset();
}

class Petal : public LedSubset {
  public: 

    virtual void init( CRGBSet masterLeds, int startLed, unsigned int numLeds) {
      LedSubset:init(masterLeds, startLed, numLeds);
      int i = 123;
    }
};

class Spar : public LedSubset {};


class PetalRing : public LedSubset{
  public:
 
    Petal *petals= new Petal[PETALS_PER_ROW];
    PetalRing(CRGBSet masterLeds, int startLed, unsigned int totalLedCount, int numPetals, unsigned int ledCounts[]): LedSubset(masterLeds, startLed, totalLedCount) {
      int offset = startLed;
      petals = new Petal[PETALS_PER_ROW6]
      numLeds = 0;
      for (int i = 0; i < PETALS_PER_ROW; i++) {
        petals[i] = new Petal(masterLeds& , offset, ledCount[i]);
        numLeds += ledCount[i];
        offset += ledCounts[i];
      }
      leds = masterLeds(startLed, startLed + numLeds -1);
    }
};

class Blossom : public LedSubset {
  public:
    Blossom(CRGBArray masterLeds, unsigned int[] pr0, unsigned int[]pr1 , unsigned int[] pr2, unsigned int[] spars) {
        // TODO: add spar init code
        petalRings[0] = new PetalRing(masterLeds, 0, pr0);
        petalRings[1] = new PetalRing(masterLeds, petalRings[0].numLeds - 1, pr1);
        petalRings[2] = new PetalRing(masterLeds, petalRings[0].numLeds + petalRings[1].numLeds - 1, pr2);
      
        ledCount = sparRing.numLeds + petalRings[0].numLeds + petalRings[1].numLeds + petalRings[2].numLeds;
      }
}


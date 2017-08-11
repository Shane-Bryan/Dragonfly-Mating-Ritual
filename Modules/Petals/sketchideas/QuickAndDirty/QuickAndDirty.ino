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
const int AVG_LEDS_PER_RING = 900;

const int TOTAL_LEDS = 2700; // All petals and spars
CRGBArray<TOTAL_LEDS> leds; // The master array of all LEDs

#define LED_TYPE    DOTSTAR
#define COLOR_ORDER BGR

#define BRIGHTNESS          96
#define FRAMES_PER_SECOND  120


//
// For "catching fire" pattern
//
CRGBPalette16 gPal;

CRGBPalette16 gPalHeat;
CRGBPalette16 gPalBlue;

#define INITIAL_COOLING  75
#define MIN_COOLING  40
#define MAX_COOLING  120

#define INITIAL_SPARKING 10
#define MAX_SPARKING 140
#define MIN_SPARKING 30

int cooling = INITIAL_COOLING;
int sparking = INITIAL_SPARKING;


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


void adjCooling(boolean cooldown) {
  if (cooldown) {
    cooling -= random8(5);
  } else {
    cooling += random8(8);
  }
  if (cooling < MIN_COOLING) {
    cooling = MIN_COOLING;
  } else if (cooling > MAX_COOLING) {
    cooling = MAX_COOLING;
  }
}

void adjSparking(boolean cooldown) {
  if (cooldown) {
    sparking += random8(5);
  } else {
    sparking -= random8(8);
  }
  if (sparking > MAX_SPARKING) {
    sparking = MAX_SPARKING;
  } else if (sparking < MIN_SPARKING) {
    sparking = MIN_SPARKING;
  }
}

// Array of temperature readings at each simulation cell
static byte heat[AVG_LEDS_PER_PETAL/2];

void Fire2012WithPalette()
{
   int numLeds = AVG_LEDS_PER_PETAL/2;  // we're going to mirror this from either end symmetrically

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < numLeds; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / AVG_LEDS_PER_PETAL) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= numLeds - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < sparking ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < numLeds; j++) {
      // Scale the heat value from 0-255 down to 0-240
      // for best results with color palettes.
      byte colorindex = scale8( heat[j], 240);
      CRGB color = ColorFromPalette( gPal, colorindex);
      int pixelnumber = (numLeds-1) - j;
      
      leds[pixelnumber] = color;
      //leds[NUM_LEDS -1 - pixelnumber] = color;    
    }
}


//
// Utilities to copy or mirror petals and rings
//
void copyPetalToPetal( int i ) {
  leds( i * AVG_LEDS_PER_PETAL, ((i + 1) *AVG_LEDS_PER_PETAL) -1) = leds( (i -1) * AVG_LEDS_PER_PETAL, (i * AVG_LEDS_PER_PETAL) -1 );
}

void mirrorPetalToPetal( int i ) {
  leds( i * AVG_LEDS_PER_PETAL, ((i + 1) *AVG_LEDS_PER_PETAL) -1) = leds( (i * AVG_LEDS_PER_PETAL) -1, (i -1) * AVG_LEDS_PER_PETAL);
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
  leds( i * AVG_LEDS_PER_RING, ((i + 1) * AVG_LEDS_PER_RING) -1) = leds( (i -1) * AVG_LEDS_PER_RING, (i * AVG_LEDS_PER_RING) -1 );
}

void mirrorRingToRing( int i ) {
  leds( i * AVG_LEDS_PER_RING, ((i + 1) * AVG_LEDS_PER_RING) -1) = leds( (i * AVG_LEDS_PER_RING) -1, (i -1) * AVG_LEDS_PER_RING);
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

void noise16_2() {                                            // just moving along one axis = "lavalamp effect"

  uint8_t scale = 1000;                                       // the "zoom factor" for the noise

  for (uint16_t i = 0; i < AVG_LEDS_PER_RING; i++) {

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

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(AVG_LEDS_PER_PETAL) ] += CRGB::White;
  }
}

// List of patterns to cycle through.  Each is defined as a separate function below. They can appear more than once, which gives us a really
// hacky way of making some run longer than others.
//
// TODO: tweak or remove the ones we think suck :)
//
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { 
  petalSizer, 
  rainbow, 
  rainbowWithGlitter, 
  confetti, 
  sinelon, 
  juggle, 
  bpm, 
  beNoisy, 
  christmasSparkles, 
  christmasSparklesRG, 
  christmasSparkles, 
  christmasSparklesBP, 
  lightning,
  lightning,
  fire18, 
  fire18, 
  fire18, 
  fire18 };

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns


//
// Main control loop
//
void loop()
{
  // TODO: grab control data here, what will we really have?
  
  // Call the current pattern function once, updating the 'leds' array
  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates  TODO: maybe we should move this into the patterns that care?
  EVERY_N_MILLISECONDS( 2000 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 15 ) { nextPattern(); } // change patterns periodically
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
   fill_solid(leds, TOTAL_LEDS, CRGB::Blue);    
   for (int i = 0; i < TOTAL_LEDS; i += AVG_LEDS_PER_PETAL) {
    leds[i] = CRGB::Red;
   }
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, AVG_LEDS_PER_PETAL, gHue, 7);

  // Mirror to all
  copyPetalToAll18();
}

void rainbowWithGlitter() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, AVG_LEDS_PER_PETAL, gHue, 7);
  
  addGlitter(80);
  
  // Mirror to all
  copyPetalToAll18();
}


void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, AVG_LEDS_PER_PETAL, 10);
  int pos = random16(AVG_LEDS_PER_PETAL);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
  // Mirror to all
  copyPetalToAll18();
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, AVG_LEDS_PER_PETAL, 20);
  int pos = beatsin16(13,0,AVG_LEDS_PER_PETAL);
  leds[pos] += CHSV( gHue, 255, 192);
  // Mirror to all
  copyPetalToAll18();
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < AVG_LEDS_PER_PETAL; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
  // Mirror to all
  copyPetalToAll18();
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, AVG_LEDS_PER_PETAL, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16(i+7,0,AVG_LEDS_PER_PETAL)] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
  // Mirror to all
  copyPetalToAll18();
}

void beNoisy() {

  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(currentNoisePalette, targetNoisePalette, maxNoiseChanges);  // Blend towards the target palette
  }

  EVERY_N_SECONDS(5) {             // Change the target palette to a random one every 5 seconds.
    targetNoisePalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }

  noise16_2();
  
  // Mirror to all
  copyRingToAll3();
 
}

void fire18()
{
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy( random());

  boolean cooldown = false;
  EVERY_N_SECONDS(5) { adjCooling(cooldown); } 
  EVERY_N_SECONDS(2) { adjSparking(cooldown); } 

  EVERY_N_SECONDS(60) { cooldown = !cooldown; }
  EVERY_N_SECONDS(120) { 
    if (gPal == gPalHeat) {
      gPal = gPalBlue;
    } else {
      gPal = gPalHeat;
    }
  
  }
  
  Fire2012WithPalette(); // run simulation frame, using palette colors
  
  // mirror and blur the other side
  leds(AVG_LEDS_PER_PETAL/2, AVG_LEDS_PER_PETAL-1) = leds(AVG_LEDS_PER_PETAL/2-1,0);

  copyPetalToAll18();
}

void christmasSparkles() {
  //"Background" color for non-sparkling pixels.
  CRGB sparkleBgColor = CHSV(50, 30, 40);  // dim white
  //CRGB sparkleBgColor = CHSV(96, 200, 30);  // dim green
  
  EVERY_N_MILLISECONDS(40){
    if( random8() < 60 ) {  // How much to sparkle!  Higher number is more.
      pick = random16(AVG_LEDS_PER_PETAL);
      if (ledsData[pick][3] == 0 ) {
        ledsData[pick][3] = 35;  // Used to tag pixel as sparkling
        uint8_t randomPick = random8(5);
        if (randomPick == 0) {
          ledsData[pick][0] = 178;   // sparkle hue (blue)
          ledsData[pick][1] = 244;  // sparkle saturation
          ledsData[pick][2] = 210;  // sparkle value
        }
        if (randomPick == 1) {
          ledsData[pick][0] = 10;  // sparkle hue (red)
          ledsData[pick][1] = 255;  // sparkle saturation
          ledsData[pick][2] = 240;  // sparkle value
        }
        if (randomPick == 2) {
          ledsData[pick][0] = 0;  // sparkle hue (white-ish)
          ledsData[pick][1] = 25;  // sparkle saturation
          ledsData[pick][2] = 255;  // sparkle value
        }
        if (randomPick == 3) {
          ledsData[pick][0] = 35;   // sparkle hue (orange)
          ledsData[pick][1] = 235;  // sparkle saturation
          ledsData[pick][2] = 245;  // sparkle value
        }
        if (randomPick == 4) {
          ledsData[pick][0] = 190;  // sparkle hue (purple)
          ledsData[pick][1] = 255;  // sparkle saturation
          ledsData[pick][2] = 238;  // sparkle value
        }
        leds[pick] = CHSV(ledsData[pick][0], ledsData[pick][1], ledsData[pick][2]);
      }
    }
    for (uint16_t i=0; i < AVG_LEDS_PER_PETAL; i++) {
      if (ledsData[i][3] == 0) {  // if not sparkling, set to "back ground" color
        leds[i] = sparkleBgColor;
      } else {
        CHSV hsv = rgb2hsv_approximate(leds[i]);  // Used to get approximate Hue
        EVERY_N_MILLISECONDS(38) { ledsData[i][0] = hsv.hue - 1; }  // slightly shift hue
        ledsData[i][2] = scale8(ledsData[i][2], 245);  // slowly darken
        leds[i] = CHSV(ledsData[i][0], ledsData[i][1], ledsData[i][2]);
        ledsData[i][3] = ledsData[i][3] - 1;  // countdown sparkle tag
      }
    }
  }
  // Mirror to all
  copyPetalToAll18();
}//end christmasSparkles


//---------------------------------------------------------------
void christmasSparklesRG() {  // Red and Green only
  //"Background" color for non-sparkling pixels.  Can be set to black for no bg color.
  CRGB sparkleBgColor = CHSV(0, 0, 0);  // black
  //CRGB sparkleBgColor = CHSV(50, 30, 30);  // dim white
 
  EVERY_N_MILLISECONDS(40){
    if( random8() < 110 ) {  // How much to sparkle!  Higher number is more.
      pick = random16(AVG_LEDS_PER_PETAL);
      if (ledsData[pick][3] == 0 ) {
        ledsData[pick][3] = 65;  // Used to tag pixel as sparkling
        uint8_t randomPick = random8(2);
        if (randomPick == 0) {
          ledsData[pick][0] = 16;  // sparkle hue (red)
          ledsData[pick][1] = 253;  // sparkle saturation
          ledsData[pick][2] = 242;  // sparkle value
        }
        if (randomPick == 1) {
          ledsData[pick][0] = 96;   // sparkle hue (green)
          ledsData[pick][1] = 230;  // sparkle saturation
          ledsData[pick][2] = 255;  // sparkle value
        }
        leds[pick] = CHSV(ledsData[pick][0], ledsData[pick][1], ledsData[pick][2]);
      }
    }
    for (uint16_t i=0; i < AVG_LEDS_PER_PETAL; i++) {
      if (ledsData[i][3] == 0) {  // if not sparkling, set to "back ground" color
        leds[i] = sparkleBgColor;
      } else {
        CHSV hsv = rgb2hsv_approximate(leds[i]);  // Used to get approximate Hue
        EVERY_N_MILLISECONDS(50) { ledsData[i][0] = hsv.hue - 1; }  // slightly shift hue
        ledsData[i][2] = scale8(ledsData[i][2], 253);  // slowly darken
        leds[i] = CHSV(ledsData[i][0], ledsData[i][1], ledsData[i][2]);
        ledsData[i][3] = ledsData[i][3] - 1;  // countdown sparkle tag
      }
    }
  }
  // Mirror to all
  copyPetalToAll18();
}//end christmasSparklesRG


//---------------------------------------------------------------
void christmasSparklesBP() {  // Blues and Purple only
  //"Background" color for non-sparkling pixels.
  CRGB sparkleBgColor = CHSV(96, 185, 30);  // green
  
  EVERY_N_MILLISECONDS(40){
    if( random8() < 170 ) {  // How much to sparkle!  Higher number is more.
      pick = random16(AVG_LEDS_PER_PETAL);
      if (ledsData[pick][3] == 0 ) {
        ledsData[pick][3] = 20;  // Used to tag pixel as sparkling
        uint8_t randomPick = random8(3);
        if (randomPick == 0) {
          ledsData[pick][0] = 165;   // sparkle hue (blue)
          ledsData[pick][1] = 180;  // sparkle saturation
          ledsData[pick][2] = 230;  // sparkle value
        }
        if (randomPick == 1) {
          ledsData[pick][0] = 200;  // sparkle hue (pink-light-purple)
          ledsData[pick][1] = 170;  // sparkle saturation
          ledsData[pick][2] = 240;  // sparkle value
        }
        if (randomPick == 2) {
          ledsData[pick][0] = 130;  // sparkle hue (light blue)
          ledsData[pick][1] = 200;  // sparkle saturation
          ledsData[pick][2] = 255;  // sparkle value
        }
        leds[pick] = CHSV(ledsData[pick][0], ledsData[pick][1], ledsData[pick][2]);
      }
    }
    for (uint16_t i=0; i < AVG_LEDS_PER_PETAL; i++) {
      if (ledsData[i][3] == 0) {  // if not sparkling, set to "back ground" color
        leds[i] = sparkleBgColor;
      } else {
        CHSV hsv = rgb2hsv_approximate(leds[i]);  // Used to get approximate Hue
        EVERY_N_MILLISECONDS(20) { ledsData[i][0] = hsv.hue - 1; }  // slightly shift hue
        ledsData[i][2] = scale8(ledsData[i][2], 242);  // slowly darken
        leds[i] = CHSV(ledsData[i][0], ledsData[i][1], ledsData[i][2]);
        ledsData[i][3] = ledsData[i][3] - 1;  // countdown sparkle tag
      }
    }
  }
  // Mirror to all
  copyPetalToAll18();
}//end christmasSparklesBP


uint8_t lightningFrequency = 50;                                       // controls the interval between strikes
uint8_t flashes = 8;                                          //the upper limit of flashes per strike
unsigned int dimmer = 1;

uint8_t lightningStart;                                             // Starting location of a flash
uint8_t lightningLen;                                               // Length of a flash

void lightning() {
  
  lightningStart = random8(AVG_LEDS_PER_RING);                               // Determine starting location of flash
  lightningLen = random8(AVG_LEDS_PER_RING-lightningStart);                        // Determine length of flash (not to go beyond NUM_LEDS-1)
  
  for (int flashCounter = 0; flashCounter < random8(3,flashes); flashCounter++) {
    if(flashCounter == 0) dimmer = 5;                         // the brightness of the leader is scaled down by a factor of 5
    else dimmer = random8(1,3);                               // return strokes are brighter than the leader
    
    fill_solid(leds+lightningStart,lightningLen,CHSV(255, 0, 255/dimmer));
    mirrorImageRingToAll3();
    FastLED.show();                       // Show a section of LED's
    delay(random8(4,10));                                     // each flash only lasts 4-10 milliseconds
    fill_solid(leds+lightningStart,lightningLen,CHSV(255,0,0));           // Clear the section of LED's
    mirrorImageRingToAll3();
    FastLED.show();
    
    if (flashCounter == 0) delay (150);                       // longer delay until next flash after the leader
    
    delay(50+random8(100));                                   // shorter delay between strokes  
  } // for()
  
  delay(random8(lightningFrequency)*100);                              // delay between strikes
  
  
} // lightning()

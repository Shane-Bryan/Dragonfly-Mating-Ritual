#include <FastLED.h>

#define LED_TYPE    DOTSTAR
#define COLOR_ORDER BGR

const int  PETAL_RING_0_CLOCK_PIN = 3;
const int  PETAL_RING_0_DATA_PIN = 2;
const int  PETAL_RING_1_CLOCK_PIN = 5;
const int  PETAL_RING_1_DATA_PIN = 4;
const int  PETAL_RING_2_CLOCK_PIN = 7;
const int  PETAL_RING_2_DATA_PIN = 6;

const int PETALS_PER_ROW = 6;
const int SPARS_PER_ROW = 6;

const int NUM_LEDS_PETAL_RING_0 = 900;
const int NUM_LEDS_PETAL_RING_1 = 900;
const int NUM_LEDS_PETAL_RING_2 = 900;

#define TOTAL_LEDS    2700
#define AVG_LEDS_PER_PETAL 150

#define BRIGHTNESS  240
#define FRAMES_PER_SECOND 300

bool gReverseDirection = false;

CRGBArray<TOTAL_LEDS> leds;

// Fire2012 with programmable Color Palette
//
// This code is the same fire simulation as the original "Fire2012",
// but each heat cell's temperature is translated to color through a FastLED
// programmable color palette, instead of through the "HeatColor(...)" function.
//
// Four different static color palettes are provided here, plus one dynamic one.
// 
// The three static ones are: 
//   1. the FastLED built-in HeatColors_p -- this is the default, and it looks
//      pretty much exactly like the original Fire2012.
//
//  To use any of the other palettes below, just "uncomment" the corresponding code.
//
//   2. a gradient from black to red to yellow to white, which is
//      visually similar to the HeatColors_p, and helps to illustrate
//      what the 'heat colors' palette is actually doing,
//   3. a similar gradient, but in blue colors rather than red ones,
//      i.e. from black to blue to aqua to white, which results in
//      an "icy blue" fire effect,
//   4. a simplified three-step gradient, from black to red to white, just to show
//      that these gradients need not have four components; two or
//      three are possible, too, even if they don't look quite as nice for fire.
//
// The dynamic palette shows how you can change the basic 'hue' of the
// color palette every time through the loop, producing "rainbow fire".

CRGBPalette16 gPal;

CRGBPalette16 gPalHeat;
CRGBPalette16 gPalBlue;

//
// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100 
#define INITIAL_COOLING  75
#define MIN_COOLING  40
#define MAX_COOLING  120

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define INITIAL_SPARKING 10
#define MAX_SPARKING 140
#define MIN_SPARKING 30


int cooling = INITIAL_COOLING;
int sparking = INITIAL_SPARKING;

void setup() {
  delay(3000); // sanity delay
  // tell FastLED about the LED strip configuration
  // Sigh.  So hacky to specify ranges this way, but it's quick.
  FastLED.addLeds<LED_TYPE,PETAL_RING_0_DATA_PIN, PETAL_RING_0_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, 0, NUM_LEDS_PETAL_RING_0).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,PETAL_RING_1_DATA_PIN, PETAL_RING_1_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PETAL_RING_0, NUM_LEDS_PETAL_RING_1).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,PETAL_RING_2_DATA_PIN, PETAL_RING_2_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PETAL_RING_0 + NUM_LEDS_PETAL_RING_1, NUM_LEDS_PETAL_RING_2).setCorrection( TypicalLEDStrip );

  FastLED.setBrightness( BRIGHTNESS );

  // This first palette is the basic 'black body radiation' colors,
  // which run from black to red to bright yellow to white.
  gPal = HeatColors_p;
  
  // These are other ways to set up the color palette for the 'fire'.
  // First, a gradient from black to red to yellow to white -- similar to HeatColors_p
  //  gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::Yellow, CRGB::White);
  
  // Second, this palette is like the heat colors, but blue/aqua instead of red/yellow
  gPalHeat = HeatColors_p;
  gPalBlue = CRGBPalette16( CRGB::Black, CRGB::Purple, CRGB::Aqua,  CRGB::White);
  
  // Third, here's a simpler, three-step gradient, from black to red to white
  //   gPal = CRGBPalette16( CRGB::Black, CRGB::Red, CRGB::White);
  
  fill_solid(leds, TOTAL_LEDS, CRGB::Black);                    // Just to be sure, let's really make it BLACK.
  FastLED.show();                         // Power managed display

}



// Fire2012 by Mark Kriegsman, July 2012
// as part of "Five Elements" shown here: http://youtu.be/knWiGsmgycY
//// 
// This basic one-dimensional 'fire' simulation works roughly as follows:
// There's a underlying array of 'heat' cells, that model the temperature
// at each point along the line.  Every cycle through the simulation, 
// four steps are performed:
//  1) All cells cool down a little bit, losing heat to the air
//  2) The heat from each cell drifts 'up' and diffuses a little
//  3) Sometimes randomly new 'sparks' of heat are added at the bottom
//  4) The heat from each cell is rendered as a color into the leds array
//     The heat-to-color mapping uses a black-body radiation approximation.
//
// Temperature is in arbitrary units from 0 (cold black) to 255 (white hot).
//
// This simulation scales it self a bit depending on NUM_LEDS; it should look
// "OK" on anywhere from 20 to 100 LEDs without too much tweaking. 
//
// I recommend running this simulation at anywhere from 30-100 frames per second,
// meaning an interframe delay of about 10-35 milliseconds.
//
// Looks best on a high-density LED setup (60+ pixels/meter).
//

void loop() {
  fire18();
  
  
  FastLED.show(); // display this frame
}

void fire18()
{
  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  
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

  // mirror on all 18 petals
  for (int i = 1; i < 18; i++) {
    leds( i * AVG_LEDS_PER_PETAL, ((i + 1) *AVG_LEDS_PER_PETAL) -1) = leds( (i -1) * AVG_LEDS_PER_PETAL, (i * AVG_LEDS_PER_PETAL) -1 );
  }
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
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (numLeds-1) - j;
      } else {
        pixelnumber = j;
      }
      leds[pixelnumber] = color;
      //leds[NUM_LEDS -1 - pixelnumber] = color;

      
    }
}


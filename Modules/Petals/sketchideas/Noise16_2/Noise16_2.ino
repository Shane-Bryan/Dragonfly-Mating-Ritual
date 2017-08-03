/* noise16_2
 * 
 * By: Andrew Tuline
 *
 * Date: January, 2017
 *
 * A 16 bit noise routine with palettes. This was significantly modified from a routine originally by Stefan Petrick.
 * 
 */

#include "FastLED.h"                                          // FastLED library.

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// Fixed definitions cannot change on the fly.

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
#define COLOR_ORDER BGR                                       // It's GRB for WS2812 and BGR for APA102.
#define LED_TYPE APA102                                       // Using APA102, WS2812, WS2801. Don't forget to modify LEDS.addLeds to suit.
#define TOTAL_LEDS 2700                                          // Number of LED's.

uint8_t max_bright = 128;

CRGBArray<TOTAL_LEDS> leds; // The master array of all LEDs

CRGBPalette16 currentPalette;
CRGBPalette16 targetPalette;
TBlendType    currentBlending;                                // NOBLEND or LINEARBLEND

uint8_t maxChanges = 24;      // Number of tweens between palettes.



void setup() {

  Serial.begin(57600);                                        // Initialize serial port for debugging.
  delay(1000);                                                // Soft startup to ease the flow of electrons.
   
  // tell FastLED about the LED strip configuration
  // Sigh.  So hacky to specify ranges this way, but it's quick.
  FastLED.addLeds<LED_TYPE,PETAL_RING_0_DATA_PIN, PETAL_RING_0_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, 0, NUM_LEDS_PETAL_RING_0).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,PETAL_RING_1_DATA_PIN, PETAL_RING_1_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PETAL_RING_0, NUM_LEDS_PETAL_RING_1).setCorrection( TypicalLEDStrip );
  FastLED.addLeds<LED_TYPE,PETAL_RING_2_DATA_PIN, PETAL_RING_2_CLOCK_PIN, COLOR_ORDER, DATA_RATE_MHZ(1)>(leds, NUM_LEDS_PETAL_RING_0 + NUM_LEDS_PETAL_RING_1, NUM_LEDS_PETAL_RING_2).setCorrection( TypicalLEDStrip );

  currentPalette = PartyColors_p;
  currentBlending = LINEARBLEND;  
  
  FastLED.setBrightness(max_bright);
  set_max_power_in_volts_and_milliamps(5, 500);               // FastLED Power management set at 5V, 500mA.

} // setup()



void loop() {

  EVERY_N_MILLISECONDS(50) {
    nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);  // Blend towards the target palette
  }

  EVERY_N_SECONDS(5) {             // Change the target palette to a random one every 5 seconds.
    targetPalette = CRGBPalette16(CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 255, random8(128,255)), CHSV(random8(), 192, random8(128,255)), CHSV(random8(), 255, random8(128,255)));
  }

  noise16_2();
  
  FastLED.show();
  
} // loop()




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

    leds[i] = ColorFromPalette(currentPalette, index, bri, LINEARBLEND);   // With that value, look up the 8 bit colour palette value and assign it to the current LED.

  }
  
} // noise16_2()




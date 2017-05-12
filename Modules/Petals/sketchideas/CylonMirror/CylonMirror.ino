#include "FastLED.h"

// How many leds in your strip?
#define NUM_LEDS 150 

// For led chips like Neopixels, which have a data line, ground, and power, you just
// need to define DATA_PIN.  For led chipsets that are SPI based (four wires - data, clock,
// ground, and power), like the LPD8806, define both DATA_PIN and CLOCK_PIN
#define DATA_PIN 3
#define CLOCK_PIN 8
#define LED_TYPE    APA102
#define COLOR_ORDER BGR
#define BRIGHTNESS          128

// Define the array of leds
CRGB leds[NUM_LEDS];

void setup() { 
	Serial.begin(57600);
	Serial.println("resetting");
  FastLED.addLeds<LED_TYPE,DATA_PIN,CLOCK_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalLEDStrip );
	LEDS.setBrightness(BRIGHTNESS);
}

void fadeall() { for(int i = 0; i < NUM_LEDS; i++) { leds[i].nscale8(250); } }

void loop() { 
  int ledsInPattern = NUM_LEDS/2;
  int interval = 25;
	static uint8_t hue = 0;
	Serial.print("x");
	// First slide the led in one direction
	for(int i = 0; i < ledsInPattern; i++) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
    leds[NUM_LEDS - 1 -i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show(); 
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
		FastLED.delay(interval);
	}
	Serial.print("x");

	// Now go in the other direction.  
	for(int i = (ledsInPattern)-1; i >= 0; i--) {
		// Set the i'th led to red 
		leds[i] = CHSV(hue++, 255, 255);
    leds[NUM_LEDS - 1 -i] = CHSV(hue++, 255, 255);
		// Show the leds
		FastLED.show();
		// now that we've shown the leds, reset the i'th led to black
		// leds[i] = CRGB::Black;
		fadeall();
		// Wait a little bit before we loop around and do it again
    FastLED.delay(interval);
	}
}

#include <Arduino.h>
#include <FastLED.h>
#include "display.h"

// Information about the LED strip itself
#define LED_PIN     5
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define BRIGHTNESS  255

displayBuffer buffer0;
displayBuffer buffer1;

bool testState = false;

void setup() {
	
	Serial.begin(9600);
    Serial.println("Serial started successfully.");
    
    delay(3000); // power-up safety delay
    FastLED.addLeds<CHIPSET, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection( TypicalSMD5050 );
	FastLED.setMaxPowerInVoltsAndMilliamps(5,1000); 
    FastLED.setBrightness( BRIGHTNESS );

    initDisplayBuffer(&buffer0);
    initDisplayBuffer(&buffer1);

    initBufferState(&buffer0, &buffer1);
	writeNumber(getActiveBuffer(), 1234);

}

void loop() {
	
	static int count = 0;
	writeNumber(getActiveBuffer(), count);
	setBicolorRainbow(getActiveBuffer(), count % 255, 100, 32, 255);
	drawDisplay(getActiveBuffer(), leds);
	count++;
	FastLED.delay(20);

}
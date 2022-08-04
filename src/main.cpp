#include <Arduino.h>
#include "display.h"
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

// Information about the LED strip itself
#define LED_PIN     5
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define BRIGHTNESS  255

displayBuffer buffer0;
displayBuffer buffer1;

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";

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

	for(int i = 0; i < 90; i++){
		static int num = 0;
		
		writeNumber(getActiveBuffer(), num * 1111);
		setBicolorRainbow(getActiveBuffer(), i % 255, 100, 32, 255);
		drawDisplay(getActiveBuffer(), leds);
		
		if((i % 10) == 0){
			num++;
		}
		
		FastLED.delay(40);
	}
	
	WiFiManager wifiManager;
	wifiManager.autoConnect("SC-Clock");

	if (WiFi.status() != WL_CONNECTED) {
		writeNumber(getActiveBuffer(), 1111);
		setBicolorRainbow(getActiveBuffer(), 160, 0, 80, 255);
		drawDisplay(getActiveBuffer(), leds);
		FastLED.delay(1000);
	}
	else{
		writeNumber(getActiveBuffer(), 2222);
		setBicolorRainbow(getActiveBuffer(), 96, 0, 80, 255);
		drawDisplay(getActiveBuffer(), leds);
		FastLED.delay(1000);
	}

}

void loop() {
	
	static int count = 0;
	writeNumber(getActiveBuffer(), count);
	setBicolorRainbow(getActiveBuffer(), count % 255, 100, 32, 255);
	drawDisplay(getActiveBuffer(), leds);
	
	
	// for(int i = 0; i < 70; i++){
	// 	if(i == count % 70){
	// 		leds[i] = CRGB::Red;
	// 		leds[i+10] = CRGB::Red;
	// 	}
	// 	else{
	// 		leds[i] = CRGB::Black;
	// 	}
	// }
	
	
	// FastLED.show();
	count++;
	delay(20);

}
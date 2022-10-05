#include <Arduino.h>
#include "display.h"
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "clock.h"

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

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServerName, -14400, 6000000);	// -4 hours, 1 hour update interval

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
		
		delay(10);
	}
	
	WiFiManager wifiManager;
	wifiManager.autoConnect("SC-Clock");

	if (WiFi.status() != WL_CONNECTED) {
		writeNumber(getActiveBuffer(), 1111);
		setBicolorRainbow(getActiveBuffer(), 160, 0, 80, 255);
		drawDisplay(getActiveBuffer(), leds);
		delay(1000);
	}
	
	if (!wifiManager.autoConnect()) {
		Serial.println("failed to connect and hit timeout");
		//reset and try again, or maybe put it to deep sleep
		writeNumber(getActiveBuffer(), 0);
		setBicolorRainbow(getActiveBuffer(), 0, 0, 80, 255);
		drawDisplay(getActiveBuffer(), leds);
		delay(5000);
		ESP.restart();
		delay(1000);
	}

	
	// Connection success!
	writeNumber(getActiveBuffer(), 2222);
	setBicolorRainbow(getActiveBuffer(), 96, 0, 80, 255);
	drawDisplay(getActiveBuffer(), leds);
	delay(1000);

	timeClient.begin();
	timeClient.update();

	

}

void loop() {
	
	static int count = 0;
	writeNumber(getActiveBuffer(), formatTime(timeClient.getHours(), timeClient.getMinutes()));
	setBicolorRainbow(getActiveBuffer(), count % 255, 100, 40, 255);
	drawDisplay(getActiveBuffer(), leds);
	
	//Serial.print("Hour:"); Serial.print(timeClient.getHours()); Serial.print(" | Formatted: "); Serial.println(((timeClient.getHours() % 12) * 100) + timeClient.getMinutes());
	
	count++;
	delay(20);

}
#include <Arduino.h>
//#define ENCODER_DO_NOT_USE_INTERRUPTS
#define ENCODER_OPTIMIZE_INTERRUPTS
#include <Encoder.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic
#include <NTPClient.h>
#include <WiFiUdp.h>
#include "display.h"
#include "clock.h"

// Information about the LED strip itself
#define LED_PIN     5
#define CHIPSET     WS2812B
#define COLOR_ORDER GRB
CRGB leds[NUM_LEDS];

#define DST_SWITCH 				14	// D5
#define NIGHT_MODE_SWITCH 		14	// D6
#define ROTARY_ENCODER_BUTTON 	2 	// D4
#define ROTARY_ENCODER_A      	4 	// D2
#define ROTARY_ENCODER_B      	0 	// D3

Encoder myEnc(ROTARY_ENCODER_A, ROTARY_ENCODER_B);
bool buttonState = false;

#define BRIGHTNESS  255
#define AP_CONFIG_PORTAL_TIMEOUT 180 // Seconds
#define WIFI_CONNECT_ATTEMPT_TIMEOUT 15 // Seconds

displayBuffer buffer0;
displayBuffer buffer1;

void buttonUpdate();
void configModeCallback(WiFiManager *myWiFiManager);

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServerName, -18000, 6000000);	// -5 hours, 1 hour update interval

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

	pinMode(DST_SWITCH, INPUT_PULLUP);
	pinMode(NIGHT_MODE_SWITCH, INPUT_PULLUP);
	pinMode(ROTARY_ENCODER_BUTTON, INPUT_PULLUP); //	D4, rotary encoder knob press
  	// attachInterrupt(ROTARY_ENCODER_BUTTON, buttonUpdate, RISING);



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
	
	WiFi.mode(WIFI_STA);
	WiFi.begin();
	unsigned long retryTime = millis();
	while(WiFi.status() != WL_CONNECTED){
		if(millis() > (retryTime + (WIFI_CONNECT_ATTEMPT_TIMEOUT * 1000))){
			break;
		}
		yield();
		delay(10);
	}

	WiFiManager wifiManager;
	wifiManager.setConfigPortalTimeout(AP_CONFIG_PORTAL_TIMEOUT);
	wifiManager.setAPCallback(configModeCallback);

	if(WiFi.status() != WL_CONNECTED){
		bool status = wifiManager.autoConnect("SC-Clock");
		if (!status) {	// Not connected
			ESP.restart();
		}
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
	static int offset = 0;
	static unsigned long lastUpdate = 0;

	long encoderRaw = myEnc.read() >> 2;
	unsigned long encoderPosition = abs(encoderRaw);
	yield();
	
	if(millis() > (lastUpdate + 5)){
		//Serial.print("Encoder: "); Serial.println(encoderPosition); 
		writeNumber(getActiveBuffer(), formatTime(timeClient.getHours(), timeClient.getMinutes()));
		//writeNumber(getActiveBuffer(), encoderPosition);
		setBicolorRainbow(getActiveBuffer(), offset % 255, 80, encoderPosition % 255, 255);
		//setBicolorRainbow(getActiveBuffer(), encoderPosition, 0, 0, 255);
		drawDisplay(getActiveBuffer(), leds);
		
		if((count % 5) == 0){
			//Serial.print("Hour: "); Serial.print(timeClient.getHours()); Serial.print(" | Formatted: "); Serial.println(((timeClient.getHours() % 12) * 100) + timeClient.getMinutes());
			offset++;
		}
		
		count++;
		lastUpdate = millis();
	}
}

void buttonUpdate(){
	buttonState = true;
	return;
}

void configModeCallback(WiFiManager *myWiFiManager) {
	Serial.println("No wifi network found, entering config mode.");
	// Do light stuff
}
#include <Arduino.h>
//#define ENCODER_DO_NOT_USE_INTERRUPTS
// #define ENCODER_OPTIMIZE_INTERRUPTS
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
#define NIGHT_MODE_SWITCH 		12	// D6
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
int readDSTSwitch();
int nightModeSwitch(int time);
int encoderTurnedInterval(long encoderPos, unsigned long interval);

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServerName, -18000, 6000000);	// -5 hours, 1 hour update interval

// MODES
#define DAY_MODE   0
#define NIGHT_MODE 1
#define DAY_TIME 800
#define NIGHT_TIME 230
int mode = 0;

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



	// for(int i = 0; i < 90; i++){
	// 	static int num = 0;
		
	// 	writeNumber(getActiveBuffer(), num * 1111);
	// 	setBicolorRainbow(getActiveBuffer(), i % 255, 100, 32, 255);
	// 	drawDisplay(getActiveBuffer(), leds);
		
	// 	if((i % 10) == 0){
	// 		num++;
	// 	}
		
	// 	delay(10);
	// }

	bootSequenceEffect(getActiveBuffer(), leds, 0);
	
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
	// writeNumber(getActiveBuffer(), 2222);
	// setBicolorRainbow(getActiveBuffer(), 96, 0, 80, 255);
	// drawDisplay(getActiveBuffer(), leds);
	// delay(1000);
	bootSequenceEffect(getActiveBuffer(), leds, 1);

	timeClient.begin();
	timeClient.update();

	

}

void loop() {
	
	static int count = 0;
	static int offset = 0;

	long encoderRaw = myEnc.read() >> 2;
	unsigned long encoderPosition = abs(encoderRaw);
	yield();


	
	EVERY_N_MILLISECONDS(5) {
		mode = nightModeSwitch(formatTime24(timeClient.getHours(), timeClient.getMinutes(), readDSTSwitch()));
		int time = formatTime(timeClient.getHours(), timeClient.getMinutes(), readDSTSwitch());
		writeNumber(getActiveBuffer(), time);
		
		if(mode == DAY_MODE){
			if(encoderTurnedInterval(encoderPosition, 1500)){
				writeNumber(getActiveBuffer(), (encoderPosition + 100) % 255);
			}		
			setBicolorRainbow(getActiveBuffer(), offset % 255, 80, (encoderPosition + 100) % 255, 255);
			drawDisplay(getActiveBuffer(), leds);
		}
		else if (mode == NIGHT_MODE){
			setBicolorRainbow(getActiveBuffer(), -30, 60, 30, 60);
			if(encoderTurnedInterval(encoderPosition, 1500)){
				lightPane(getActiveBuffer(), 3, 8, 30, 185, 255, 255);
			}
			drawDisplay(getActiveBuffer(), leds);
		}
		
		if((count % 5) == 0){
			//Serial.print("Hour: "); Serial.print(timeClient.getHours()); Serial.print(" | Formatted: "); Serial.println(((timeClient.getHours() % 12) * 100) + timeClient.getMinutes());
			offset++;
		}
		
		count++;
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

int readDSTSwitch(){
	int state = digitalRead(DST_SWITCH);
	return state;
}

int nightModeSwitch(int time){
	int state = digitalRead(NIGHT_MODE_SWITCH);
	return state && (time > NIGHT_TIME) && (time < DAY_TIME);
}


// Return true if the encoder was turned in the last interval (milliseconds).
int encoderTurnedInterval(long encoderPos, unsigned long interval){
	static long lastEncoderPos = encoderPos;
	static int state = false;
	static unsigned long lastUpdate = 0;

	if(encoderPos != lastEncoderPos){
		state = true;
		lastEncoderPos = encoderPos;
		lastUpdate = millis();
	}
	if(millis() > (lastUpdate + interval)){
		state = false;
	}
	
	return state;
}
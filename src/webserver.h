#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "Arduino.h"
#include "ESP8266WiFi.h"
#include <ESP8266HTTPClient.h>
// #define DEBUG_ESP_HTTP_SERVER
#include <ESP8266WebServer.h>
#define CUSTOM 0
#define SLOW_RAINBOW 1

typedef struct effectSettings{
	int effect;
	uint8_t hue;
	uint8_t sat;
	uint8_t val;
	uint8_t offset;
	uint8_t separation;
	unsigned int width;
} effectSettings;

typedef struct webServerData {
	effectSettings settings;
	bool DLST;
	bool nightMode;
	int sleepTime;
	int wakeTime;
	uint8_t sleepBrightness;
} webServerData;



void wifiInit();
void handleRoot();
void handleNotFound();
webServerData* webServerInit();
void webServerHandleClient(webServerData*);

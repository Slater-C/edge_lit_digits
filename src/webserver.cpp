#include "ESP8266WiFi.h"
#include "Arduino.h"

#include "webserver.h"
#include "index.html.h"

webServerData serverData;

ESP8266WebServer server ( 80 );
WiFiClient client;

// void wifiInit() {
  
// 	WiFi.mode(WIFI_AP);
// 	WiFi.softAP("Lobster ROL #001", "lobstah123");

// 	IPAddress myIP = WiFi.softAPIP();
// 	Serial.print("AP IP address: ");
// 	Serial.println(myIP);

// 	Serial.print("ESP Board MAC Address:  ");
// 	Serial.println(WiFi.macAddress());

// }


void handleRoot() {
	char temp[400];
	int sec = millis() / 1000;
	int min = sec / 60;
	int hr = min / 60;

	snprintf ( temp, 400,

	"<html>\
	<head>\
	<meta http-equiv='refresh' content='10'/>\
	<title>ESP32 Demo</title>\
	<style>\
		body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
	</style>\
	</head>\
	<body>\
	<h1>Hello from ESP32!</h1>\
	<p>Uptime: %02d:%02d:%02d</p>\
	<img src=\"/test.svg\" />\
	</body>\
	</html>",

	hr, min % 60, sec % 60
	);

	server.send ( 200, "text/html", MAIN_PAGE_HTML );
}

void handleNotFound() {
	String message = "File Not Found\n\n";
	message += "URI: ";
	message += server.uri();
	message += "\nMethod: ";
	message += ( server.method() == HTTP_GET ) ? "GET" : "POST";
	message += "\nArguments: ";
	message += server.args();
	message += "\n";

	for ( uint8_t i = 0; i < server.args(); i++ ) {
	message += " " + server.argName ( i ) + ": " + server.arg ( i ) + "\n";
	}

	server.send ( 404, "text/plain", message );
}

webServerData* webServerInit(){	
	serverData.settings.effect = 0;
	serverData.settings.hue
	serverData.settings.
	serverData.settings.
	serverData.DLST;
	serverData.nightMode;
	serverData.sleepTime;
	serverData.wakeTime;
	serverData.sleepBrightness;

	
	server.on ( "/", handleRoot );

	// EPOCH
	server.on("/epochTime", HTTP_GET, []() {
		server.send( 200, "text/plain", String(serverData.serverTime));
	});
	server.on("/epochTime", HTTP_POST, []() {
		String value_s = server.arg("value");
		long value = (long)value_s.toInt();
		Serial.println(server.client().remoteIP().toString() + " => Set epoch time to \"" + value_s + "\" = " + String(value));

		serverData.serverTime = value;
		server.send(200, "text/plain", String(serverData.serverTime));
		adjustRTCTime(serverData.serverTime);
	});
	
	// RELEASE
	server.on("/releaseTime", HTTP_GET, []() {
		server.send( 200, "text/plain", String(serverData.releaseTime));
	});
	server.on("/releaseTime", HTTP_POST, []() {
		String value_s = server.arg("value");
		long value = (long)value_s.toInt();
		Serial.println(server.client().remoteIP().toString() + " => Set release time to \"" + value_s + "\" = " + String(value));

		serverData.releaseTime = value;
		server.send(200, "text/plain", String(serverData.releaseTime));
		while(!led_blink(GREEN, 2)){
			yield();
		};
	});

	// CONFIRMED
	server.on("/confirmButton", HTTP_GET, []() {
	server.send ( 200, "text/plain", String(serverData.confirmed));
	});
	server.on("/confirmButton", HTTP_POST, []() {
		String value_s = server.arg("value");
		bool value = value_s.equalsIgnoreCase("true");
		Serial.println(server.client().remoteIP().toString() + " => Set confirmButton state to \"" + value_s + "\" = " + String(value));

		serverData.confirmed = value;
		server.send(200, "text/plain", String(serverData.confirmed));
		if(serverData.confirmed){
			bool status = scheduleAlarm(serverData.releaseTime);
			while(!led_blink(GREEN, 4)){
				yield();
			};
		}
		else {
			clearAlarm1();
		}
	});

	//ATTACH OVERRIDE
	server.on("/attachButton", HTTP_GET, []() {
		// server.send ( 200, "text/plain", String(false));
	});
		server.on("/attachButton", HTTP_POST, []() {
		Serial.println(server.client().remoteIP().toString() + " => Attach button override was pressed!");

		serverData.attachCommand = true;
		// server.send(200, "text/plain", String(false));
	});


	//RELEASE OVERRIDE
	server.on("/releaseButton", HTTP_GET, []() {
		// server.send ( 200, "text/plain", String(false));
	});
	server.on("/releaseButton", HTTP_POST, []() {
		Serial.println(server.client().remoteIP().toString() + " => Release button override was pressed!");

		serverData.releaseCommand = true;
		// server.send(200, "text/plain", String(false));
	});

	// RTC TIME
	server.on("/rtcTime", HTTP_GET, []() {
		server.send( 200, "text/plain", String(epochTimeNow()));
	});

	server.onNotFound ( handleNotFound );
	server.begin();
	Serial.println ( "HTTP server started" );

	return &serverData;
}

void webServerHandleClient(webServerData* webServerDataStruct){
	server.handleClient();
}
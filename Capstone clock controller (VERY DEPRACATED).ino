#include <Encoder.h>
#include "Lixie.h" // Include Lixie Library
#define DATA_PIN   2
#define NUM_LIXIES 4
Lixie lix(DATA_PIN, NUM_LIXIES);
Encoder myEnc(4, 0); //D2, D3
//in use
//0,2,4,5
//D5,D6
//14,12

#include <TimeLib.h>
#include <ESP8266WiFi.h>          //ESP8266 Core WiFi Library (you most likely already have this in your sketch)
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

//---------------------------------------
//const char* WIFI_SSID = "RT-AC66U_B1_68_2G";  //  your network SSID (name)
//const char* WIFI_PASS = "mercy_9927";  //  your network password

const bool HOUR_12 = true;    // 12-hour format
const bool SIX_DIGIT = false; // True if 6-digit clock with seconds
int TIME_OFFSET = -4;     // -5 = Boston, -4 = DLST??
uint16_t bedtime = 100; //24 hour bedtime
uint16_t wakeup  = 600; //24 hour wakeup
//---------------------------------------

// NTP Servers:
static const char ntpServerName[] = "us.pool.ntp.org";
//static const char ntpServerName[] = "time.nist.gov";
//static const char ntpServerName[] = "time-a.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-b.timefreq.bldrdoc.gov";
//static const char ntpServerName[] = "time-c.timefreq.bldrdoc.gov";

//encoder start
byte oldPosition = 127;
byte buttonmodeunc = 0;
byte buttonmode = constrain(buttonmodeunc, 0, 4);  //LIKE THIS FOR TEST - REDUCE BELOW FOUR
boolean isButtonPressed = false;
long lastUpdateMillis = 0;
long millis2;
long savetimer;


//time padding & DST

uint32_t sum;
uint32_t ISO_time;
byte base_hour;
int dstswitch = 14; //daylight savings switch pin (D5)
long waittime = 0;
long waittime2 = 0;

//autoval

boolean t_nightmode;
int AVmode_I;

//display values

uint16_t hue = 255;
byte sat = 255;
byte val = 255;
uint16_t hueday = 255;
byte satday = 255;
byte valday = 255;
bool up;
byte rainbow;

void handleKey() {
  isButtonPressed = true;
}
//encoder end

WiFiUDP Udp;
unsigned int localPort = 8888;  // local port to listen for UDP packets

time_t getNtpTime();
void digitalClockDisplay();
void sendNTPpacket(IPAddress &address);

void setup()
{
  lix.begin(); // Initialize LEDs
  lix.sweep(CHSV(160, 255, 255), 5);
  Serial.begin(115200);



  // wifimanager setup

  WiFiManager wifiManager;  // initialize
  //wifiManager.resetSettings(); //reset settings - for testing
  wifiManager.setAPCallback(configModeCallback); // config on failed attempt
  wifiManager.autoConnect("SC Capstone");  // first parameter is name of access point, second is the password


  // if not connected
  if (WiFi.status() != WL_CONNECTED) {
    lix.color(255, 255, 0);
    lix.write(8888);
  }

  // if something breaks

  if (!wifiManager.autoConnect()) {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    lix.color(255, 0, 0);
    lix.write(800);
    delay(5000);
    ESP.restart();
    delay(1000);
  }

  //if you get here you have connected to the WiFi
  Serial.println("connected...yeey :)");



  // Green on connection success
  lix.sweep(CHSV(96, 255, 255), 5);
  millis2 = millis();

  //encoder attach
  pinMode(2, INPUT_PULLUP); //D4
  attachInterrupt(2, handleKey, RISING); //D4

  lix.color(255, 0, 0);

  Serial.print("IP number assigned by DHCP is ");
  Serial.println(WiFi.localIP());
  Serial.println("Starting UDP");
  Udp.begin(localPort);
  Serial.print("Local port: ");
  Serial.println(Udp.localPort());
  Serial.println("waiting for sync");
  setSyncProvider(getNtpTime);
  setSyncInterval(300);
  
  pinMode(dstswitch, INPUT_PULLUP);
  twenty_four_hour_time();

  hue = 0;
  sat = 255;
  val = 255;  
  hueday = hue;
  satday = sat;
  valday = val;
  
  start_night_brightness();

}

time_t prevDisplay = 0; // when the digital clock was displayed

//called when wifimanager enters config mode

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println(myWiFiManager->getConfigPortalSSID());
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  lix.sweep(CHSV(160, 255, 255), 5);
  lix.color(0, 0, 255);
  lix.write(8888);
}


void hueconstrain(){
    uint16_t underflow = 256; //underflow number, allows modes. must be between 256 and 300.
    if(hue > underflow && hue < 300){
          hue = 0;
        }
    else if(hue == 65535){
          hue = underflow; //make top value for underflow
        }
   else if(hue == underflow + 1){
          hue = underflow; //make top value for underflow
        }
    else if(hue > 300){
          hue = underflow; //make top value for underflow
        }
}

void savevals(){
  if(millis() - savetimer > 12*60*60*1000){
    savetimer = millis();
    //save in here
    Serial.println("We just tried to save!!");
  }
}


void loop()
{

hueconstrain();
        
  //encoder increment
  if (isButtonPressed && millis() - lastUpdateMillis > 400) {
    isButtonPressed = false;
    lastUpdateMillis = millis();

    //increment mode
    buttonmode++;
    millis2 = millis();

    if (buttonmode >= 3) {
      buttonmode = 0;
    }
    //----------UI----------

    uint16_t huemode = buttonmode + 5000;
    uint16_t satmode = buttonmode + 4000;
    uint16_t valmode = buttonmode + 3000;

    switch (buttonmode) {

      //mode hue UI
      case 0:
        lix.write(huemode);
        lix.color(CHSV(hue, sat, val)); //first on
        lix.color(0, 0, 0, 1);        //second off
        lix.color(0, 0, 0, 2);        //third off
        break;

      //mode saturation UI
      case 1:
        lix.write(satmode);
        lix.color(CHSV(hue, sat, val)); //first on
        lix.color(0, 0, 0, 1);        //second off
        lix.color(0, 0, 0, 2);        //third off
        break;

      //mode brightness UI
      case 2:
        lix.write(valmode);
        lix.color(CHSV(hue, sat, val)); //first on
        lix.color(0, 0, 0, 1);        //second off
        lix.color(0, 0, 0, 2);        //third off
        break;
    }





    // lix.write(buttonmode);
    Serial.print("buttonmode = ");
    Serial.println(buttonmode);
  }


if(hue == 256){
if (millis() - waittime > 120) {
    waittime = millis();
    //increment rainbow
    rainbow++;
    //Serial.print("rainbow[0] is "); Serial.println(rainbow[0]);
}
}

  //encoder turn
  byte newposition1 = myEnc.read() / 4;
  if (newposition1 != oldPosition) {
    byte initialposition = newposition1;
    byte finalposition = oldPosition;
    oldPosition = newposition1;
    millis2 = millis();

    //direction?
//Serial.println(myEnc.read());

 if(finalposition > initialposition){
  up = false;
  //myEnc.read() = 200;
  //Serial.println("Encoder value went up");
 }
  if(finalposition < initialposition){
  up = true;
  //myEnc.read() = 200;
  //Serial.println("Encoder value went down");
 }

    //direction.

    //knob turned
    //print vals
    Serial.print("hue: ");
    Serial.println(hue);
    Serial.print("saturation: ");
    Serial.println(sat);
    Serial.print("brightness: ");
    Serial.println(val);
    Serial.print("buttonmode really is: ");
    Serial.println(buttonmode);

    switch (buttonmode) {
      //mode hue
      case 0:
        if(up == true){
          hue++;
        }
        else if(up == false){
          hue--;
        }
        hueconstrain();
        if(hue == 256){
          lix.color(CHSV(rainbow, sat, val));
        }
        lix.color(CHSV(hue, sat, val));
        hueday = hue;
        lix.write(hue + 5000);
        Serial.print("Encoder hue is "); Serial.println(hue);
        break;

      //mode saturation
      case 1:
        if(up == true){
          sat++;
          sat++;
          Serial.println("tried to increment");
        }
        else if(up == false){
          sat--;
          sat--;
          Serial.println("tried to decrement");
        }
        lix.color(CHSV(hue, sat, val));
        satday = sat;
        lix.write(sat + 4000);
        break;

      //mode brightness
      case 2:
        if(up == true){
          val++;
          val++;
          val++;
          val++;
        }
        else if(up == false){
          val--;
          val--;
          val--;
          val--;
        }
        if(val < 40){
          val = 255;
        }
        lix.color(CHSV(hue, sat, val));
        valday = val;
        lix.write(val + 3000);
        break;

      //buttonmode reset
      case 3:
        //modereset();
        break;
      case 4:
        //modereset();
        break;
      case 5:
        //modereset();
        break;

    }   //switchcase end

  }
//core time print
  if (millis() - millis2 > 1500) {
    //time aet
    if (timeStatus() != timeNotSet) {
      if (now() != prevDisplay) { //update the display only if time has changed
        prevDisplay = now();

//core time
        digitalClockDisplay();
        //dstcheck();
        loop_night_brightness();
        Serial.println("-----------------------");
      }
    }
      
    //high resolution color updating
    if (millis() - waittime2 > 100) {
      waittime2 = millis();
          if(hue < 256){
            lix.color(CHSV(hue, sat, val));
          }
          //mode rainbow
          if(hue == 256){
            lix.color(CHSV(rainbow, sat, val));
            lix.write(sum);
            //Serial.println("high res color ran"); 
          } 
    }
  }

}

void digitalClockDisplay()
{
  // Using 1,000,000 as our base number creates zero-padded times.
  sum = 1000000;
  int DaylightOffset;
  //int dstswitch = 14; //daylight savings switch pin (D5)
  int DSTmode = digitalRead(dstswitch);
  //Serial.print("DSTswitch is "); Serial.println(DSTmode);
  if (DSTmode == LOW) {
    //sum -= 10000;
    DaylightOffset = 1;
    ISO_time -= 100; //CHECK THIS - MAY HAVE TO BE 1000
    Serial.print("DCDisplay hue is "); Serial.println(hue);
    //Serial.print("ISO time is "); Serial.println(ISO_time);
  }
  else {
    DaylightOffset = 0;
  }
  // Put the hour on two digits,
  sum += format_hour(hour()- DaylightOffset) * 10000;

  // The minute on two more,
  sum += minute() * 100;
  // and the seconds on two more.
  sum += second();

  //------

  
  //Serial.print("TIME_OFFSET is ");  Serial.println(TIME_OFFSET);
  //------

  // Take out the seconds if we just have a 4-digit clock
  if (SIX_DIGIT == false) {
    sum /= 100;
  }

  //sum = 1819;     //override time

  if(sum >= 34300 && sum <=34360){
    sum -= 33200;
  }

  
  lix.write(sum); // write our formatted long
  Serial.print("Time sum is"); Serial.println(sum);
  twenty_four_hour_time();
  autoval_check();

}


//daylight savings

void dstcheck() {
  int dstswitch = 14; //daylight savings switch pin (D5)
  pinMode(dstswitch, INPUT_PULLUP);
  int DSTmode = digitalRead(dstswitch);
  //Serial.print("DSTswitch is "); Serial.println(DSTmode);
  if (DSTmode == LOW) {

  }
  else {

  }
  Serial.print("TIME_OFFSET is ");  Serial.println(TIME_OFFSET);
}

//end DST

//Auto brightness
void twenty_four_hour_time() {

  int DaylightOffset;
  //int dstswitch = 14; //daylight savings switch pin (D5)
  int DSTmode = digitalRead(dstswitch);
  //Serial.print("DSTswitch is "); Serial.println(DSTmode);
  if (DSTmode == LOW) {
    //sum -= 10000;
    DaylightOffset = 1;
    //ISO_time -= 100; //CHECK THIS - MAY HAVE TO BE 1000
    
  }
  else {
    DaylightOffset = 0;
  }
  
  format_hour(hour() - DaylightOffset);

  ISO_time = 1000000;

  // Put the hour on two digits,
  ISO_time += base_hour * 10000;
  // The minute on two more,
  ISO_time += minute() * 100;
  // and the seconds on two more.
  ISO_time += second();
  //take out seconds
  ISO_time /= 100;
  ISO_time -= 10000;

  if(ISO_time >= 25500 && ISO_time <= 25560){
    ISO_time -= 23200;
  }
  
  Serial.print("24 hour formatted time is"); Serial.println(ISO_time);
}

void autoval_check() {
  int autovalswitch = 12; //Autoval switch pin (D6)
  pinMode(autovalswitch, INPUT_PULLUP);
  int AVmode = digitalRead(autovalswitch);
  if(AVmode_I != AVmode){
    AVmode_I = AVmode;
      Serial.print("AVmode is "); Serial.println(AVmode);
      Serial.print("AVmode_I is "); Serial.println(AVmode);
    if (AVmode == HIGH) {
    //if switch position 1, do
    t_nightmode = true;
        //lix.color(0, 0, 0, 0);        //first off
        //lix.color(0, 0, 0, 1);        //second off
        //lix.color(0, 0, 0, 2);        //third off
        lix.color(25, 0, 255);        //fourth night
        lix.write(bedtime);
          //hueday = hue;
          //satday = sat;
          //valday = val;
          start_night_brightness();
  }
  else {
    //switch position 2, do
    t_nightmode = false;
        //lix.color(0, 0, 0, 0);        //first off
        //lix.color(0, 0, 0, 1);        //second off
        //lix.color(0, 0, 0, 2);        //third off
        lix.color(255, 170, 0);    //fourth night
        lix.write(wakeup);
        start_night_brightness();
     }
  }
  
}

void start_night_brightness() {
  autoval_check();
  if (t_nightmode == true) {
    if(bedtime >= 0 && bedtime < wakeup){ //If bedtime is past wraparound and before wakeup
      if (ISO_time < wakeup && ISO_time >= bedtime) { //time is less than wakeup but greater than bedtime
      //set night time
      nighttime();
      Serial.print("Wakeup is"); Serial.println(wakeup);
      Serial.print("ISO_time is"); Serial.println(ISO_time);
      Serial.println("Bedtime condition 1, time is night.");
      }
      else{
        daytime();
        Serial.println("Bedtime condition 1, time is day.");
      }
    }
    else if(bedtime <= 2400 && bedtime > wakeup){ //If bedtime is before wraparound and greater than wakeup
      if (ISO_time >= bedtime) { //nighttime is greater than bedtime
      //set night time
      nighttime();
      Serial.println("Bedtime condition 2, time is night.");
      }
      if (ISO_time < wakeup){
        nighttime();
        Serial.println("Bedtime condition 2, time is night.");
      }
      else{
        daytime();
        Serial.println("Bedtime condition 2, time is day.");
      }
    }
    else if (t_nightmode == false){
      daytime();
      Serial.println("Nightmode is off, setting to day.");
    }
  }
}

void loop_night_brightness() {
  autoval_check();
  //Serial.print("Loop ISO_time is"); Serial.println(ISO_time);
  if (t_nightmode == true) {
    if (ISO_time == bedtime) {
      //set night time
      nighttime();
    }
    if (ISO_time == wakeup) {
      //set morning
      daytime();
    }

    if (ISO_time == 1036) {
      //set morning
      //daytime();
    }
  }
  if (t_nightmode == false){ //if stuff breaks take this away
      daytime();
      Serial.println("Nightmode is off, setting to day.");
    }
}

void daytime() {
  hue = hueday;
  sat = satday;
  val = valday;
  Serial.println("Daytime");

}

void nighttime() {
  hue = 0;
  sat = 255;
  val = 130;
  Serial.println("Nightime");
  Serial.print("nighttime() hue is "); Serial.println(hue);
}

//end auto brightness


//buttonmode reset
void modereset() {
  buttonmode = 0;
}


/*-------- NTP code ----------*/

const int NTP_PACKET_SIZE = 48; // NTP time is in the first 48 bytes of message
byte packetBuffer[NTP_PACKET_SIZE]; //buffer to hold incoming & outgoing packets

time_t getNtpTime()
{
  IPAddress ntpServerIP; // NTP server's ip address

  while (Udp.parsePacket() > 0) ; // discard any previously received packets
  Serial.println("Transmit NTP Request");
  // get a random server from the pool
  WiFi.hostByName(ntpServerName, ntpServerIP);
  Serial.print(ntpServerName);
  Serial.print(": ");
  Serial.println(ntpServerIP);
  sendNTPpacket(ntpServerIP);
  uint32_t beginWait = millis();
  while (millis() - beginWait < 1500) {
    int size = Udp.parsePacket();
    if (size >= NTP_PACKET_SIZE) {
      Serial.println("Receive NTP Response");
      Udp.read(packetBuffer, NTP_PACKET_SIZE);  // read packet into the buffer
      unsigned long secsSince1900;
      // convert four bytes starting at location 40 to a long integer
      secsSince1900 =  (unsigned long)packetBuffer[40] << 24;
      secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
      secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
      secsSince1900 |= (unsigned long)packetBuffer[43];
      return secsSince1900 - 2208988800UL + TIME_OFFSET * SECS_PER_HOUR;
      Serial.print("secsSince1900="); Serial.println(secsSince1900);
      Serial.print("SECS_PER_HOUR"); Serial.println(SECS_PER_HOUR);
    }
  }
  Serial.println("No NTP Response :-(");
  return 0; // return 0 if unable to get the time
}

// send an NTP request to the time server at the given address
void sendNTPpacket(IPAddress &address)
{
  // set all bytes in the buffer to 0
  memset(packetBuffer, 0, NTP_PACKET_SIZE);
  // Initialize values needed to form NTP request
  // (see URL above for details on the packets)
  packetBuffer[0] = 0b11100011;   // LI, Version, Mode
  packetBuffer[1] = 0;     // Stratum, or type of clock
  packetBuffer[2] = 6;     // Polling Interval
  packetBuffer[3] = 0xEC;  // Peer Clock Precision
  // 8 bytes of zero for Root Delay & Root Dispersion
  packetBuffer[12] = 49;
  packetBuffer[13] = 0x4E;
  packetBuffer[14] = 49;
  packetBuffer[15] = 52;
  // all NTP fields have been given values, now
  // you can send a packet requesting a timestamp:
  Udp.beginPacket(address, 123); //NTP requests are to port 123
  Udp.write(packetBuffer, NTP_PACKET_SIZE);
  Udp.endPacket();
}

byte format_hour(byte hr) {
  base_hour = hr;
  //dstcheck();
  //Serial.print("Base hour is"); Serial.println(base_hour);
  //base_hour = base_hour * 1000;
  //base_hour = base_hour + minute();
  //Serial.print("Base hour formatted is"); Serial.println(base_hour);

  if (hr > 12) {
    hr -= 12;
  }
  if (hr == 0) {
    hr = 12;
  }
  return hr;
}

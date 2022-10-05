#pragma once
#include <Arduino.h>
#include <stdint.h>
#include <stdbool.h>
#include <TimeLib.h>
#include <WiFiUdp.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic

int formatTime(int hour, int minute);
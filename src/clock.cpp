#include "clock.h"
#include <cstddef>
#include <NTPClient.h>

int formatTime(int hour, int minute, unsigned int offset_hours){
	hour = hour + (offset_hours % 12);
	hour = (hour > 12) ? (hour - 12) : hour;
	hour = (hour == 0) ? (hour = 12) : hour;
	int time =  (hour * 100) + minute;

	return time;
}

int formatTime24(int hour, int minute, unsigned int offset_hours){
	hour = (hour + offset_hours) % 24;
	int time =  (hour * 100) + minute;

	return time;
}
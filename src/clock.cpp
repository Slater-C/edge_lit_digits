#include "clock.h"
#include <cstddef>
#include <NTPClient.h>

int formatTime(int hour, int minute){
	hour = (hour > 12) ? (hour - 12) : hour;
	int time =  (hour * 100) + minute;

	return time;
}
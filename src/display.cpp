#include "display.h"
#include <cstddef>


bufferState activeBuffer;



void initDisplayBuffer(displayBuffer* buffer){
	
	for (int i = 0; i < LEDS_PER_DIGIT*DIGITS; i++){

		buffer->layer0[i].hue = 0;
		buffer->layer0[i].sat = 0;
		buffer->layer0[i].val = 0;
		buffer->layer0[i].transparent = true;

	}

	// This function sets up an empty display buffer
}


void initBufferState(displayBuffer* buffer0, displayBuffer* buffer1){
	activeBuffer.buffers[0] = buffer0;
	activeBuffer.buffers[1] = buffer1;
	activeBuffer.bufferNumber = 0;
}

void flipBuffers(){
	activeBuffer.bufferNumber = !activeBuffer.bufferNumber;
}

displayBuffer* getActiveBuffer(){
	return activeBuffer.buffers[activeBuffer.bufferNumber];
}

displayBuffer* getInactiveBuffer(){
	return activeBuffer.buffers[!activeBuffer.bufferNumber];
}


void drawDisplay(displayBuffer* buffer, CRGB leds[]){

	uint8_t h;
    uint8_t s;
    uint8_t v;
	static unsigned long lastUpdate = 0;

	if(millis() > (lastUpdate + MAX_UPDATE_INTERVAL)){
		for (int i = 0; i < LEDS_PER_DIGIT*DIGITS; i++){

			h = buffer->layer0[i].hue;
			s = buffer->layer0[i].sat;
			
			if(!buffer->layer0[i].transparent){
				v = buffer->layer0[i].val;
				leds[i].setHSV(h, s, v);
			}
			else{
				leds[i] = CRGB::Black;
			}
		}
		FastLED.show();
		FastLED.delay(5);
		delayMicroseconds(100);
		flipBuffers();
		lastUpdate = millis();
	}

}

void writeNumber(displayBuffer* buffer, uint32_t number){

	for(int i = 0; i < NUM_LEDS; i++){
		buffer->layer0[i].transparent = true;
	}

	for(int digit = 0; digit < (DIGITS); digit++){
		
		int out = number % 10;
		number = number / 10;
		
		buffer->layer0[DIGIT_ADDRESSES[out] + (digit * 20)].transparent = false; 			// Right LED
		buffer->layer0[DIGIT_ADDRESSES[out] + 10 + (digit * 20)].transparent = false;		// Left LED

	}
}

/// @brief Light an individual pane of a certain digit.
/// @param buffer 
/// @param digit Digit index, 0-3. 0 is the least significant digit.
/// @param pane Pane index, 0-9.
/// @param separation Hue separation between left and right LEDs.
/// @param hue 
/// @param sat 
/// @param val 
void lightPane(displayBuffer* buffer, int digit, int pane, uint8_t separation, uint8_t hue, uint8_t sat, uint8_t val){

	// Keep in range
	digit = digit % 4;
	pane = pane % 10;

	// Right LED
	buffer->layer0[DIGIT_ADDRESSES[pane] + (digit * 20)].transparent = false; 			
	buffer->layer0[DIGIT_ADDRESSES[pane] + (digit * 20)].hue = hue;
	buffer->layer0[DIGIT_ADDRESSES[pane] + (digit * 20)].sat = sat;
	buffer->layer0[DIGIT_ADDRESSES[pane] + (digit * 20)].val = val;
	
	// Left LED
	buffer->layer0[DIGIT_ADDRESSES[pane] + 10 + (digit * 20)].transparent = false; 			
	buffer->layer0[DIGIT_ADDRESSES[pane] + 10 + (digit * 20)].hue = hue + separation;
	buffer->layer0[DIGIT_ADDRESSES[pane] + 10 + (digit * 20)].sat = sat;
	buffer->layer0[DIGIT_ADDRESSES[pane] + 10 + (digit * 20)].val = val;
}

/// @brief Set all active LEDs to the static color provided (hue, saturation, brightness)
/// @param buffer 
/// @param hue 
/// @param sat 
/// @param val 
void setColorStatic(displayBuffer* buffer, uint8_t hue, uint8_t sat, uint8_t val){
	for(int i = 0; i < NUM_LEDS; i++){
		
		buffer->layer0[i].hue = hue;
		buffer->layer0[i].sat = sat;
		buffer->layer0[i].val = val;

	}
}


/// @brief A rainbow effect from left to right
/// @param buffer 
/// @param offset An offset to allow the rainbow to scroll and neatly wrap.
/// @param width The width of the rainbow from the leftmost to rightmost LED.
/// @param brightness 
void setColorRainbow(displayBuffer* buffer, uint8_t offset, unsigned int width, uint8_t brightness){
	int delta = width / NUM_LEDS;

	for(int i = 0; i < NUM_LEDS; i++){
		
		buffer->layer0[i].hue = (offset + (delta * i)) % 255;
		buffer->layer0[i].sat = 255;
		buffer->layer0[i].val = brightness;

	}
}

/// @brief A rainbow effect from left to right, where the two LEDs in each digit have a pronounced color shift.
/// @param buffer 
/// @param offset An offset to allow the rainbow to scroll and neatly wrap.
/// @param width The width of the rainbow from the leftmost to rightmost LED.
/// @param separation The difference (in hue) between the two LEDs in an individual digit.
/// @param brightness 
void setBicolorRainbow(displayBuffer* buffer, uint8_t offset, unsigned int width, uint8_t separation, uint8_t brightness){
	int delta = width / NUM_LEDS;

	for(int digit = 0; digit < (DIGITS); digit++){
		
		for(int led = 0; led < LEDS_PER_DIGIT; led++){
			int led_pos = led + digit * 20;

			if(led > 9){
				buffer->layer0[led_pos].hue = (int)(offset + separation + (delta * (led_pos))) % 255;
			}
			else{
				buffer->layer0[led_pos].hue = (int)(offset + (delta * (led_pos))) % 255;
			}
			buffer->layer0[led_pos].sat = 255;
			buffer->layer0[led_pos].val = brightness;

		}
	}
}


// EFFECTS:

void bootSequenceEffect(displayBuffer* buffer, CRGB leds[], int stage){
	// static int startingPanes[10];
	// randomSeed(millis());
	// for(int i = 0; i <= 9; i++){
	// 	startingPanes[i] = random(4);
	// }
	int digit = 0;
	int pane = 0;
	int hue;

	if(stage){
		hue = 96;
	}
	else{
		hue = 160;
	}

	for(int i = 0; i <= 200; i++){

		// Right LED
		buffer->layer0[DIGIT_ADDRESSES[pane] + (digit * 20)].transparent = false; 			
		buffer->layer0[DIGIT_ADDRESSES[pane] + (digit * 20)].hue = hue;
		buffer->layer0[DIGIT_ADDRESSES[pane] + (digit * 20)].sat = 255;
		buffer->layer0[DIGIT_ADDRESSES[pane] + (digit * 20)].val = 255;
		
		// Left LED
		buffer->layer0[DIGIT_ADDRESSES[pane] + 10 + (digit * 20)].transparent = false; 			
		buffer->layer0[DIGIT_ADDRESSES[pane] + 10 + (digit * 20)].hue = hue + 10;
		buffer->layer0[DIGIT_ADDRESSES[pane] + 10 + (digit * 20)].sat = 255;
		buffer->layer0[DIGIT_ADDRESSES[pane] + 10 + (digit * 20)].val = 255;

		drawDisplay(getActiveBuffer(), leds);
		if((i % 9) == 0){
			digit = (digit + 1) % 4;	
		}
		else{
			pane = (pane + 1) % 10;
		}
	} 
}
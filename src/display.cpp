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

	for (int i = 0; i < LEDS_PER_DIGIT*DIGITS; i++){

		h = buffer->layer0[i].hue;
		s = buffer->layer0[i].sat;
		
		if(buffer->layer0[i].transparent){
			v = 0;
		}
		else{
			v = buffer->layer0[i].val;
		}

		leds[i].setHSV(h, s, v);
	}
	FastLED.show();
	flipBuffers();
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

void setColorStatic(displayBuffer* buffer, uint8_t hue, uint8_t sat, uint8_t val){
	for(int i = 0; i < NUM_LEDS; i++){
		
		buffer->layer0[i].hue = hue;
		buffer->layer0[i].sat = sat;
		buffer->layer0[i].val = val;

	}
}

void setColorRainbow(displayBuffer* buffer, uint8_t offset, int width, uint8_t brightness){
	int delta = width / NUM_LEDS;

	for(int i = 0; i < NUM_LEDS; i++){
		
		buffer->layer0[i].hue = (offset + (delta * i)) % 255;
		buffer->layer0[i].sat = 255;
		buffer->layer0[i].val = brightness;

	}
}

void setBicolorRainbow(displayBuffer* buffer, uint8_t offset, int width, uint8_t separation, uint8_t brightness){
	int delta = width / NUM_LEDS;

	for(int digit = 0; digit < (DIGITS); digit++){
		
		for(int led = 0; led < LEDS_PER_DIGIT; led++){
			int led_pos = led + digit * 20;

			if(led > 9){
				buffer->layer0[led_pos].hue = (offset + separation + (delta * (led_pos))) % 255;
			}
			else{
				buffer->layer0[led_pos].hue = (offset + (delta * (led_pos))) % 255;
			}
			buffer->layer0[led_pos].sat = 255;
			buffer->layer0[led_pos].val = brightness;

		}

	}
}
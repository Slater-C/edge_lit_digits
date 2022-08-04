#pragma once
#include <stdint.h>
#include <stdbool.h>
#include <FastLED.h>
#include <math.h>

#define PANES_PER_DIGIT 10
#define LEDS_PER_PANE 2
#define LEDS_PER_DIGIT 20
#define DIGITS 4
#define NUM_LEDS 80

const uint8_t DIGIT_ADDRESSES[10] = {5, 6, 7, 8, 9, 0, 1, 2, 3, 4};



typedef struct pixel {

	bool transparent;
	uint8_t hue;
	uint8_t sat;
	uint8_t val;

} pixel;



typedef struct displayBuffer
{

    pixel layer0[LEDS_PER_DIGIT*DIGITS];  // Display buffer has an extra space for overflow pixels

} displayBuffer;



typedef struct bufferState
{

    uint8_t bufferNumber;
    displayBuffer* buffers[2];

} bufferState;



void initDisplayBuffer(displayBuffer* buffer);
void initBufferState(displayBuffer* buffer0, displayBuffer* buffer1);
void flipBuffers();
displayBuffer* getActiveBuffer();
displayBuffer* getInactiveBuffer();
void drawDisplay(displayBuffer* buffer, CRGB leds[]);
void writeNumber(displayBuffer* buffer, uint32_t number);
void setColorStatic(displayBuffer* buffer, uint8_t hue, uint8_t sat, uint8_t val);
void setColorRainbow(displayBuffer* buffer, uint8_t offset, int width, uint8_t brightness);
void setBicolorRainbow(displayBuffer* buffer, uint8_t offset, int width, uint8_t separation, uint8_t brightness);
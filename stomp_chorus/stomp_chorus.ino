
#include "StompShield.h"

#define B_MAX 100 // max value for rotary encoder input (min freq)
#define B_MIN 1 // min value for rotary encoder input (max freq)

#define MAX_DELAY 400
#define MIN_DELAY 200

uint16_t sDelayBuffer0[MAX_DELAY+400];
uint16_t sDelayBuffer1[MAX_DELAY+400];
unsigned int DelayCounter = 0;
unsigned int Delay_Depth = 300;
unsigned int count_up=1;
int p;
int POT0 = 10;


unsigned int location = 0; // incoming data buffer pointer
byte button; // button checking timer
byte last_state; // last rotary encoder state
byte counter = 4; // rotary encoder rotation counter (and start value)
unsigned int fractional = 0x00; // fractional sample position
int data_buffer; // temporary data storage to give a 1 sample buffer

void checkButton()
{
  button--; // check buttons every so often
  if (button == 0) {
    byte temp3 = (PIND & 0x94); // mask off button pins
    if (((last_state | temp3) & 0x10) == 0) { // falling edge on pin4
      if ((temp3 & 0x04) == 0) { // low value on pin2
        if (counter > B_MIN) counter--; // if not at min, decrement
      }
      else { // high value on pin2
        if (counter < B_MAX) counter++; // if not at max, increment
      }
    }
    last_state = (temp3 ^ 0x94); // invert buttons states and save
    button = 0x20; // reset counter (determines how often buttons are checked)
  }
}

void setup() {
  StompShield_init(); // setup the arduino for the shield
}

void loop() {
  // nothing happens here, all down in the interrupt
}


ISR(TIMER1_OVF_vect) { // all processing happens here

  // output the last value calculated
  OCR1AL = ((data_buffer + 0x8000) >> 8); // convert to unsigned, send out high byte
  OCR1BL = data_buffer; // send out low byte
  
  // get ADC data
  byte temp1 = ADCL; // you need to fetch the low byte first
  byte temp2 = ADCH; // yes it needs to be done this way
  int input = ((temp2 << 8) | temp1) + 0x8000; // make a signed 16b value
  
  checkButton();
 
  sDelayBuffer0[DelayCounter] = input;

   
 
  
  // save value for playback next interrupt
  data_buffer = output; 
}



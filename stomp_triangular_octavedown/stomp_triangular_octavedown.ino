
#include "StompShield.h"

int outBuffer = 0x80;
int output;
int oldVal;
int minVal = 0;
int maxVal = 0;
int currDirection = 0;
int input;
int average;

void setup() {
  StompShield_init(); // setup the arduino for the shield 
//  Serial.begin(9600); 
//  while (!Serial) {
//    ; // wait for serial port to connect. Needed for Leonardo only
//  }  
}

void loop() {
//  delay(1000);
//  Serial.print(input);
//  Serial.print('_');
//  Serial.print(currDirection);
//  Serial.print('~');
//  Serial.print(average);
//  Serial.print('~');
//  Serial.print(minVal);
//  Serial.print('_');
//  Serial.print(maxVal);
//  Serial.print('>');
//  Serial.print(output);
//  Serial.print('\n');
}

ISR(TIMER1_OVF_vect) {  
  // get ADC data
  byte temp1 = ADCL; // you need to fetch the low byte first
  byte temp2 = ADCH; // yes it needs to be done this way
  input = ((temp2 << 8) | temp1) + 0x8000; // make a signed 16b value
  
  if (minVal==0) { // first time initialization;
    oldVal = input;
    output = input;
  }
  
  if (input > maxVal)
    maxVal = +10;
  if (input < minVal)
    minVal = -10;
  maxVal--;
  minVal++;

  
//  average = ((maxVal-minVal)/2)+minVal;
//  if (input < 0) // going /
//    if (oldVal >= 0)
//      currDirection = 0;

  if (input > 0) // going \
    if (oldVal <= 0)
      currDirection = ++currDirection%2;
  
  // produce output data based on currDirection
  if (currDirection) {
    output+=maxVal>>4;
    //output = maxVal;
  }
  else {
    output-=maxVal>>4;
    //output = -maxVal;
  }
  if (output < -5000) output=-5000;
  if (output > 5000) output= 5000;
  
  oldVal = input;
  // save data
  outBuffer = output;
  // output data
  OCR1AL = ((outBuffer >> 8) + 0x80); 
  OCR1BL = outBuffer; // output the bottom byte


}



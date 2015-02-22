
#include "StompShield.h"

#define B_MAX 100 // max value for rotary encoder input (min freq)
#define B_MIN 1 // min value for rotary encoder input (max freq)

#define MIN_OUT -16383
#define MAX_OUT 16383

byte button; // button checking timer
byte last_state; // last rotary encoder state
byte counter = 10; // rotary encoder rotation counter (and start value)

byte looper=0;

int oldInput;
int inMaxVal = 0;
int currDirection = 0;
int passedFirst = 0;
int l1=0;
int l2=0;

int input=0; // new input value
int output=0; // calculated current output
int oldOutput=0; // last output value
int filtered=0; // current filtered value;
int oldFiltered=0; // previousFiltered;

int average;
int outGate = 0;

void setup() {
  StompShield_init(); // setup the arduino for the shield 
  Serial.begin(9600); 
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
}
void loop() {
  delay(1000);
  Serial.print(input);
  Serial.print('_');
  Serial.print(passedFirst);
  Serial.print('_');
  Serial.print(currDirection);
  Serial.print('~');
  Serial.print(outGate);
  Serial.print('~');
  Serial.print(inMaxVal);
  Serial.print('\n');
}

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

void highPassFilter() // frequency unknown ;)
{
  int filterTemp = oldFiltered + output - oldOutput;
    filtered = output - (filterTemp/(2));

//        filteredArray[i] = alpha * (filteredArray[i-1] + data.recordedSamples[i] - data.recordedSamples[i-1]);
}

ISR(TIMER1_OVF_vect)
{
  // get ADC data
  byte temp1 = ADCL; // you need to fetch the low byte first
  byte temp2 = ADCH; // yes it needs to be done this way
  input = ((temp2 << 8) | temp1) + 0x8000; // make a signed 16b value
  
  checkButton();
    
  if (input > inMaxVal)
    inMaxVal++;
  if (looper++ % 10 && input > 0 && input < inMaxVal)
    inMaxVal--;
 
  if (passedFirst) 
  {
    if (input < 0) { // going down
      if (oldInput >= 0) {
        if (currDirection == 0) {
          currDirection = 1;
          passedFirst = 0;
        }
        else
        {
          currDirection = 0;
        }
      }
    }
  }
  else 
  {
    if (input < 0) { // going up
      if (oldInput >= 0) {
        passedFirst = 1;
      }
    }    
  }
  
  // produce output data based on currDirection
  if (currDirection)
    output+=(counter<<5);
  else
    output-=(counter<<5);
  
  //calculate gate according to current input
  //if (outGate > maxVal) outGate = outGate -10;
  //if (outGate < minVal) outGate = outGate +10;
  if (inMaxVal < 1200) {
    outGate-=100;
    if (outGate < 0) outGate=0;
  }
  else 
    outGate = MAX_OUT;
  
  // limit outvalue according to current gate  
  if (output < -outGate) output=-outGate;
  if (output > outGate) output=outGate;
  
  highPassFilter();
  
  // save data
  oldInput = input;
  oldOutput = output;
  oldFiltered = filtered;
  
  // output data
  OCR1AL = ((filtered >> 8) + 0x80); 
  OCR1BL = filtered; // output the bottom byte


}



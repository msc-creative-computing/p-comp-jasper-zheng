/*
   Example using a piezo to change the frequency of a sinewave
   with Mozzi sonification library.

   Demonstrates the use of Oscil to play a wavetable, and analog input for control.

   This example goes with a tutorial on the Mozzi site:
   http://sensorium.github.io/Mozzi/learn/introductory-tutorial/

   The circuit:
     Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or
     check the README or http://sensorium.github.io/Mozzi/

     Piezo on analog pin 3:
       + connection of the piezo attached to the analog pin
       - connection of the piezo attached to ground
       1-megohm resistor between the analog pin and ground

    Mozzi documentation/API
    https://sensorium.github.io/Mozzi/doc/html/index.html

    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users

   Tim Barrass 2013, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator
#include <tables/sin2048_int8.h> // table for Oscils to play
#include <tables/square_no_alias_2048_int8.h>
#include <Smooth.h>
#include <mozzi_midi.h>

// increase the rate of updateControl from the default of 50, to catch the piezo's rapid transients
#define CONTROL_RATE 150

const int PIEZO_PIN_0 = 0;  // set the analog input pin for the piezo
const int PIEZO_PIN_1 = 1;
const int PIEZO_PIN_2 = 2;
const int PIEZO_PIN_3 = 3;
const int PIEZO_PIN_4 = 4;

int voicing = 0;
int frequency_0 = 0;
int frequency_1 = 0;
int frequency_2 = 0;
int frequency_3 = 0;
int frequency_4 = 0;

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSin_0(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSin_1(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSin_2(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSin_3(SQUARE_NO_ALIAS_2048_DATA);
Oscil <SQUARE_NO_ALIAS_2048_NUM_CELLS, AUDIO_RATE> aSin_4(SQUARE_NO_ALIAS_2048_DATA);

//Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin_0(SIN2048_DATA);
//Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin_1(SIN2048_DATA);
//Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin_2(SIN2048_DATA);
//Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin_3(SIN2048_DATA);
//Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin_4(SIN2048_DATA);

//int thred_0 = 600;
//int thred_1 = 40;
//int thred_2 = 600;
//int thred_3 = 650;
//int thred_4 = 650;
//

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values // set up the Serial output so we can look at the piezo values
//  Serial.println("hhhhi");
  startMozzi(CONTROL_RATE); // :)) use the control rate defined above

//  aSin_0.setFreq(221); // a 109
//  aSin_1.setFreq(292); // d3 145
//  aSin_2.setFreq(389); // g3 195
//  aSin_3.setFreq(494); // b 245
//  aSin_4.setFreq(654); // e 329
  
  aSin_0.setFreq(mtof(67)); // a 109
  aSin_1.setFreq(mtof(65)); // d3 145
  aSin_2.setFreq(mtof(64)); // g3 195
  aSin_3.setFreq(mtof(62)); // b 245
  aSin_4.setFreq(mtof(60)); // e 329

//  thred_0 = mozziAnalogRead(PIEZO_PIN_0) + 100; 
//  thred_1 = mozziAnalogRead(PIEZO_PIN_1) + 100; 
//  thred_2 = mozziAnalogRead(PIEZO_PIN_2) + 100; 
//  thred_3 = mozziAnalogRead(PIEZO_PIN_3) + 100; 
//  thred_4 = mozziAnalogRead(PIEZO_PIN_4) + 100; 
//
//  Serial.println(thred_0);
//  Serial.println(thred_1);
//  Serial.println(thred_2);
//  Serial.println(thred_3);
//  Serial.println(thred_4);
  
}


void updateControl(){
  voicing = 0;
  // read the piezo
  int piezo_value_0 = mozziAnalogRead(PIEZO_PIN_0); // value is 0-1023
  int piezo_value_1 = mozziAnalogRead(PIEZO_PIN_1);
  int piezo_value_2 = mozziAnalogRead(PIEZO_PIN_2);
  int piezo_value_3 = mozziAnalogRead(PIEZO_PIN_3);
  int piezo_value_4 = mozziAnalogRead(PIEZO_PIN_4);
  frequency_0 = 0;
  frequency_1 = 0;
  frequency_2 = 0;
  frequency_3 = 0;
  frequency_4 = 0;

  
  if (piezo_value_0 >= 700){
    frequency_0 = 1;
    voicing += 1;
  }
  if (piezo_value_1 >= 850){
    frequency_1 = 1;
    voicing += 1;
  }
  if (piezo_value_2 >= 690){
    frequency_2 = 1;
    voicing += 1;
  }
  if (piezo_value_3 >= 40){
    frequency_3 = 1;
    voicing += 1;
  }
  if (piezo_value_4 >= 650){
    frequency_4 = 1;
    voicing += 1;
  }
  
  Serial.print("0 = ");
  Serial.print(piezo_value_0);
  Serial.print("\t"); // prints 2 tabs
  Serial.print("1 = ");
  Serial.print(piezo_value_1);
  Serial.print("\t"); // prints 2 tabs
  Serial.print("2 = ");
  Serial.print(piezo_value_2);
  Serial.print("\t"); // prints 2 tabs
  Serial.print("3 = ");
  Serial.print(piezo_value_3);
  Serial.print("\t"); // prints 2 tabs
  Serial.print("4 = ");
  Serial.print(piezo_value_4);
  Serial.print("\t"); // prints 2 tabs 

//  int frequency_0 = piezo_value_0*3; // 680
//  int frequency_1 = piezo_value_1*3; // 550
//  int frequency_2 = piezo_value_2*3; // 650
//  int frequency_3 = piezo_value_3*3; // 40
//  int frequency_4 = piezo_value_4*3; // 700
  
  // print the frequency to the Serial monitor for debugging
//  Serial.print("frequency = ");
//  Serial.print(piezo_value_0);

  // set the frequency

  Serial.print(voicing);
  Serial.println(); // next line
}


AudioOutput_t updateAudio(){
  int output_voice = 0;

  if (frequency_0 == 1){
    output_voice += aSin_0.next();
  }
  if (frequency_1 == 1){
    output_voice += aSin_1.next();
  }
  if (frequency_2 == 1){
    output_voice += aSin_2.next();
  }
  if (frequency_3 == 1){
    output_voice += aSin_3.next();
  }
  if (frequency_4 == 1){
    output_voice += aSin_4.next();
  }
  

//  Serial.println(0>>0);
//  Serial.println(output_voice);
  return (output_voice)>>(voicing-1);
}


void loop(){
  audioHook();
}

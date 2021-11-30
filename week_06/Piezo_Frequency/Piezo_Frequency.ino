#include <MozziGuts.h>
#include <Oscil.h> // oscillator
#include <tables/sin2048_int8.h> // table for Oscils to play
#include <tables/square_no_alias_2048_int8.h>
#include <Smooth.h>
#include <mozzi_midi.h>

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

void setup(){
  //Serial.begin(9600); // for Teensy 3.1, beware printout can cause glitches
  Serial.begin(115200); // set up the Serial output so we can look at the piezo values // set up the Serial output so we can look at the piezo values
  startMozzi(CONTROL_RATE); // :)) use the control rate defined above
  
  aSin_0.setFreq(mtof(67)); // a 109
  aSin_1.setFreq(mtof(65)); // d3 145
  aSin_2.setFreq(mtof(64)); // g3 195
  aSin_3.setFreq(mtof(62)); // b 245
  aSin_4.setFreq(mtof(60)); // e 329 
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
  Serial.print(voicing);
  Serial.println();
}


AudioOutput_t updateAudio(){
  int output_voice = 0;

  if (frequency_0 == 1){output_voice += aSin_0.next();}
  if (frequency_1 == 1){output_voice += aSin_1.next();}
  if (frequency_2 == 1){output_voice += aSin_2.next();}
  if (frequency_3 == 1){output_voice += aSin_3.next();}
  if (frequency_4 == 1){output_voice += aSin_4.next();}
  
  return (output_voice)>>(voicing-1);
}


void loop(){
  audioHook();
}

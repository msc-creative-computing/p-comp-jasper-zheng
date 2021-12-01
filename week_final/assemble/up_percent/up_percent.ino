
#include <Wire.h>
#include "Adafruit_MPR121.h"

#ifndef _BV
#define _BV(bit) (1 << (bit)) 
#endif

#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/cos8192_int8.h>
#include <tables/sin2048_int8.h>
#include <mozzi_midi.h>

#include <ReverbTank.h>

#define CONTROL_RATE 128

#define LDR_PIN 35


/* Jar LED and LDR
 * init
 * 
 */


int fade_update_speed = 50;
int fadeAmount [7] = {10,10,10,10,10,10,10};

int fadeValues [7] = {0,0,0,0,0,0,0};
uint8_t led_pins [7] = {13,12,14,27,18,5,32};
const uint8_t led_pins_1 [7] = {13,12,14,27,18,5,32};
const uint8_t led_pins_2 [7] = {14,27,12,13,18,5,32};
const uint8_t led_pins_3 [7] = {27,32,14,13,18,5,12};

const int freq = 5000; 
const int resolution = 13;

/* Moisture
 * init
 * 
 */

const uint8_t mois_pins [3] = {34, 39, 36};
int moisture [3] = {0,0,0};
int moisture_base [3] = {0,0,0};

uint8_t notes_1 [7] = {62,64,69,73,70,80,86};
uint8_t notes_3 [7] = {57,62,66,69,70,80,86};
uint8_t notes_4 [7] = {61,62,66,74,70,80,86};
uint8_t notes_5 [7] = {59,62,64,69,57,80,86};

uint8_t used_notes [4] = {62,64,69,71};

uint8_t notes_2 [8] = {62,64,69,71,78,80,85,87};

/* MPR and Blinking Audio
 * init
 * 
 */


Adafruit_MPR121 cap = Adafruit_MPR121();
uint16_t lasttouched = 0;
uint16_t currtouched = 0;

uint64_t TimePastFade;
uint64_t TimePastControl;
uint64_t TimePastMois;
uint64_t TimePastStepping;
uint64_t TimePastSleep;
int ms = 1000;

uint8_t control_update_speed = 100;
uint8_t mois_update_speed = 1000;
int stepping_update_speed = 250;

int sleep_update_speed = 5000;

uint8_t stepping = 0;
bool steppingUp = false;
uint8_t steppingChoice = 0;


ReverbTank reverb;

// harmonics
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, AUDIO_RATE> aCos8(COS8192_DATA);

// volume controls
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol5(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol6(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol7(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol8(COS8192_DATA);

Oscil<SIN2048_NUM_CELLS, CONTROL_RATE> LFO1(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, CONTROL_RATE> LFO2(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, CONTROL_RATE> LFO3(SIN2048_DATA);
Oscil<SIN2048_NUM_CELLS, CONTROL_RATE> LFO4(SIN2048_DATA);

// audio volumes updated each control interrupt and reused in audio till next control
int v1,v2,v3,v4,v5,v6,v7,v8,lfo_v1,lfo_v2,lfo_v3,lfo_v4;

int freq_shift = 0;
int freq_shift_2 = 0;
int freq_shift_dst = 0;
int freq_shift_amount = 0;
uint8_t freq_shift_speed = 1; //per sec

int controls[4] = {0, 0, 0, 0};
//0,1,2,3
//4,5,6,7
//8,9,10,11

//3,7,11: blink effect
//0,4,8 -> control[0]

bool blinking = false;
int blink_count = 3;
int blink_duration = 200;
uint64_t blink_duration_count;
uint8_t blink_frame = 0;

int lastValues [12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int thisValues [12] = {0,0,0,0,0,0,0,0,0,0,0,0};
int selected_note1 = 0;
int selected_note2 = 0;

float lfo_control_aa1 = 0.0;
float lfo_control_aa2 = 0.0;
float lfo_control_aa3 = 0.0;
float lfo_control_aa4 = 0.0;

int master_level = 0;
int LDR_MIN = 520;
int LDR_MAX = 750;

uint8_t num_led = 1;

int index_led = 1;

void setup(){

  /* Jar LED and LDR
   * Setup
   * 
   */
   
  for (int i = 0; i<7;i++){
    pinMode(led_pins[i], OUTPUT);
    ledcSetup(i, freq, resolution);
    ledcAttachPin(led_pins[i], i);
  }

  


  /* Mozzi Audio
   * setup
   * 
   */

   
  // set harmonic frequencies
  aCos1.setFreq(mtof(62));
  aCos2.setFreq(mtof(64));
  aCos3.setFreq(mtof(70));
  aCos4.setFreq(mtof(72));
  aCos5.setFreq(mtof(79));
  aCos6.setFreq(mtof(81));
  aCos7.setFreq(mtof(86));
//  aCos8.setFreq(mtof(107));

// set volume change frequencies
  kVol1.setFreq(4.43f); // more of a pulse
  kVol2.setFreq(0.0245f);
  kVol3.setFreq(0.019f);
  kVol4.setFreq(0.07f);
  kVol5.setFreq(0.047f);
  kVol6.setFreq(0.031f);
  kVol7.setFreq(0.0717f);
  kVol8.setFreq(0.041f);

  LFO1.setFreq(0.5f);
  LFO2.setFreq(0.5f);
  LFO3.setFreq(0.5f);
  LFO4.setFreq(0.5f);
//  kVol1.setFreq(2.0f); // more of a pulse
//  kVol2.setFreq(2.0f);
//  kVol3.setFreq(2.0f);
//  kVol4.setFreq(2.0f);
//  kVol5.setFreq(2.0f);
//  kVol6.setFreq(2.0f);
//  kVol7.setFreq(2.0f);
//  kVol8.setFreq(2.0f);

  v1=v2=v3=v4=v5=v6=v7=v8=lfo_v1=lfo_v2=lfo_v3=lfo_v4=127;
  Serial.begin(9600);

  /* MPR121 Audio
   * setup
   * 
   */

  while (!Serial) { // needed to keep leonardo/micro from starting too fast!
    delay(10);
  }
  
  Serial.println("Adafruit MPR121 Capacitive Touch sensor test"); 
  
  // Default address is 0x5A, if tied to 3.3V its 0x5B
  // If tied to SDA its 0x5C and if SCL then 0x5D
  if (!cap.begin(0x5A)) {
    Serial.println("MPR121 not found, check wiring?");
    while (1);
  }
  Serial.println("MPR121 found!");
  startMozzi(CONTROL_RATE);

  /* Mois
   * Setup
   * 
   */
   for (int i = 0; i<3;i++){
     moisture_base[i] = mozziAnalogRead(mois_pins[i]);
   }

   
  /* Global Time
   * setup
   * 
   */

  TimePastControl = esp_timer_get_time();
  TimePastFade = esp_timer_get_time();
  TimePastSleep = esp_timer_get_time();
}

void ledcAnalogWrite(uint8_t channel, uint32_t value, uint32_t valueMax = 255) {
  // calculate duty, 8191 from 2 ^ 13 - 1
  uint32_t duty = (8191 / valueMax) * min(value, valueMax);

  // write duty to LEDC
  ledcWrite(channel, duty);
}

void loop(){
  audioHook();
}


void updateControl(){

    
  /* Jar LED 
   * loop
   * 
   */


  if(esp_timer_get_time() - TimePastFade >= fade_update_speed*ms){
    int lightVal = mozziAnalogRead(LDR_PIN);
    Serial.print(lightVal);
    Serial.print("\t  hi");
    Serial.println();
    if (lightVal<=LDR_MIN){
      master_level = 0;
    } else if (lightVal<=LDR_MAX){
//      Serial.print("hi");
      master_level = map(lightVal, LDR_MIN, LDR_MAX, 0, 8);
    } else {
      master_level = 8;
    }
//    Serial.print(master_level);
//    Serial.println();
    
    if (lightVal<=700){
//    if (true){

//      uint8_t index = random(7);

      index_led = map(num_led,1,15,0,7);
//      Serial.println(index_led);
      if (index_led>7){
        index_led = 7;
      } else if (index_led <= 0){
        index_led = 0;
      }
      for (int i = 0; i < index_led; i++){
        fadeValues[i] += fadeAmount[i];
        if (fadeValues[i] >= 250) {
          fadeAmount[i] = -10;
          fadeValues[i] = 250;
        } else if (fadeValues[i]<=0){
          fadeAmount[i] = 10;
          fadeValues[i] = 0;

          
        }
        ledcAnalogWrite(i, fadeValues[i]);
      }

      for (int i = index_led; i<7;i++){
        if (fadeValues[i]>=10){
          fadeValues[i] -= 10;
        } else {
          fadeValues[i] = 0;
        }
        ledcAnalogWrite(i, fadeValues[i]);
      }
    } else {

      for (int i = 0; i < 7; i++){
        ledcAnalogWrite(i, 0);

        
      }
      
    }
    TimePastFade = esp_timer_get_time();
  }

     /* MPR121
   * loop
   * 
   */
   
  
  
  



  /* Mozzi Audio
   * loop
   * 
   */

   v1 = kVol1.next(); // going at a higher freq, this creates zipper noise, so reduce the gain
   v2 = kVol2.next();
   v3 = kVol3.next();
//   v4 = kVol4.next();
   v5 = kVol5.next();
   v6 = kVol6.next();
//   v7 = kVol7.next();
   //v8 = kVol8.next();
   v8 = 0;
   lfo_v1 = LFO1.next();
   lfo_v2 = LFO2.next();
//   lfo_v3 = LFO3.next();
//   lfo_v4 = LFO4.next();

   if (blinking && blink_frame<=30){
    v8 = 127 - blink_frame * 4;
    blink_frame+=1;
   }

   if (esp_timer_get_time() - TimePastStepping >= stepping_update_speed*ms){
      if (stepping>0){
//        Serial.println("change");
        aCos5.setFreq(mtof(selected_note1)+freq_shift);
        
        aCos6.setFreq(mtof(selected_note2)+freq_shift);
        selected_note1 = notes_2[random(8)];
        if (steppingUp){
          
          steppingChoice += 1;
          if (steppingChoice >= 7){
            steppingChoice = 7;
            steppingUp = false;
          }
          selected_note2 = notes_2[steppingChoice];
          
        } else {
          steppingChoice -= 1;
          if (steppingChoice <= 0){
            steppingChoice = 0;
            steppingUp = true;
          }
          selected_note2 = notes_2[steppingChoice];
        }
        stepping -= 1;
      }
      TimePastStepping = esp_timer_get_time();
   }

    if (abs(lfo_v1)<10){
      LFO1.setFreq(lfo_control_aa1);
      aCos1.setFreq(mtof(used_notes[0])+freq_shift);
//      Serial.print(lfo_control_aa1);
//      Serial.println("\t");
    }
    if (abs(lfo_v2)<10){
      LFO2.setFreq(lfo_control_aa2);
      aCos2.setFreq(mtof(used_notes[1])+freq_shift);
//      Serial.print(lfo_control_aa2);
//      Serial.println("\t");
    }
    
//    if (abs(lfo_v3)<10){
//      LFO3.setFreq(lfo_control_aa3);
//      aCos3.setFreq(mtof(used_notes[2])+freq_shift);
////      Serial.println(lfo_control_aa3);
//    }
//    if (abs(lfo_v4)<10){
//      LFO4.setFreq(lfo_control_aa4);
//      aCos4.setFreq(mtof(used_notes[3])+freq_shift);
////      Serial.println(lfo_control_aa4);
//    }
//    
    
    
    
    
   

   /* Control Loop:
     *    Moisture read
     * 
     */

   if((esp_timer_get_time() - TimePastMois)>>6 >= mois_update_speed*ms){
     //freq_shift = 0;
//     freq_shift_amount = 0;
     freq_shift_dst = 0;
     for (uint8_t j=0; j<3; j++){
        moisture[j] = mozziAnalogRead(mois_pins[j]) - moisture_base[j];
        freq_shift_dst += moisture[j];
//        Serial.print(moisture[j]);
//        Serial.print("\t");
     }
//     Serial.print(freq_shift_dst);
//     Serial.print("  ");
     if (freq_shift_dst - freq_shift > 30){
       freq_shift_dst = freq_shift + 30;
     } else if (freq_shift_dst - freq_shift < -30) {
       freq_shift_dst = freq_shift - 30;
     }
//     Serial.println(freq_shift_dst);
     
   }

  /* MPR121 and MozziControl
   * loop
   * 
   */

  if(esp_timer_get_time() - TimePastSleep >= sleep_update_speed*ms){
     num_led -= 3;
     if (num_led<=1 || num_led>=250){
      num_led = 1;
     }
     uint8_t seq = random(3);
     if (seq == 2){
      num_led += 2;
     }
     Serial.println(num_led);
     
     TimePastSleep = esp_timer_get_time();
  }
  
  if(esp_timer_get_time() - TimePastControl >= control_update_speed*ms){

    /* Control Loop:
     *    MPR121 read
     * 
     */
    currtouched = cap.touched();
//    for (uint8_t i=0; i<12; i++) {
//      // it if *is* touched and *wasnt* touched before, alert!
//      if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
//    //      Serial.print(i); Serial.println(" touched");
//        
//        controls[int(floor(i/3))] += 1;
//        
//        if (i==0){
//          blinking = true;
//          blink_duration_count = esp_timer_get_time() + blink_duration;
//        } 
//      }
//      
//      // if it *was* touched and now *isnt*, alert!
//      if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
//    //      Serial.print(i); Serial.println(" released");
//      }
//    }
    


//     Serial.print("Filt: ");
     for (uint8_t i=0; i<12; i++) {
        if ((currtouched & _BV(i)) && !(lasttouched & _BV(i)) ) {
      //      Serial.print(i); Serial.println(" touched");
          
          controls[int(floor(i/3))] += 1;
          num_led += 1;
          
          if (i==0 || i==11 || i==5){
            blinking = true;
            blink_duration_count = esp_timer_get_time() + blink_duration;
          } 
        }
        
        // if it *was* touched and now *isnt*, alert!
        if (!(currtouched & _BV(i)) && (lasttouched & _BV(i)) ) {
      //      Serial.print(i); Serial.println(" released");
        }
//        Serial.print(cap.filteredData(i)); 
//        Serial.print("\t");
//        freq_shift += cap.baselineData(i) - cap.filteredData(i);

        if (i==1||i==6||i==10){
          
          if(stepping==0){
            thisValues[i] = cap.filteredData(i);
            if (lastValues[i] - thisValues[i] > 20){
              blinking = true;
              blink_duration_count = esp_timer_get_time() + blink_duration;
              num_led += 1;
//              Serial.print("stepping");
              stepping = random(4,7);
              selected_note1 = notes_2[random(8)];
              steppingChoice = random(8);
              if (steppingChoice>4){
                steppingUp = false;
              } else {
                steppingUp = true;
              }
              selected_note2 = notes_2[steppingChoice];
            }
            lastValues[i] = thisValues[i];
          }
        } else if (i==2||i==9){
          freq_shift_2 += cap.filteredData(i);
        } else {
          thisValues[i] = cap.filteredData(i);
          if (lastValues[i] - thisValues[i] > 20){
            blinking = true;
            blink_duration_count = esp_timer_get_time() + blink_duration;
//            num_led += 1;
            uint8_t choice = random(4);
            Serial.print("choice:");
            Serial.print(choice);
            switch(choice){
              case 0:
                for(uint8_t i = 0; i<4; i++){
                  used_notes[i] = notes_1[i];
                }
                Serial.print("Switch to 1");
                break;
              case 1:
                for(uint8_t i = 0; i<4; i++){
                    used_notes[i] = notes_3[i];
                  }
                Serial.print("Switch to 2");
                break;
              case 2:
                for(uint8_t i = 0; i<4; i++){
                  used_notes[i] = notes_4[i];
                }
                Serial.print("Switch to 3");
                break;
              case 3:
                for(uint8_t i = 0; i<4; i++){
                  used_notes[i] = notes_5[i];
                }
                Serial.print("Switch to 4");
                break;
            }
            Serial.println();
          }
          lastValues[i] = thisValues[i];
        }


     }
//     Serial.println(freq_shift_2);
     freq_shift_2 = 0;
     lasttouched = currtouched;
//     Serial.println(freq_shift);
//     Serial.print("Base: ");
//     for (uint8_t i=0; i<12; i++) {
//       Serial.print(cap.baselineData(i)); Serial.print("\t");
//     }
//      Serial.println();


//    for (uint8_t n=0; n<4; n++){
//      Serial.print(controls[n]);
//      Serial.print("\t ");
//    }
//    Serial.println(" ");

    

    /* Control Loop:
     *    Mozzi Write
     * 
     */

    if (freq_shift_dst>freq_shift){
      freq_shift += 1; 
    } else if (freq_shift_dst<freq_shift){
      freq_shift -= 1;
    } else {
      
    }

    
    lfo_control_aa1 = (200 - (float)min(min(min(cap.filteredData(9),cap.filteredData(11)),min(cap.filteredData(5),cap.filteredData(7))),min(min(cap.filteredData(6),cap.filteredData(10)),min(cap.filteredData(4),cap.filteredData(8)))) )*0.005;
    lfo_control_aa2 = (200 - (float)min(min(min(cap.filteredData(4),cap.filteredData(9)),min(cap.filteredData(8),cap.filteredData(6))),min(min(cap.filteredData(5),cap.filteredData(7)),min(cap.filteredData(1),cap.filteredData(11)))) )*0.005;
//    lfo_control_aa3 = (200 - (float)cap.filteredData(11) )*0.005;
//    lfo_control_aa4 = (200 - (float)cap.filteredData(8) )*0.005;
    

    
//    aCos5.setFreq(mtof(notes_1[4])+freq_shift);
//    aCos6.setFreq(mtof(notes_1[5])+freq_shift);
//    aCos7.setFreq(mtof(notes_1[6])+freq_shift);
    //aCos8.setFreq(mtof(84+note_shift));
    

    /* Control Loop:
     *    LED Fade Write
     * 
     */
    if (blinking){
      if (esp_timer_get_time() - blink_duration_count >= blink_duration*ms){
        if (blink_count>=0){
          v8 = 127;
          blink_frame = 0;
//          Serial.println("blinnnl");
          aCos8.setFreq(mtof(69+4*int(random(12))));
          blink_count -= 1;
        } else {
          blinking = false;
          blink_count = int(random(1,4));
        }
        blink_duration = random(30,700);
        blink_duration_count = esp_timer_get_time();
      }
    }
    
    // reset our state
    TimePastControl = esp_timer_get_time();
  }


  // comment out this line for detailed data from the sensor!
  return;
}

AudioOutput_t updateAudio(){
  long cho1 = (long)aCos1.next()*v1* lfo_v1/127;
  long cho2 = (long)aCos2.next()*v2* lfo_v2/127;
  long cho3 = (long)aCos3.next()*v3* lfo_v1/127;
  long cho4 = (long)aCos4.next()*v4* lfo_v2/127;

//  long mod = (long) aCos4.next() * v4 * 2 * aCos5.next();
//  mod >>= 15;
//  cho = 1.0 * lfo_v1/127 *cho;
  long st = (long)
    aCos5.next()*v5 + aCos6.next()*v6;
  st = v1*st;
  int asig = (int) aCos8.next()*v8;
//  cho = lfo_v * cho;
  asig >>= 11;
  cho1 >>= 11;
  cho2 >>= 11;
  cho3 >>= 11;
  cho4 >>= 11;
  st >>= 18;
//  int arev = reverb.next(asig);
//  return (int)cho;
  long master = (cho1 + cho2 + cho3 + cho4 + st*0.8 + asig);
  master >>= master_level;
  master <<= 1;
//  return (int)cho1 + (int)cho2 + (int)cho3 + (int)cho4 +(int)st + asig;
  return (int) master;
}

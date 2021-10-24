// Sentient: A simulation of the evolving personality inside an AI.
// https://github.com/msc-creative-computing/p-comp-week-1-labs-jasper-zheng 

const int MIN_SPEED = 10;
const int MAX_SPEED = 600;

unsigned long time;
int display_state[8] = {0,0,0,0,0,0,0,0};
int switch_state[2] = {0,0}; // -1; 1;
int pointer = -1;

float upset_count = 2.0;
//float peace_count = 10;
float confident_count = 2.0;

float averange = 0.0;
float personality = 100.0;
int direction = 0;

void setup()
{
  //Serial.begin(9600);
  
  for (int i = 2; i<10; i++){
    pinMode(i,OUTPUT);
    digitalWrite(i, LOW); 
  }
  
  pinMode(11,INPUT);
  pinMode(12,INPUT);
  pinMode(13,INPUT);
  
  time = millis();
}

void loop()
{
  if(millis() - time >= personality){
    update_pointers();
    update_status();
    update_speed();
    time = millis();
    /*Serial.print("Confident: ");
    Serial.print(confident_count);
    Serial.print(" Peace: ");
    Serial.print(peace_count);
    Serial.print(" Upset: ");
    Serial.print(upset_count);
    Serial.print(" Personality : ");
    Serial.println(averange);*/
  }
  
  for (int i = 0; i<2; i++){
    switch_state[i] = digitalRead(11+i);
  }
  //Serial.print(digitalRead(12));
  
  if (switch_state[0] == HIGH) {
    upset_count -= 1;
  } else if (switch_state[1] == HIGH) {
    confident_count += 1;
  }
  
  for (int i = 0; i<8; i++){
    if (display_state[i] == 1){
      digitalWrite(i+2, HIGH); 
    } else {
      digitalWrite(i+2, LOW); 
    }
  }
}

void update_pointers(){
  if (direction == 0){
    pointer += 1;
  } else {
    pointer -= 1;
  }
  if (pointer == -1) {
    direction = 0;
  } else if (pointer == 8) {
    direction = 1;
  }
}

void update_status(){
  for (int i = 0; i<8; i++){
    if (pointer <= i + 1 && pointer >= i - 1) {
      display_state[i] = 1;
    } else {
      display_state[i] = 0;
    }
  }
}

void update_speed() {
  if (confident_count - upset_count == 0) {
    averange = 0.0;
  } else {
    averange = (confident_count + upset_count)/(confident_count - upset_count);
    personality = (averange + 1) / 2 * (MAX_SPEED - MIN_SPEED) + MIN_SPEED; // 50 - 1000

  }
  
  
}

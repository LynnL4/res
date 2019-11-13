#include <Wire.h>
#include <ADXL345.h>
ADXL345 adxl; //variable adxl is an instance of the ADXL345 library


#define CLOCK_START  1
#define CLOCK_STOP   0

#define FACE_MAIN 5
#define FACE_BACK 6
#define FACE_THREE 1
#define FACE_FIVE 3
#define FACE_TEN 2
#define FACE_TWENTY 4

#define TIMEBASE 1000
#define BUTTON A1
#define LIGHT A0
#define BEE A2

#define LOG  Serial.print("status: ");\
        Serial.println(status);\
        Serial.print("face: ");\
        Serial.println(face);\
        Serial.print("change: ");\
        Serial.println(change);\
        Serial.print("isCount: ");\
        Serial.println(isCount);


int32_t countDown = 0;
int32_t countMillis = 0; 
uint8_t status = CLOCK_STOP;
uint8_t face = 5;
uint8_t change = 0;
uint8_t isCount = 0;
int tempo = 200;

/* play tone */
void playTone(int tone, int duration) {
    for (long i = 0; i < duration * 1000L; i += tone * 2) {
        digitalWrite(BEE, HIGH);
        delayMicroseconds(tone);
        digitalWrite(BEE, LOW);
        delayMicroseconds(tone);
    }
}

void playNote(char note, int duration) {
    char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
    int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };

    // play the tone corresponding to the note name
    for (int i = 0; i < 8; i++) {
        if (names[i] == note) {
            playTone(tones[i], duration);
        }
    }
}



void beeInit()
{
  pinMode(BEE, OUTPUT);
}

void beeOn()
{
   digitalWrite(BEE, HIGH);
}

void beeOff()
{
   digitalWrite(BEE, LOW);
}
void wait(uint32_t timeout)
{
  uint8_t timer = millis();
  while(millis() - timer >= timeout){
    shake();
    if(CLOCK_STOP == status){
      return;
    }
  }
  
}
void clockTimeOut()
{
  lightOn();
  while(1){
  Serial.println(__FUNCTION__);
  LOG;
  shake();
  if(CLOCK_STOP == status){
    beeOff();
    lightOff();
    return;
  }
  playNote('c', 1 * tempo);
  delay(50);
 }
}

void lightInit()
{
  pinMode(LIGHT, OUTPUT);
}
void lightOn()
{
  digitalWrite(LIGHT, HIGH);
}
void lightOff()
{
  digitalWrite(LIGHT, LOW);
}

void clockBegin()
{
   lightOn();
   playNote('f', 1 * tempo);
   lightOff();
}

void clockEnd()
{
   lightOn();
   beeOff();
   delay(300);
   playNote('f', 1 * tempo);
   delay(100);
   playNote('f', 1 * tempo);
   beeOff();
   lightOff();
}

void adxl345Init()
{
   adxl.powerOn();

  //set activity/ inactivity thresholds (0-255)
  adxl.setActivityThreshold(75); //62.5mg per increment
  adxl.setInactivityThreshold(75); //62.5mg per increment
  adxl.setTimeInactivity(10); // how many seconds of no activity is inactive?
 
  //look of activity movement on this axes - 1 == on; 0 == off 
  adxl.setActivityX(1);
  adxl.setActivityY(1);
  adxl.setActivityZ(1);
 
  //look of inactivity movement on this axes - 1 == on; 0 == off
  adxl.setInactivityX(1);
  adxl.setInactivityY(1);
  adxl.setInactivityZ(1);
 
  //look of tap movement on this axes - 1 == on; 0 == off
  adxl.setTapDetectionOnX(0);
  adxl.setTapDetectionOnY(0);
  adxl.setTapDetectionOnZ(1);
 
  //set values for what is a tap, and what is a double tap (0-255)
  adxl.setTapThreshold(50); //62.5mg per increment
  adxl.setTapDuration(15); //625us per increment
  adxl.setDoubleTapLatency(80); //1.25ms per increment
  adxl.setDoubleTapWindow(200); //1.25ms per increment
 
  //set values for what is considered freefall (0-255)
  adxl.setFreeFallThreshold(7); //(5 - 9) recommended - 62.5mg per increment
  adxl.setFreeFallDuration(45); //(20 - 70) recommended - 5ms per increment
 
  //setting all interrupts to take place on int pin 1
  //I had issues with int pin 2, was unable to reset it
  adxl.setInterruptMapping( ADXL345_INT_SINGLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_DOUBLE_TAP_BIT,   ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_FREE_FALL_BIT,    ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_ACTIVITY_BIT,     ADXL345_INT1_PIN );
  adxl.setInterruptMapping( ADXL345_INT_INACTIVITY_BIT,   ADXL345_INT1_PIN );
 
  //register interrupt actions - 1 == on; 0 == off  
  adxl.setInterrupt( ADXL345_INT_SINGLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_DOUBLE_TAP_BIT, 1);
  adxl.setInterrupt( ADXL345_INT_FREE_FALL_BIT,  1);
  adxl.setInterrupt( ADXL345_INT_ACTIVITY_BIT,   1);
  adxl.setInterrupt( ADXL345_INT_INACTIVITY_BIT, 1);
}

int getDir()
{
  int x,y,z;  
  adxl.readXYZ(&x, &y, &z);
  
  if(abs(x) > abs(y) && abs(x) > abs(z)){
    if(x >= 0)
      return 1;
    else
      return 2;
   }
  else if(abs(y) > abs(x) && abs(y) > abs(z)){
       if(y >= 0)
      return 3;
    else
      return 4;
  }
  else if(abs(z) > abs(x) && abs(z) > abs(y)){
     if(z >= 0)
      return 5;
    else
      return 6;
  }
  return 0;
}
void op(uint8_t status)
{
  switch(status)
  {
    case FACE_MAIN:
      countDown = 0;
      break;
    case FACE_BACK:
      countDown = 0;
       break;
    case FACE_THREE:
      countDown = (TIMEBASE) * 1;
       break;
    case FACE_FIVE:
      countDown = (TIMEBASE) * 2;
       break;
    case FACE_TEN:
      countDown = (TIMEBASE) * 3;
       break;
    case FACE_TWENTY:
     countDown = (TIMEBASE) * 4;
      break;
    default:
      break;
  }
}

void reactor()
{
  if(FACE_MAIN != face && FACE_BACK != face && isCount == 1)
  {
    if(0 >= countDown ){
      isCount = 0;
      clockTimeOut();
    }
    
    int32_t temp = millis();
    countDown -=  (temp - countMillis);
    countMillis = temp;
  }
}

void keyInit()
{
  pinMode(BUTTON, INPUT);
}
void getKey()
{
    delay(50);
    if(digitalRead(BUTTON) == LOW){
      Serial.println(__FUNCTION__);
      LOG;
      if(CLOCK_START == status)
      {
        status = CLOCK_STOP;
        isCount = 0;
        clockEnd();
      }else if(CLOCK_STOP == status && change == 1){
        status = CLOCK_START;
        change = 0;
      }
    }
  
}

void shake()
{
  double xyz[3];
  for(int i = 0; i < 80; i++)
  {
    adxl.getAcceleration(xyz);
    if(abs(xyz[0]) > 1.1 || abs(xyz[1]) > 1.1 || abs(xyz[2]) > 1.1 )
      continue;
    else
      return;
  }
  Serial.println(__FUNCTION__);
      LOG;
  if(CLOCK_START == status)
  {
    status = CLOCK_STOP;
    isCount = 0;
    clockEnd();
   }else if(CLOCK_STOP == status && change == 1){
    status = CLOCK_START;
    change = 0;
  }
  delay(700);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  adxl345Init();
  lightInit();
  beeInit();
  //keyInit();
  //attachInterrupt(digitalPinToInterrupt(BUTTON), getKey, FALLING);
}

void loop() {
  countMillis = millis();
  int temp = getDir();
  if(temp != face){
     face = temp;
     change = 1;
  }
  shake();
  if(CLOCK_START == status && isCount == 0 && (FACE_MAIN != face && FACE_BACK != face))
  { 
    clockBegin();
    op(face);
    isCount = 1;
  }
  reactor();
 
}

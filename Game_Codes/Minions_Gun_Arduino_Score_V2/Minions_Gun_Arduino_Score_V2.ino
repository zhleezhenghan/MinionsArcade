#include <IRremote.h> //include the library
int receiver = 11;  //initialise pin 11 as receiver pin
IRrecv irrecv(receiver);  //create a new instance of receiver
decode_results results;
#include <Target2.h>
#include "TM1637.h"
#define CLK 2//pins definitions for TM1637 and can be changed to other ports       
#define DIO 3
TM1637 tm1637(CLK, DIO);

#define VIBRATION_DELAY 100

int ones;
int tens;
int hundreds;
int thousands;
int score = 0;
int MoPin = 9;

void setup()
{
  Serial.begin(9600);
  // In case the interrupt driver crashes on setup, give a clue
  // to the user what's going on.
  irrecv.enableIRIn(); // Start the receiver

  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  pinMode(MoPin, OUTPUT);
}

void loop()
{
  unsigned long startTime = millis();
  static unsigned long startCounting = 0;
  if (irrecv.decode(&results)) {
    Serial.println(results.value);
    if (results.value == IRSEND_SCORE_PLAYER1) {
      score++;
      vibrateScore();
    }
    else if(results.value == IRSEND_PENALTY_PLAYER1){
      score /= 2;
      vibratePenalty();
    }
    irrecv.resume(); // Receive the next value
  }
  displayNum(score);
  delay(100); // delete this line if it doesn't reduce the noise.
}


void displayNum(int inputNum) {
  if (inputNum <= 9999) {
    if (inputNum >= 1000) {
      thousands = inputNum / 1000;
      inputNum -= thousands * 1000;
      hundreds = inputNum / 100;
      inputNum -= hundreds * 100;
      tens = inputNum / 10;
      inputNum -= tens * 10;
      ones = inputNum;
      tm1637.display(3, ones);
      tm1637.display(2, tens);
      tm1637.display(1, hundreds);
      tm1637.display(0, thousands);
    }
    else if (inputNum >= 100) {
      hundreds = inputNum / 100;
      inputNum -= hundreds * 100;
      tens = inputNum / 10;
      inputNum -= tens * 10;
      ones = inputNum;
      tm1637.display(3, ones);
      tm1637.display(2, tens);
      tm1637.display(1, hundreds);
      tm1637.display(0, 0);
    }
    else if (inputNum >= 10) {
      tens = inputNum / 10;
      inputNum -= tens * 10;
      ones = inputNum;
      tm1637.display(3, ones);
      tm1637.display(2, tens);
      tm1637.display(1, 0);
      tm1637.display(0, 0);
    }
    else if (inputNum >= 0) {
      ones = inputNum;
      tm1637.display(3, ones);
      tm1637.display(2, 0);
      tm1637.display(1, 0);
      tm1637.display(0, 0);
    }
  }
  else {
    tm1637.display(3, 13);
    tm1637.display(2, 12);
    tm1637.display(1, 11);
    tm1637.display(0, 10);
  }
}

void vibrateScore(){
  digitalWrite(MoPin, HIGH);
  delay(VIBRATION_DELAY);
  digitalWrite(MoPin, LOW);
  delay(VIBRATION_DELAY);
}

void vibratePenalty(){
  vibrateScore();
  vibrateScore();
}




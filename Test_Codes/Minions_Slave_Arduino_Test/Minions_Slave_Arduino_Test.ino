/*
   Code for slave arduino in Minions game
   Transmits '1' for player1 hit, '2' for player2 hit,
   ' ' for nothing happening, and 'D' for getting the target to die after 2 seconds

   Receives data from master arduino: 'd' to kill target, 'r' to revive target

   Receives IR signal from gun laser: 1 for player1 and 2 for player2
*/
#include <Wire.h>

//  initialise IR receiver
#include <IRremote.h> //include the library
int receiver = 11;  //initialise pin 11 as receiver pin
IRrecv irrecv(receiver);  //create a new instance of receiver
decode_results results;

//  initialise targets and servos
#include <Servo.h>
#include <Target2.h>

Servo targetServo;
Target2 target;
int LEDpin = 12;
int servoPin = 10;

//  initialise receive data from master arduino
char recvSignal = ' ';
int irRecvSignal;
bool startCounting = false;

void setup() {
  // put your setup code here, to run once:
  Wire.begin(6);  // setting address of slave ( change this code from 0 to 9 for each target arduino nano
  Wire.onReceive(receiveEvent);
  Wire.onRequest(requestEvent);
  Serial.begin(9600);

  targetServo.attach(servoPin);
  target.attachLED(LEDpin);
  target.attachServo(targetServo);
  //  setting type of target: Minion ('M') or Girl ('G')
  target.setType('M');
  target.revive();

  //  initialising starting position of servos
  targetServo.write(0);

  irrecv.enableIRIn();  //start the receiver
}

void loop() {
  unsigned long startTime = millis();
  static unsigned long counter = 0;
  if (irrecv.decode(&results)) { //if we have received an IR signal
    irRecvSignal = results.value;
    Serial.print("Received signal from player Gun laser: ");
    Serial.println(irRecvSignal);
    if (irRecvSignal == PLAYER1_SIGNAL || irRecvSignal == PLAYER2_SIGNAL) {
      target.die();
    }
    irrecv.resume();  //next value
  }


  //  Set target to die after 2 seconds of being alive and not getting shot
  /*
    if(target.getState() == ALIVE){
    startCounting = true;
    counter = startTime;
    }
    if((startTime - counter >= 2000) && startCounting = true){
    startCounting = false;
    irRecvSignal = DIE_AFTER_DELAY_SIGNAL;
    target.revive();
    }
  */
}

//  receiving signals from master arduino
void receiveEvent(int howMany) {
  while (Wire.available()) {
    recvSignal = Wire.read();
    Serial.print("Received data from master arduino, data = ");
    Serial.println(recvSignal);

    if (recvSignal == DIE_SIGNAL) {
      target.die();
    }
    else if (recvSignal == REVIVE_SIGNAL) {
      target.revive();
    }
  }
}

void requestEvent() {
  if (irRecvSignal == PLAYER1_SIGNAL) {
    Wire.write(HIT_BY_PLAYER_1);
    irRecvSignal = NULL_IR_SIGNAL;
  }
  else if (irRecvSignal == PLAYER2_SIGNAL) {
    Wire.write(HIT_BY_PLAYER_2);
    irRecvSignal = NULL_IR_SIGNAL;
  }
  else if (irRecvSignal == DIE_AFTER_DELAY_SIGNAL) {
    Wire.write(DIE_AFTER_DELAY);
    irRecvSignal = NULL_IR_SIGNAL;
  }
  else {
    Wire.write(' ');
  }
}

bool actAfterDelay(unsigned long startTime, int timeInterval) {
  static unsigned long prevTime = startTime;
  unsigned long currentTime = millis();
  if ((currentTime - prevTime) >= timeInterval) {
    prevTime = currentTime;
    return true;
  }
  else {
    return false;
  }
}

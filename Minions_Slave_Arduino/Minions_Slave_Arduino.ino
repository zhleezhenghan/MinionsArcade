/*
 * Code for slave arduino in Minions game
 * Transmits '1' for player1 hit, '2' for player2 hit,
 * ' ' for nothing happening, and 'D' for getting the target to die after 2 seconds
 * 
 * Receives data from master arduino: 'd' to kill target, 'r' to revive target
 * 
 * Receives IR signal from gun laser: 1 for player1 and 2 for player2
 */
#include <Wire.h>

//  Define alive and dead states
#define ALIVE true
#define DEAD false

//  Defining transmit signals to master arduino
#define SEND_HIT_BY_PLAYER_1 '1'
#define SEND_HIT_BY_PLAYER_2 '2'
#define SEND_DIE_AFTER_DELAY 'D'

//  Defining receive signals to slave arduinos
#define RECEIVE_DIE 'd'
#define RECEIVE_REVIVE 'r'



//  Define received IR signal from gun laser
#define RECEIVE_PLAYER1 1
#define RECEIVE_PLAYER2 2
#define NULL_IR_SIGNAL 50
#define DIE_AFTER_DELAY_SIGNAL 60

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
int LEDpin = 2;
int servoPin = 3;

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

  //  initialising starting position of servos
  targetServo.write(0);

  irrecv.enableIRIn();  //start the receiver
}

void loop() {
  unsigned long startTime = millis();
  static unsigned long counter = 0;
  if (irrecv.decode(&results)){ //if we have received an IR signal
    irRecvSignal = results.value;
    Serial.print("Received signal from player Gun laser: ");
    Serial.println(irRecvSignal);
    if(irRecvSignal == RECEIVE_PLAYER1 || irRecvSignal == RECEIVE_PLAYER2){
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
void receiveEvent(int howMany){
  while(Wire.available()){
    recvSignal = Wire.read();
    Serial.print("Received data from master arduino, data = ");
    Serial.println(recvSignal);

    if (recvSignal == RECEIVE_DIE){
      target.die();
    }
    else if(recvSignal == RECEIVE_REVIVE){
      target.revive();
    }
  }
}

void requestEvent(){
  if(irRecvSignal == RECEIVE_PLAYER1){
    Wire.write(SEND_HIT_BY_PLAYER_1);
    irRecvSignal = NULL_IR_SIGNAL;
  }
  else if(irRecvSignal == RECEIVE_PLAYER2){
    Wire.write(SEND_HIT_BY_PLAYER_2);
    irRecvSignal = NULL_IR_SIGNAL;
  }
  else if(irRecvSignal == DIE_AFTER_DELAY_SIGNAL){
    Wire.write(SEND_DIE_AFTER_DELAY);
    irRecvSignal = NULL_IR_SIGNAL;
  }
  else{
    Wire.write(' ');
  }
}

bool actAfterDelay(unsigned long startTime, int timeInterval){
  static unsigned long prevTime = startTime;
  unsigned long currentTime = millis();
  if((currentTime - prevTime) >= timeInterval){
    prevTime = currentTime;
    return true;
  }
  else{
    return false;
  }
}

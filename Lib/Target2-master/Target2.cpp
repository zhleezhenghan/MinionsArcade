#include "Arduino.h"
#include "Target2.h"
#include "Servo.h"

Target2::Target2(){
state = DEAD;
down = 0;
up = 75;
}

void Target2::attachLED(int LEDPin){
    this->LEDPin = LEDPin;
    pinMode(this->LEDPin,OUTPUT);
}

void Target2::attachServo(Servo servo){
    this->servo = servo;
}

void Target2::setType(char type){
    this->type = type;
}

char Target2::getType(){
    return type;
}

void Target2::die(){
    if (state == ALIVE){
    digitalWrite(LEDPin,LOW);
    servo.write(down);
    state = DEAD;
    timeOfDeath = millis();
    }
}

int Target2::getState(){
    return state;
}

void Target2::setState(bool state){
    this->state = state;
}

void Target2::revive(){
    if (state == 0&&(millis()-timeOfDeath>1000)){
    digitalWrite(LEDPin,HIGH);
    servo.write(up);
    state = 1;
    }
}

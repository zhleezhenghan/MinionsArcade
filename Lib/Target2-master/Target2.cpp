#include "Arduino.h"
#include "Target2.h"
#include "Servo.h"

Target2::Target2(){
state = DEAD;
down = 125;
up = 70;
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
    }
}

int Target2::getState(){
    return state;
}

void Target2::setState(bool state){
    this->state = state;
}

void Target2::revive(){
    if (state == DEAD){
    digitalWrite(LEDPin,HIGH);
    servo.write(up);
    state = ALIVE;
    }
}

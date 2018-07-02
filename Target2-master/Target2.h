#ifndef Target2_h
#define Target2_h

#include "Arduino.h"
#include "Servo.h"

//  Defining Alive and Dead states
#define ALIVE true
#define DEAD false

//  Defining IR sending signals to ping gun arduino
#define IRSEND_SCORE_PLAYER1 69
#define IRSEND_PENALTY_PLAYER1 96
#define IRSEND_SCORE_PLAYER2 66
#define IRSEND_PENALTY_PLAYER2 99

//  Defining transmit signals to slave arduinos
#define DIE_SIGNAL 'd'
#define REVIVE_SIGNAL 'r'

//  Defining receive signals from slave arduinos
#define HIT_BY_PLAYER_1 '1'
#define HIT_BY_PLAYER_2 '2'
#define DIE_AFTER_DELAY 'D'

//  Define received IR signal from gun laser
#define PLAYER1_SIGNAL 1
#define PLAYER2_SIGNAL 2
#define NULL_IR_SIGNAL 50
#define DIE_AFTER_DELAY_SIGNAL 60

class Target2
{
    public:
    Target2();
    void attachLED(int LEDPin);
    void attachServo(Servo servo);
    void setType(char type);
    char getType();
    void die();
    int getState();
    void setState(bool state);
    void revive();

    private:
    char type;
    int LEDPin;
    int ReceiverPin;
    Servo servo;
    bool state;
    int down;
    int up;
    unsigned long timeOfDeath;
    int average;
};

#endif

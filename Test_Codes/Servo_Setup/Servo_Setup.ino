
//  initialise targets and servos
#include <Servo.h>
#include <Target2.h>

Servo targetServo;
Target2 target;
int LEDpin = 12;
int servoPin = 10;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

  targetServo.attach(servoPin);
  target.attachLED(LEDpin);
  target.attachServo(targetServo);
  //  setting type of target: Minion ('M') or Girl ('G')
  target.setType('M');
}

void loop() {
  target.die();
  delay(1000);
  target.revive();
  delay(1000);
}


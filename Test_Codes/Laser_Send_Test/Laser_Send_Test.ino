/*
   IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
   An IR LED must be connected to Arduino PWM pin 3.
   Version 0.1 July, 2009
   Copyright 2009 Ken Shirriff
   http://arcfn.com
*/


#include <IRremote.h>

IRsend irsend;

void setup()
{
  pinMode(3, OUTPUT);
}

void loop() {
  irsend.sendSony(69, 12);
  digitalWrite(3, HIGH);
  delay(1000);
  //  delay(500); //5 second delay between each signal burst
}

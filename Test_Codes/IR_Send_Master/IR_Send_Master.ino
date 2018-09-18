/*
 * IRremote: IRsendDemo - demonstrates sending IR codes with IRsend
 * An IR LED must be connected to Arduino PWM pin 3.
 * Version 0.1 July, 2009
 * Copyright 2009 Ken Shirriff
 * http://arcfn.com
 */


#include <IRremote.h>

IRsend irsend;

int player1_score = 0;
int player2_score = 0;

void setup()
{
}

void loop() {
  pingPlayerGun();
  player2_score++;
  player1_score++;
  if(player2_score > 99){
    player2_score = 0;
    player1_score = 0;
  }
  delay(500);
}

void pingPlayerGun() {
  //  the 2 player scores are parsed into a 4 digit number. (e.g. player1_score = 2, player2_score = 4, score = 0204)
  int score_send = player1_score*100 + player2_score;
  irsend.sendSony(score_send, 13);
  delay(25);
}


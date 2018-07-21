#include <IRremote.h>
#include <Target2.h>

IRsend irsend1;
int inPin=2;
int val = 0;

//  neopixels for energy bar
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define PIN 6
#define NUMPIXELS 14
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

int minMana = 0;
int maxMana = 100;
int currentMana = maxMana;
char incomingChar;
bool startRecovering = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(3, OUTPUT);
  pinMode(inPin, INPUT_PULLUP);
  strip.begin();
  for(int i=0; i<NUMPIXELS; i++){
    strip.setPixelColor(i, strip.Color(0, 150, 150));
  }
  strip.show();
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long startTime = millis();
  static unsigned long counter = 0;
  val = digitalRead(inPin);
  currentMana = constrain(currentMana, -1, maxMana);
  digitalWrite(3, HIGH);
  Serial.println(val);
  if(val==LOW){
    counter = startTime;
    startRecovering = false;
    if(currentMana >= 0 ){
      //  change the send signal between SEND_PLAYER1 and SEND_PLAYER2 depending on the gun
      irsend1.sendSony(PLAYER2_SIGNAL, 12);   //ASCII code 69, 12 bits 
      currentMana--;
      delay(25);                  //minimum 25ms delay
    }
  }
  else{
    if(startTime - counter >= 2000){
      startRecovering = true;
    }
  }
  if(startRecovering){
    currentMana ++;
  }
  Serial.println(counter);
  updateManaBar(0, NUMPIXELS, currentMana, minMana, maxMana);
  strip.show();
}

void updateManaBar(int firstManaPixel, int lastManaPixel, int currentMana, int minMana, int maxMana) {
  int numberOfPixels = lastManaPixel - firstManaPixel + 1;
  int numberOfStates = numberOfPixels * 3;
  currentMana = constrain(currentMana, minMana, maxMana);
  int currentState = map(currentMana, minMana, maxMana, 0, numberOfStates);
  int numberOfBlue = map(currentMana, minMana, maxMana, 0, numberOfPixels);
  int colorOfEdge = currentState % 3;

  for (int i = firstManaPixel; i <= lastManaPixel; i++) {
    //Serial.print("Mana pixel number: ");
    //Serial.print(i);
    if (i < firstManaPixel + numberOfBlue) {
      strip.setPixelColor(i, strip.Color(0, 150, 150));
      //Serial.println("light blue");
    }
    else if (i == firstManaPixel + numberOfBlue) {
      switch (colorOfEdge) {
        case 2:
          //Serial.println("green");
          strip.setPixelColor(i, strip.Color(0, 50, 0));
          break;
        case 1:
          //Serial.println("red");
          strip.setPixelColor(i, strip.Color(50, 0, 0));
          break;
        case 0:
          //Serial.println("Black");
          strip.setPixelColor(i, strip.Color(0, 0, 0));
          break;
      }
    } else {
      //Serial.println("Black");
      strip.setPixelColor(i, strip.Color(0, 0, 0));
    }
  }
}

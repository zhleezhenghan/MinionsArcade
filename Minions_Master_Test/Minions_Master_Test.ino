/*
   Code for master arduino in Minions game.
   Receives data from slave arduinos: '1' for player1 hit, '2' for player2 hit,
   ' ' for nothing happening, and 'D' for getting the target to die after 2 seconds

   Transmit data to slave arduinos: 'd' to kill the target, 'r' to revive the target
   Transmit IR signal to player gun's arduino to count score and haptic feedback on scoring/penalty

   Contains code on the gameplay: random targets to come up when all targets are down,
   and counting scores and states of all slave arduinos at a time.
*/
#include <Wire.h>
#include <Target2.h>

#define NUM_BYTES 5
#define MAX_TARGETS 10

//  Defining girls and minions address number
#define GIRL 1

//  initialising each target nano's address
const int target_Address[] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
bool target_State[MAX_TARGETS] = {DEAD};   // setting all target states to false(dead): true = alive, false = dead
char recvSignal[MAX_TARGETS] = {' '};    // initialising all receive signals from slave arduino with placeholder character ' '
char transSignal[MAX_TARGETS] = {' '};   // initialising all transmit singals from master arduino to slave arduinos
int player1_score = 0;
int player2_score = 0;



//  neopixels initialisation and library
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
#define NUMPIXELS 82

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  //  setup colors and pattern
  colorWipeReverse(strip.Color(0, 0, 50), 10);
  colorWipe(strip.Color(50, 50, 0), 10);

  randomSeed(analogRead(A0));
}

void loop() {
  //  code for timer neopixels
  unsigned long timer = 60; 
  unsigned long startTime = millis();
  static unsigned long counter = 0;
  if(timer - counter >= 1000){
    counter = startTime;
    timer--;
    updateTimerBar(0, NUMPIXELS, timer, 0, 60); 
  }
  //  code to move each target
  for (int i = 0; i < MAX_TARGETS; i++) {
    sendData(target_Address[i], REVIVE_SIGNAL);
    transSignal[i] = ' ';
    delay(1000);
    sendData(target_Address[i], DIE_SIGNAL);
    delay(1000);
  }
}


//  counter for neo-pixels function
void updateTimerBar(int firstPixel, int lastPixel, int currentTime, int minTime, int maxTime) {
  int numberOfPixels = lastPixel - firstPixel + 1;
  currentTime = constrain(currentTime, minTime, maxTime);
  int currentPixel = map(currentTime, minTime, maxTime, 0, numberOfPixels);
  for (int i = firstPixel; i <= lastPixel; i++) {
    if (i < currentPixel) {
      strip.setPixelColor(i, strip.Color(50, 50, 0)); //yellow
    }
    else {
      strip.setPixelColor(i, strip.Color(0, 0, 50));  //blue
    }
  }
  strip.show();
}

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, int wait) {
  for (int i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

// Fill the dots one after the other with a color
void colorWipeReverse(uint32_t c, int wait) {
  for (int i = strip.numPixels(); i >= 0; i--) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
}

void flashColor(uint32_t c) {
  for (int i = 0; i < NUMPIXELS; i++) {
    strip.setPixelColor(i, c);
  }
  strip.show();
}

void sendData(int slaveAddress, char c) {
  Wire.beginTransmission(slaveAddress);  //5 is the address of the slave we are writing to
  Wire.write(c);
  Wire.endTransmission();
  Serial.print("Sending data to slave number ");
  Serial.print(slaveAddress);
  Serial.print(", data = ");
  Serial.println(c);
}

/*
   Receive Data from target nano only when target is hit.
*/
void receiveData(int slaveAddress, int dataBytes, int counter) {
  Wire.requestFrom(slaveAddress, dataBytes);  //request from address of the slave arduino, and number of bytes of data

  while (Wire.available()) {
    recvSignal[counter] = Wire.read();
    Serial.print("Received data from ");
    Serial.print(slaveAddress);
    Serial.print(", data = ");
    Serial.println(recvSignal[counter]);
  }
}



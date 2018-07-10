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


// initialising IR transmitter to the gun's receiver
#include <IRremote.h> //include the library
IRsend irsend1;



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

  //  initialise a counter for time keeping
  unsigned long startTime = millis();
  static long timer = 60;
  static unsigned long counter = 0;

  static int i = constrain(i, 0, 3);
  if (isGameOver(timer)) {
    sendAllDead();
    if (startTime - counter >= 500) {
      i++;
      counter = startTime;
      switch (i) {
        case 0:
          flashColor(strip.Color(208, 28, 239));
          break;
        case 1:
          flashColor(strip.Color(66, 244, 86));
          break;
        case 2:
          flashColor(strip.Color(255, 0, 0));
          break;
        default:
          flashColor(strip.Color(0, 0, 0));
      }
    }
  }
  else {
    //  countdown neopixel
    if (startTime - counter >= 1000) {
      counter = startTime;
      updateTimerBar(0, NUMPIXELS, timer, 0, 60);
      timer--;
    }
    //  receive data from all slave arduinos and updating scores
    for (int i = 0; i < MAX_TARGETS; i++) {
      recvSignal[i] = ' ';  // reset the signal back to blank to avoid double counting
      receiveData(target_Address[i], NUM_BYTES, i);  // update the signal if a specific target is hit
      updateScoreAndState(target_Address[i], recvSignal[i]);  //updating the score of each player and states
      pingPlayerGun();
    }

    //  check if all targets are dead, then choose random targets to revive
    if (isAllDead()) {
      randRevive(10);  // set random 10 targets to 'alive' state and change transmit signals
    }

    //  transmit relevant data to slave nanos
    for (int i = 0; i < MAX_TARGETS; i++) {
      sendData(target_Address[i], transSignal[i]);
      transSignal[i] = ' ';
    }
  }
}


bool isGameOver(long timer) {
  return timer < 0;
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

/*  updating the score based on the receiving signal from slave nano.
    differentiate score based on each player hit, as well as if the target is a penalty/score
*/
void updateScoreAndState(int slaveAddress, char recvSignal) {
  if (recvSignal == ' ') {
    return;     // do nothing if no signal is received.
  }
  else if (recvSignal == HIT_BY_PLAYER_1) {
    if (slaveAddress == GIRL) {
      player1_score /= 2;
    }
    else {
      player1_score++;
    }
  }
  else if (recvSignal == HIT_BY_PLAYER_2) {
    if (slaveAddress == GIRL) {
      player2_score /= 2;
    }
    else {
      player2_score++;
    }
  }
  target_State[slaveAddress] = DEAD;  // change state to dead if it's hit
}

/*  sends IR signal to the player's gun to vibrate and update score on player gun's arduino
 *   This signal is continuously sent out by the IR transmitter and will contain information on each
 *   player's current score. The gun receiver will receive the signal and parse out their respective score and 
 *   update the sevseg accordingly.
 */
void pingPlayerGun() {
  //  the 2 player scores are parsed into a 4 digit number. (e.g. player1_score = 2, player2_score = 4, score = 0204)
  int score_send = player1_score*100 + player2_score;
  irsend1.sendSony(score_send, 12);
}

//  check if all targets are dead
bool isAllDead() {
  for (int counter = 0; counter < MAX_TARGETS; counter++) {
    if (target_State[counter] == ALIVE) {
      return false;
    }
  }
  return true;
}

void getRandomNumbers(int* numArray, int noOfRandom)
{
  int i;
  for (i = 0; i < noOfRandom; i++) {
    bool check;
    int n, j;
    do
    {
      n = random(0, MAX_TARGETS);
      //check if the number is already used:
      check = true;
      for (j = 0; j < i; j++)
      {
        if (n == numArray[j]) //if number is already used
        {
          check = false;
          break;
        }
      }
    } while (!check); //loop until new, unique number is found
    numArray[i] = n; //store the generated number in the array
  }
}

//  choose random targets to revive by randomising the slave addresses
void randRevive(int numToRevive) {
  int numArray[numToRevive];
  getRandomNumbers(numArray, numToRevive);
  for (int i = 0; i < numToRevive; i++) {
    target_State[i] = ALIVE;    // change state to being alive
    transSignal[i] = REVIVE_SIGNAL; // change the transmit signal for the target to revive
  }
}

//  function to send a kill signal to all targets
void sendAllDead() {
  for (int i = 0; i < MAX_TARGETS; i++) {
    target_State[i] = DEAD;     // change state to being alive
    transSignal[i] = DIE_SIGNAL;  // change the transmit signal for the target to die
    sendData(target_Address[i], transSignal[i]);
    transSignal[i] = ' ';
  }
}



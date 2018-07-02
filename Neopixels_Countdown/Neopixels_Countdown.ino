#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN 6
#define NUMPIXELS 82

// Parameter 1 = number of pixels in strip
// Parameter 2 = Arduino pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_KHZ800  800 KHz bitstream (most NeoPixel products w/WS2812 LEDs)
//   NEO_KHZ400  400 KHz (classic 'v1' (not v2) FLORA pixels, WS2811 drivers)
//   NEO_GRB     Pixels are wired for GRB bitstream (most NeoPixel products)
//   NEO_RGB     Pixels are wired for RGB bitstream (v1 FLORA pixels, not v2)
//   NEO_RGBW    Pixels are wired for RGBW bitstream (NeoPixel RGBW products)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);

// IMPORTANT: To reduce NeoPixel burnout risk, add 1000 uF capacitor across
// pixel power leads, add 300 - 500 Ohm resistor on first pixel's data input
// and minimize distance between Arduino and first pixel.  Avoid connecting
// on a live circuit...if you must, connect GND first.


static unsigned long startTime = millis();
int timer = 60;

void setup() {
  Serial.begin(9600);

  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  
  //  setup colors and pattern
  colorWipeReverse(strip.Color(0, 0, 50), 10);
  colorWipe(strip.Color(50, 50, 0), 10);

}

void loop() {
  if(actAfterDelay(startTime, 1000)){
    updateTimerBar(0, NUMPIXELS, timer, 0, 60);
    timer--;
  }
  static int i = constrain(i, 0, 3);
  if (timer < 0){
    if(actAfterDelay(startTime, 500)){
      i++;
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
}

void updateTimerBar(int firstPixel, int lastPixel, int currentTime, int minTime, int maxTime){
  int numberOfPixels = lastPixel - firstPixel + 1;
  currentTime = constrain(currentTime, minTime, maxTime);
  int currentPixel = map(currentTime, minTime, maxTime, 0, numberOfPixels);
  for(int i=firstPixel; i<=lastPixel; i++){
    if(i < currentPixel){
      strip.setPixelColor(i, strip.Color(50, 50, 0)); //yellow
    }
    else{
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

bool actAfterDelay(unsigned long startTime, int timeInterval) {
  static unsigned long prevTime = startTime;
  unsigned long currentTime = millis();
  if ((currentTime - prevTime) >= timeInterval) {
    prevTime = currentTime;
    return true;
  }
  else {
    return false;
  }
}

void flashColor(uint32_t c){
  for(int i=0; i<NUMPIXELS; i++){
    strip.setPixelColor(i, c);
  }
  strip.show();
}


#include <Wire.h>

void setup() {
  // put your setup code here, to run once:
  Wire.begin();
}

char x = 'c';
void loop() {
  // put your main code here, to run repeatedly:
  Wire.beginTransmission(8);
  Wire.write(x);
  Wire.endTransmission();
  delay(500);
}

#include <Wire.h>
#include <VL53L0X.h>

VL53L0X lidar;

const int motorPin = 9;
const int maxDistance = 200;
const int minDistance = 20;

int distance = 0;
int vibrationStrength = 0;

float smoothedDistance = 0;
float alpha = 0.2;

void setup() {
  Wire.begin();
  
  pinMode(motorPin, OUTPUT);
  
  lidar.init();
  lidar.setTimeout(500);
  lidar.startContinuous();
  
  int firstReading = lidar.readRangeContinuousMillimeters() / 10;
  smoothedDistance = firstReading;
}

void loop() {
  int rawReading = lidar.readRangeContinuousMillimeters();
  distance = rawReading / 10;
  
  if (lidar.timeoutOccurred()) {
    analogWrite(motorPin, 0);
    return;
  }
  
  smoothedDistance = (alpha * distance) + ((1.0 - alpha) * smoothedDistance);
  int filteredDistance = (int)smoothedDistance;
  
  if (filteredDistance < minDistance) {
    vibrationStrength = 255;
  } else if (filteredDistance > maxDistance) {
    vibrationStrength = 0;
  } else {
    int range = maxDistance - minDistance;
    int distanceFromMin = filteredDistance - minDistance;
    float ratio = (float)distanceFromMin / (float)range;
    float inverted = 1.0 - ratio;
    vibrationStrength = (int)(inverted * 255);
  }
  
  analogWrite(motorPin, vibrationStrength);
  
  delay(50);
}

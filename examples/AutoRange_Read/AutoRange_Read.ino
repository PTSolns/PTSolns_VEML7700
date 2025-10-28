// Example: Basic Read
// Last Update: Oct 28, 2025
//
// DESCRIPTION
// 1) Enables autorange so the library adjusts gain and integration time automatically
// 2) Keeps readings within a good dynamic range and avoids saturation
// 3) Prints current lux value once per second
//
// REFERENCES
// Website: http://PTSolns.com
// Documentation Repository: http://docs.PTSolns.com
// Newsletter Signup: http://eepurl.com/hnESjL
// Tinker Thoughts Blog: https://PTSolns.com/Tinker-Thoughts

#include <Wire.h>
#include <PTSolns_VEML7700.h>

PTSolns_VEML7700 als;

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  if (!als.begin()) {
    Serial.println("VEML7700 begin failed");
    for (;;) {}
  }
  als.enableAutoRange(true);
  als.setPowerSave(false); // Fully disables power save mode (if this was used in a previous sketch...)
}

void loop() {
  float lux;
  if (als.readLux(lux) == VEML_OK) {
    Serial.print("Lux="); Serial.println(lux, 2);
  } else {
    Serial.print("Error="); Serial.println((int)als.lastError());
  }
  delay(1000);
}

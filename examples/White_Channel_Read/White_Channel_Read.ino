// Example: Basic Read
// Last Update: Oct 28, 2025
//
// DESCRIPTION
// 1) Reads both ALS and White channels
// 2) ALS is intended to approximate human eye response and is converted to lux
// 3) White is a broader band measurement including more IR and is returned as raw counts
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
  als.setPowerSave(false); // Fully disables power save mode (if this was used in a previous sketch...)
}

void loop() {
  uint16_t rawAls = 0;
  uint16_t rawWhite = 0;
  float lux = 0.0f;

  if (als.readALSRaw(rawAls) == VEML_OK &&
      als.readWhiteRaw(rawWhite) == VEML_OK &&
      als.readLux(lux) == VEML_OK) {
    Serial.print("ALS_raw=");   Serial.print(rawAls);
    Serial.print(" Lux=");      Serial.print(lux, 2);
    Serial.print(" White_raw="); Serial.println(rawWhite);
  } else {
    Serial.print("Error="); Serial.println((int)als.lastError());
  }
  delay(1000);
}

// Example: Basic Read
// Last Update: Oct 28, 2025
//
// DESCRIPTION
// 1) Initializes the VEML7700 at 0x10 using Wire at 100 kHz
// 2) Reads raw ALS and converts to lux using current gain and integration time
// 3) Prints both values once per second
//
// REFERENCES
// Website: http://PTSolns.com
// Documentation Repository: http://docs.PTSolns.com
// Newsletter Signup: http://eepurl.com/hnESjL
// Tinker Thoughts Blog: https://PTSolns.com/Tinker-Thoughts

/* NOTE 1
Raw and lux are two different forms of the same light measurement from the VEML7700.

Raw reading
- The sensor outputs a 16 bit number that represents the amount of light detected by the photodiode.
- It is not corrected for gain or integration time.
- It has no unit and cannot be compared between different sensor settings.

Lux reading
- Lux is a calculated value that applies the resolution factor based on the selected gain and integration time.
- It represents light intensity as perceived by the human eye.
- Lux is comparable between sensors and between different settings.
*/

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
  uint16_t raw;
  float lux;
  if (als.readALSRaw(raw) == VEML_OK && als.readLux(lux) == VEML_OK) {
    Serial.print("ALS_raw="); Serial.print(raw);
    Serial.print(" Lux="); Serial.println(lux, 2);
  } else {
    Serial.print("Error="); Serial.println((int)als.lastError());
  }
  delay(1000);
}

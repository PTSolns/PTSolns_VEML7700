// Example: Low Power - Power Save
// Last Update: Oct 28, 2025
//
// DESCRIPTION
// 1) Demonstrates all four Power Save Modes PSM 1 to PSM 4 and compares to Shutdown
// 2) Each loop iteration sets a PSM mode, waits, reads lux, then advances to the next mode
// 3) Use this to observe current reduction and response timing on your hardware
// 4) 10 sec between each PSM setting. Use a high accuracy current meter (E.g. PPK2) to measure current consumption during these modes.
//
// REFERENCES
// Website: http://PTSolns.com
// Documentation Repository: http://docs.PTSolns.com
// Newsletter Signup: http://eepurl.com/hnESjL
// Tinker Thoughts Blog: https://PTSolns.com/Tinker-Thoughts

/* NOTE 1
The VEML7700 has four Power Saving Modes named PSM1 to PSM4.
When PSM is enabled the sensor reduces its measurement frequency to lower average current.
Higher mode number gives lower power usage but also slower update rate.

PSM1
Lowest level of power saving
Fastest refresh rate of the four PSM modes
Highest current consumption of the four PSM modes

PSM2
More power saving than PSM1
Slower refresh rate than PSM1

PSM3
More power saving than PSM2
Slower refresh rate than PSM2

PSM4
Maximum power saving
Slowest refresh rate
Lowest current consumption of the four PSM modes
*/

#include <Wire.h>
#include <PTSolns_VEML7700.h>

PTSolns_VEML7700 als;

static const uint32_t DEMO_DELAY_MS = 10000; // wait time per mode
static uint8_t mode = 0; // 0..4 where 0 = Shutdown demo then PSM1..PSM4

void setup() {
  Serial.begin(115200);
  while (!Serial) {}
  if (!als.begin()) {
    Serial.println("VEML7700 begin failed");
    for (;;) {}
  }
}

void loop() {
  float lux = 0.0f;

  if (mode == 0) {
    Serial.println("Mode 0: Shutdown ON then OFF and read once");
    als.shutdown(true); // Turns the sensor off
    delay(DEMO_DELAY_MS);
    als.shutdown(false); // Wakes the sensor up
    delay(5);
    als.readLux(lux);
    Serial.print("Lux="); Serial.println(lux, 2);
  } else {
    uint8_t psm = mode - 1; // 0..3 maps to PSM1..PSM4
    als.setPowerSave(true, psm);
    Serial.print("PSM "); Serial.print(psm + 1); Serial.print(": ");
    delay(DEMO_DELAY_MS);
    als.readLux(lux);
    Serial.print("Lux="); Serial.println(lux, 2);
  }

  mode = (mode + 1) % 5; // 0..4 cycle
  delay(10000);
}

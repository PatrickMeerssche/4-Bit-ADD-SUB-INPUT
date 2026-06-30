#include <Arduino.h>

// Set TABLETEST to 1 to sweep all 256 combinations every 200 ms.
// Set TABLETEST to 0 to keep INPUT1 and INPUT2 fixed.
// Set TIME to the desired delay in milliseconds between each step when sweeping combinations.

//1bit -> PA0
//2bit -> PA1
//3bit -> PA4
//4bit -> PA5
//1.1bit -> PA6
//2.1bit -> PA7
//3.1bit -> PB0
//4.1bit -> PB1

int TABLETEST = 0; // 1 = sweep all combinations, 0 = fixed inputs
int TIME = 2; // Time in milliseconds between each step when sweeping combinations

// Fixed 4-bit inputs.
// Write values as 4-digit binary-looking numbers, e.g. 0110, 1010, 0011.
// The code will convert them automatically to the correct binary value.
uint16_t INPUT1 = 0000;
uint16_t INPUT2 = 0001;

// Avoid PA2/PA3 here because they are commonly used by Serial (USART2).
const uint8_t A_PINS[4] = {PA0, PA1, PA4, PA5};
const uint8_t B_PINS[4] = {PA6, PA7, PB0, PB1};
const uint8_t ACTIVITY_LED_PIN = PC13;

unsigned long lastStepMs = 0;
uint8_t autoA = 0;
uint8_t autoB = 0;

void writeNibbleToPins(uint8_t value, const uint8_t pins[4]);
void writeInputs(uint8_t first, uint8_t second);
void pulseActivityLed();
uint8_t parseBin(uint16_t val);

void setup() {
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(A_PINS[i], OUTPUT);
    pinMode(B_PINS[i], OUTPUT);
  }

  pinMode(ACTIVITY_LED_PIN, OUTPUT);
  digitalWrite(ACTIVITY_LED_PIN, HIGH);

  if (TABLETEST == 0) {
    writeInputs(parseBin(INPUT1), parseBin(INPUT2));
  } else {
    writeInputs(0, 0);
  }
}

void loop() {
  if (TABLETEST == 1) {
    unsigned long now = millis();
    if (now - lastStepMs >= TIME) {
      lastStepMs = now;
      writeInputs(autoA, autoB);

      autoB++;
      if (autoB > 15) {
        autoB = 0;
        autoA++;
        if (autoA > 15) {
          autoA = 0;
        }
      }
    }
    return;
  }

  if (TABLETEST == 0) {
    // Static mode: outputs are already set in setup().
    return;
  }
}

void writeNibbleToPins(uint8_t value, const uint8_t pins[4]) {
  for (uint8_t bit = 0; bit < 4; bit++) {
    digitalWrite(pins[bit], (value >> bit) & 0x01);
  }
}

void writeInputs(uint8_t first, uint8_t second) {
  writeNibbleToPins(first & 0x0F, A_PINS);
  writeNibbleToPins(second & 0x0F, B_PINS);
  pulseActivityLed();
}

// Converts a binary-looking decimal (e.g. 1010) to its real value (10).
// Each decimal digit is treated as one binary bit (only 0 and 1 are valid).
uint8_t parseBin(uint16_t val) {
  uint8_t result = 0;
  for (uint8_t i = 0; i < 4; i++) {
    result |= ((val % 10) & 1) << i;
    val /= 10;
  }
  return result;
}

void pulseActivityLed() {
  // Bluepill onboard LED at PC13 is active-low.
  digitalWrite(ACTIVITY_LED_PIN, LOW);
  delay(20);
  digitalWrite(ACTIVITY_LED_PIN, HIGH);
}
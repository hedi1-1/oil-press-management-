#include <SPI.h>

// Many RC522 clone boards are unstable at default 4MHz SPI.
// Force a slower bus to improve reliability.
#ifndef MFRC522_SPICLOCK
#define MFRC522_SPICLOCK (100000u)
#endif

#include <MFRC522.h>

// This probe scans common SS/RST pin combinations and reports
// where an RC522 responds on SPI.

const uint8_t SS_CANDIDATES[]  = {10, 9, 8, 7, 6, 5, 4, 3, 2};
const uint8_t RST_CANDIDATES[] = {9, 8, 7, 6, 5, 4, 3, 2};

static bool isValidVersion(byte v) {
  return v != 0x00 && v != 0xFF;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  Serial.begin(9600);
  delay(1500);

  Serial.println();
  Serial.println("==== RC522 PIN PROBE START ====");
  Serial.println("Board: Arduino UNO");
  Serial.println("Scanning SS/RST combinations...");

  SPI.begin();

  bool foundAny = false;

  for (uint8_t r = 0; r < sizeof(RST_CANDIDATES); r++) {
    for (uint8_t s = 0; s < sizeof(SS_CANDIDATES); s++) {
      const uint8_t rstPin = RST_CANDIDATES[r];
      const uint8_t ssPin  = SS_CANDIDATES[s];

      if (rstPin == ssPin) {
        continue;
      }

      MFRC522 probe(ssPin, rstPin);
      probe.PCD_Init();
      delay(30);

      const byte version = probe.PCD_ReadRegister(MFRC522::VersionReg);

      Serial.print("TRY SS=");
      Serial.print(ssPin);
      Serial.print(" RST=");
      Serial.print(rstPin);
      Serial.print(" -> VersionReg=0x");
      Serial.println(version, HEX);

      if (isValidVersion(version)) {
        foundAny = true;
        Serial.println("FOUND: RC522 RESPONDING");
        Serial.print("USE THIS IN SKETCH: SS_PIN=");
        Serial.print(ssPin);
        Serial.print(" RST_PIN=");
        Serial.println(rstPin);
        probe.PCD_DumpVersionToSerial();
        Serial.println("-------------------------------");
      }
    }
  }

  if (!foundAny) {
    Serial.println("RESULT: NO RC522 RESPONSE ON ANY TESTED PINS");
    Serial.println("LIKELY: power/wiring/contact/module issue");
    Serial.println("CHECK: 3.3V only, common GND, wire orientation, and try another RC522 module");
  } else {
    Serial.println("RESULT: RC522 DETECTED ON AT LEAST ONE PIN COMBINATION");
  }

  Serial.println("==== PIN PROBE COMPLETE ====");
}

void loop() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(80);
  digitalWrite(LED_BUILTIN, LOW);
  delay(920);
}

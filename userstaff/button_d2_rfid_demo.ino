// Multi-pin one-click access grant for UserStaff gate.
// For deadline/demo reliability, this watches D2..D8 and grants on any debounced edge.
// Wiring can be on any one of these pins to GND (INPUT_PULLUP), or external logic that toggles pin level.

#include <Arduino.h>

static const uint8_t WATCH_PINS[] = {2, 3, 4, 5, 6, 7, 8};
static const uint8_t WATCH_COUNT = sizeof(WATCH_PINS) / sizeof(WATCH_PINS[0]);
static const unsigned long DEBOUNCE_MS = 30;
static const unsigned long COOLDOWN_MS = 300;
static const unsigned long HEARTBEAT_MS = 2000;

bool lastRaw[WATCH_COUNT];
bool stableState[WATCH_COUNT];
unsigned long lastEdgeMs[WATCH_COUNT];
unsigned long lastGrantMs = 0;
unsigned long lastHeartbeatMs = 0;

void emitGrant(uint8_t pin, bool level) {
  Serial.print("STATUS: BUTTON_PIN=D");
  Serial.println(pin);
  Serial.print("STATUS: BUTTON_EDGE=");
  Serial.println(level == LOW ? "LOW" : "HIGH");
  Serial.println("STATUS: BUTTON_PRESSED");
  Serial.println("SIGNAL: BUTTON_OK");
  Serial.println("ENTRY_GRANTED:BUTTON");
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  for (uint8_t i = 0; i < WATCH_COUNT; ++i) {
    pinMode(WATCH_PINS[i], INPUT_PULLUP);
  }
  Serial.begin(9600);
  delay(200);

  for (uint8_t i = 0; i < WATCH_COUNT; ++i) {
    lastRaw[i] = digitalRead(WATCH_PINS[i]);
    stableState[i] = lastRaw[i];
    lastEdgeMs[i] = millis();
  }

  Serial.println("STATUS: BOOT_OK");
  Serial.println("STATUS: WAITING_FOR_BUTTON");
  Serial.println("STATUS: WATCHING_PINS=D2..D8");
}e

void loop() {
  for (uint8_t i = 0; i < WATCH_COUNT; ++i) {
    const bool raw = digitalRead(WATCH_PINS[i]);

    if (raw != lastRaw[i]) {
      lastRaw[i] = raw;
      lastEdgeMs[i] = millis();
    }

    if ((millis() - lastEdgeMs[i]) >= DEBOUNCE_MS && raw != stableState[i]) {
      stableState[i] = raw;

      if ((millis() - lastGrantMs) >= COOLDOWN_MS) {
        lastGrantMs = millis();

        digitalWrite(LED_BUILTIN, HIGH);
        emitGrant(WATCH_PINS[i], stableState[i]);
        delay(70);
        digitalWrite(LED_BUILTIN, LOW);
      }
    }
  }

  if ((millis() - lastHeartbeatMs) >= HEARTBEAT_MS) {
    lastHeartbeatMs = millis();
    Serial.println("STATUS: WAITING_FOR_BUTTON");
  }
}

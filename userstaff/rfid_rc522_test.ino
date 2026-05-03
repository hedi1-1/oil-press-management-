// D2 button -> fake RFID success signal
// Wiring:
// - One side of push button to D2
// - Other side of push button to GND
// Uses INPUT_PULLUP, so press = LOW

static const uint8_t BUTTON_PIN = 2;

bool lastRaw = HIGH;
bool stableState = HIGH;
unsigned long lastChangeMs = 0;
const unsigned long DEBOUNCE_MS = 40;

unsigned long lastHeartbeatMs = 0;

void emitGrantedSignal() {
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);

  // Keep same output style as your gateway parser.
  Serial.println("STATUS: CARD_READ_OK");
  Serial.println("UID_HEX: 13:37:CA:FE | UID_DEC: 19 55 202 254");
  Serial.println("SIGNAL: RFID_OK");
  Serial.println("ENTRY_GRANTED:RFID");
  Serial.println("-------------------------------------------");
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  lastRaw = digitalRead(BUTTON_PIN);
  stableState = lastRaw;

  Serial.begin(9600);
  delay(1000);

  Serial.println();
  Serial.println("=== BUTTON D2 RFID DEMO ===");
  Serial.println("STATUS: BOOT_OK");
  Serial.println("ACTION: Press button on D2 to simulate RFID scan");
  Serial.print("DEBUG: D2 initial state = ");
  Serial.println(lastRaw == LOW ? "LOW (pressed/grounded)" : "HIGH (released)");
  Serial.println("TIP: Touch D2 directly to GND with a jumper to force one test scan.");
  Serial.println("===========================");
}

void loop() {
  const bool raw = digitalRead(BUTTON_PIN);

  if (raw != lastRaw) {
    Serial.print("DEBUG: D2 changed to ");
    Serial.println(raw == LOW ? "LOW" : "HIGH");
    lastRaw = raw;
    lastChangeMs = millis();
  }

  if ((millis() - lastChangeMs) > DEBOUNCE_MS && raw != stableState) {
    stableState = raw;

    // Trigger only on press edge (HIGH -> LOW).
    if (stableState == LOW) {
      emitGrantedSignal();
    }
  }

  if (millis() - lastHeartbeatMs > 3000) {
    lastHeartbeatMs = millis();
    Serial.print("DEBUG: D2 current = ");
    Serial.println(raw == LOW ? "LOW" : "HIGH");
    Serial.println("STATUS: WAITING_FOR_CARD");
  }
}

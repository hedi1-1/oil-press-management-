// Button wiring diagnostic for Arduino Uno/Nano.
// Goal: identify whether the button is really on D2, on another pin,
// or not electrically connected.
//
// How to use:
// 1) Upload this sketch.
// 2) Open serial monitor at 9600 baud.
// 3) During each phase, press and release your button multiple times.
// 4) Read the final DIAGNOSIS section.

static const uint8_t TEST_PINS[] = {
  2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, A0, A1, A2, A3, A4, A5
};
static const uint8_t PIN_COUNT = sizeof(TEST_PINS) / sizeof(TEST_PINS[0]);

static const unsigned long SAMPLE_MS = 8;
static const unsigned long PHASE_MS = 15000;

enum Phase {
  PHASE_PULLUP = 0,
  PHASE_FLOAT = 1,
  PHASE_DONE = 2
};

Phase phase = PHASE_PULLUP;
unsigned long phaseStartMs = 0;
unsigned long lastSampleMs = 0;
unsigned long lastTickMs = 0;

int8_t lastPullup[PIN_COUNT];
int8_t lastFloat[PIN_COUNT];

bool changedPullup[PIN_COUNT];
bool lowSeenPullup[PIN_COUNT];

bool changedFloat[PIN_COUNT];
bool lowSeenFloat[PIN_COUNT];
bool highSeenFloat[PIN_COUNT];

const char* pinLabel(uint8_t pin) {
  switch (pin) {
    case A0: return "A0";
    case A1: return "A1";
    case A2: return "A2";
    case A3: return "A3";
    case A4: return "A4";
    case A5: return "A5";
    default: {
      static char buf[4];
      buf[0] = 'D';
      if (pin >= 10) {
        buf[1] = char('0' + (pin / 10));
        buf[2] = char('0' + (pin % 10));
        buf[3] = '\0';
      } else {
        buf[1] = char('0' + pin);
        buf[2] = '\0';
      }
      return buf;
    }
  }
}

void printEvent(const char* mode, uint8_t pin, int fromState, int toState) {
  Serial.print("EVENT ");
  Serial.print(mode);
  Serial.print(" ");
  Serial.print(pinLabel(pin));
  Serial.print(" ");
  Serial.print(fromState == HIGH ? "HIGH" : "LOW");
  Serial.print(" -> ");
  Serial.println(toState == HIGH ? "HIGH" : "LOW");
}

void startPullupPhase() {
  Serial.println();
  Serial.println("=== PHASE 1: INPUT_PULLUP SCAN (expected for D2-button-to-GND) ===");
  Serial.println("Press/release your button multiple times for 15s...");

  for (uint8_t i = 0; i < PIN_COUNT; ++i) {
    uint8_t p = TEST_PINS[i];
    pinMode(p, INPUT_PULLUP);
    int v = digitalRead(p);
    lastPullup[i] = (int8_t)v;
    changedPullup[i] = false;
    lowSeenPullup[i] = (v == LOW);
  }

  phase = PHASE_PULLUP;
  phaseStartMs = millis();
  lastTickMs = 0;
}

void startFloatPhase() {
  Serial.println();
  Serial.println("=== PHASE 2: INPUT (FLOAT/EXTERNAL RESISTOR) SCAN ===");
  Serial.println("Press/release your button multiple times for 15s...");

  for (uint8_t i = 0; i < PIN_COUNT; ++i) {
    uint8_t p = TEST_PINS[i];
    pinMode(p, INPUT);
  }

  delay(20);

  for (uint8_t i = 0; i < PIN_COUNT; ++i) {
    int v = digitalRead(TEST_PINS[i]);
    lastFloat[i] = (int8_t)v;
    changedFloat[i] = false;
    lowSeenFloat[i] = (v == LOW);
    highSeenFloat[i] = (v == HIGH);
  }

  phase = PHASE_FLOAT;
  phaseStartMs = millis();
  lastTickMs = 0;
}

void printPhaseTick() {
  unsigned long elapsed = millis() - phaseStartMs;
  unsigned long left = (elapsed < PHASE_MS) ? (PHASE_MS - elapsed) : 0;

  if (lastTickMs == 0 || millis() - lastTickMs >= 1000) {
    lastTickMs = millis();
    Serial.print("TIME_LEFT_MS=");
    Serial.println(left);
  }
}

void scanPullup() {
  for (uint8_t i = 0; i < PIN_COUNT; ++i) {
    int v = digitalRead(TEST_PINS[i]);
    if (v != lastPullup[i]) {
      changedPullup[i] = true;
      printEvent("PULLUP", TEST_PINS[i], lastPullup[i], v);
      lastPullup[i] = (int8_t)v;
    }
    if (v == LOW) {
      lowSeenPullup[i] = true;
    }
  }
}

void scanFloat() {
  for (uint8_t i = 0; i < PIN_COUNT; ++i) {
    int v = digitalRead(TEST_PINS[i]);
    if (v != lastFloat[i]) {
      changedFloat[i] = true;
      printEvent("FLOAT", TEST_PINS[i], lastFloat[i], v);
      lastFloat[i] = (int8_t)v;
    }
    if (v == LOW) {
      lowSeenFloat[i] = true;
    }
    if (v == HIGH) {
      highSeenFloat[i] = true;
    }
  }
}

void summarize() {
  Serial.println();
  Serial.println("=== SUMMARY ===");

  int pullupCandidates = 0;
  int floatCandidates = 0;
  bool d2PullupActive = false;
  bool d2FloatActive = false;

  for (uint8_t i = 0; i < PIN_COUNT; ++i) {
    bool pActive = changedPullup[i] && lowSeenPullup[i];
    bool fActive = changedFloat[i] && lowSeenFloat[i] && highSeenFloat[i];

    if (pActive) {
      ++pullupCandidates;
      Serial.print("PULLUP_ACTIVE: ");
      Serial.println(pinLabel(TEST_PINS[i]));
      if (TEST_PINS[i] == 2) {
        d2PullupActive = true;
      }
    }

    if (fActive) {
      ++floatCandidates;
      Serial.print("FLOAT_ACTIVE: ");
      Serial.println(pinLabel(TEST_PINS[i]));
      if (TEST_PINS[i] == 2) {
        d2FloatActive = true;
      }
    }
  }

  Serial.println();
  Serial.println("=== DIAGNOSIS ===");

  if (d2PullupActive) {
    Serial.println("OK: D2 is electrically changing in INPUT_PULLUP mode.");
    Serial.println("Use wiring: D2 <-> button <-> GND and keep INPUT_PULLUP sketch.");
  } else if (pullupCandidates > 0) {
    Serial.println("WRONG_PIN: Your button changes a pin, but not D2.");
    Serial.println("Move your signal wire to D2 (or update software to that pin).");
  } else if (d2FloatActive) {
    Serial.println("EXTERNAL_MODE: D2 changes only in FLOAT mode.");
    Serial.println("You likely have external resistor wiring (pulldown/pullup). Check resistor and +5V/GND path.");
  } else if (floatCandidates > 0) {
    Serial.println("WRONG_PIN_OR_FLOATING: Activity found on non-D2 pin in FLOAT mode.");
    Serial.println("Breadboard row/pin mapping is likely wrong.");
  } else {
    Serial.println("NO_SIGNAL: No tested pin showed real press/release transitions.");
    Serial.println("Likely causes: bad button orientation, missing GND/5V, broken jumper, wrong board/port.");
  }

  Serial.println();
  Serial.println("Tip: quick hardware proof -> touch D2 directly to GND momentarily.");
  Serial.println("If that is detected, software is fine and the pushbutton wiring is the issue.");
  Serial.println("=== END ===");
}

void setup() {
  Serial.begin(9600);
  delay(600);

  Serial.println();
  Serial.println("BUTTON DIAGNOSTIC START");
  Serial.println("Board: Arduino Uno/Nano style expected");

  startPullupPhase();
}

void loop() {
  if (phase == PHASE_DONE) {
    return;
  }

  if (millis() - lastSampleMs >= SAMPLE_MS) {
    lastSampleMs = millis();

    if (phase == PHASE_PULLUP) {
      scanPullup();
    } else if (phase == PHASE_FLOAT) {
      scanFloat();
    }

    printPhaseTick();
  }

  unsigned long elapsed = millis() - phaseStartMs;

  if (phase == PHASE_PULLUP && elapsed >= PHASE_MS) {
    startFloatPhase();
  } else if (phase == PHASE_FLOAT && elapsed >= PHASE_MS) {
    summarize();
    phase = PHASE_DONE;
  }
}

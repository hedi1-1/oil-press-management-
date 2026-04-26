#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <stdlib.h>

// ---------------- Hardware config ----------------
// Analog directional control (e.g. joystick Y axis)
const int PIN_ANALOG_X = A0; // VRx
const int PIN_ANALOG_Y = A1; // VRy
const int PIN_JOY_SW = 2;    // SW (active LOW)
const int PIN_BUZZER = 3;
const bool BUZZER_ENABLED = true;
// Set true only if buzzer is wired active-low (e.g. +5V -> buzzer -> pin).
const bool BUZZER_ACTIVE_LOW = false;

// 2-digit 7-seg (manual multiplex, no external library)
// Segment order: A,B,C,D,E,F,G,DP
const byte digitPins[] = {10, 11};
const byte segmentPins[] = {4, 5, 6, 7, 8, 9, 12, 13};
const byte DIGIT_MASKS[10] = {
  0b00111111, // 0
  0b00000110, // 1
  0b01011011, // 2
  0b01001111, // 3
  0b01100110, // 4
  0b01101101, // 5
  0b01111101, // 6
  0b00000111, // 7
  0b01111111, // 8
  0b01101111  // 9
};

// LCD I2C (16x2)
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------------- Business rules ----------------
const float MAX_LITERS = 20.0f;
const float MIN_THRESHOLD_L = 10.0f;
const unsigned long SERIAL_PERIOD_MS = 500;
const unsigned long STABLE_DELAY_MS = 1200;
const float STEP_LITERS = 1.0f;
const unsigned long LOW_WEIGHT_BUZZ_MS = 3000;

// Analog thresholds around center (0..1023)
const int ANALOG_UP_THRESHOLD = 700;
const int ANALOG_DOWN_THRESHOLD = 300;
const unsigned long SW_DEBOUNCE_MS = 180;

float currentLiters = 0.0f;
bool validated = false;
String currentStatus = "FAIBLE";

unsigned long lastSerialMs = 0;
unsigned long lastChangeMs = 0;
unsigned long lastSwPressMs = 0;
bool lastSwState = HIGH;
unsigned long buzzerStopMs = 0;
unsigned long lcdFeedbackUntilMs = 0;
unsigned long lcdStepFeedbackUntilMs = 0;
float lastStepDelta = 0.0f;
bool lastConfirmSuccess = false;
unsigned long stockIdCounter = 1;
unsigned long confirmedStockId = 0;
int lastJoyZone = 0; // -1=DOWN, 0=NEUTRAL, +1=UP

void disableAllDigits()
{
  // Common cathode digits: HIGH disables digit, LOW enables digit.
  digitalWrite(digitPins[0], HIGH);
  digitalWrite(digitPins[1], HIGH);
}

void writeSegmentsMask(byte mask)
{
  for (byte i = 0; i < 8; ++i) {
    const bool on = (i < 7) ? ((mask >> i) & 0x01) : false; // DP off
    digitalWrite(segmentPins[i], on ? HIGH : LOW);
  }
}

void setBuzzer(bool on)
{
  if (!BUZZER_ENABLED) {
    // High-impedance mode: prevents buzz on both active-high and active-low wirings.
    pinMode(PIN_BUZZER, INPUT);
    return;
  }

  pinMode(PIN_BUZZER, OUTPUT);

  if (BUZZER_ACTIVE_LOW) {
    digitalWrite(PIN_BUZZER, on ? LOW : HIGH);
  } else {
    digitalWrite(PIN_BUZZER, on ? HIGH : LOW);
  }
}

void setStatusByThreshold()
{
  currentStatus = (currentLiters < MIN_THRESHOLD_L) ? "FAIBLE" : "STABLE";
}

void handleAnalogDirection()
{
  unsigned long now = millis();
  int analogValue = analogRead(PIN_ANALOG_Y);
  int joyZone = 0;

  if (analogValue >= ANALOG_UP_THRESHOLD) {
    joyZone = +1;
  } else if (analogValue <= ANALOG_DOWN_THRESHOLD) {
    joyZone = -1;
  }

  if (validated) {
    lastJoyZone = joyZone;
    return;
  }

  // One movement in UP/DOWN zone from neutral triggers one 1L step.
  if (lastJoyZone == 0 && joyZone != 0) {
    lastStepDelta = 0.0f;
    if (joyZone > 0) {
      currentLiters += STEP_LITERS;
      if (currentLiters > MAX_LITERS) {
        currentLiters = MAX_LITERS;
      }
      lastStepDelta = STEP_LITERS;
    } else {
      currentLiters -= STEP_LITERS;
      if (currentLiters < 0.0f) {
        currentLiters = 0.0f;
      }
      lastStepDelta = -STEP_LITERS;
    }
    currentStatus = "REMPLISSAGE";
    lastChangeMs = now;
    lcdFeedbackUntilMs = 0;
    lcdStepFeedbackUntilMs = now + 900;
  }

  lastJoyZone = joyZone;
}

void handleSwitchConfirm()
{
  unsigned long now = millis();
  bool swState = digitalRead(PIN_JOY_SW);

  if (lastSwState == HIGH && swState == LOW && (now - lastSwPressMs >= SW_DEBOUNCE_MS)) {
    lastSwPressMs = now;

    if (validated) {
      validated = false;
      currentLiters = 0.0f;
      currentStatus = "REMPLISSAGE";
      lastStepDelta = 0.0f;
      lcdStepFeedbackUntilMs = 0;
      buzzerStopMs = 0;
      lcdFeedbackUntilMs = 0;
      confirmedStockId = 0;
      lastJoyZone = 0;
    } else {
      if (currentLiters >= MIN_THRESHOLD_L) {
        validated = true;
        currentStatus = "VALIDATED";
        lastConfirmSuccess = true;
        confirmedStockId = stockIdCounter++;
        lcdFeedbackUntilMs = now + 3000;
        buzzerStopMs = 0;
      } else {
        validated = true;
        currentStatus = "FAIBLE_CONFIRMED";
        lastConfirmSuccess = false;
        confirmedStockId = 0;
        lcdFeedbackUntilMs = now + 3000;
        buzzerStopMs = now + LOW_WEIGHT_BUZZ_MS;
      }
    }

    // Emit immediate payload on confirmation for Qt integration.
    Serial.print("WEIGHT:");
    Serial.println(currentLiters, 1);
    Serial.print("STATUS:");
    Serial.println(currentStatus);
    Serial.print("STOCK_ID:");
    Serial.println(confirmedStockId);

    lastChangeMs = now;
  }

  lastSwState = swState;
}

void updateBuzzer()
{
  if (buzzerStopMs > millis()) {
    setBuzzer(true);
    return;
  }

  setBuzzer(false);
}

void updateStatusTiming()
{
  if (validated) {
    return;
  }

  if (millis() - lastChangeMs > STABLE_DELAY_MS && currentStatus == "REMPLISSAGE") {
    setStatusByThreshold();
  }
}

void updateLcd()
{
  lcd.setCursor(0, 0);
  char line1[17];
  char litersBuf[8];
  dtostrf(currentLiters, 4, 1, litersBuf);
  snprintf(line1, sizeof(line1), "Poids:%sL", litersBuf);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(line1);

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);

  if (millis() < lcdFeedbackUntilMs) {
    if (lastConfirmSuccess) {
      char successLine[17];
      snprintf(successLine, sizeof(successLine), "SUCCESS ID:%04lu", confirmedStockId);
      lcd.print(successLine);
    } else {
      lcd.print("ECHEC <10L");
    }
    return;
  }

  if (millis() < lcdStepFeedbackUntilMs && lastStepDelta != 0.0f) {
    char stepLine[17];
    char deltaBuf[8];
    char totalBuf[8];
    const float absDelta = (lastStepDelta >= 0.0f) ? lastStepDelta : -lastStepDelta;
    dtostrf(absDelta, 3, 1, deltaBuf);
    dtostrf(currentLiters, 4, 1, totalBuf);
    if (lastStepDelta >= 0.0f) {
      snprintf(stepLine, sizeof(stepLine), "+%sL -> %sL", deltaBuf, totalBuf);
    } else {
      snprintf(stepLine, sizeof(stepLine), "-%sL -> %sL", deltaBuf, totalBuf);
    }
    lcd.print(stepLine);
    return;
  }

  if (currentStatus == "REMPLISSAGE") {
    lcd.print("Remplissage");
  } else if (currentStatus == "STABLE") {
    lcd.print("Stable");
  } else if (currentStatus == "FAIBLE") {
    lcd.print("Niveau faible");
  } else if (currentStatus == "VALIDATED") {
    lcd.print("Valide");
  } else {
    lcd.print(currentStatus);
  }
}

void update7Seg()
{
  static unsigned long lastMuxMicros = 0;
  static byte activeDigit = 0;

  if (micros() - lastMuxMicros < 2000) {
    return;
  }
  lastMuxMicros = micros();

  int valueInt = (int)roundf(currentLiters);
  if (valueInt < 0) valueInt = 0;
  if (valueInt > 99) valueInt = 99;
  int tens = valueInt / 10;
  int ones = valueInt % 10;

  disableAllDigits();

  byte digitVal = (activeDigit == 0) ? (byte)tens : (byte)ones;
  writeSegmentsMask(DIGIT_MASKS[digitVal]);
  digitalWrite(digitPins[activeDigit], LOW);

  activeDigit = 1 - activeDigit;
}

void sendSerial()
{
  unsigned long now = millis();
  if (now - lastSerialMs >= SERIAL_PERIOD_MS) {
    lastSerialMs = now;

    Serial.print("WEIGHT:");
    Serial.println(currentLiters, 1);

    Serial.print("STATUS:");
    Serial.println(currentStatus);
  }
}

void setup()
{
  Serial.begin(9600);

  // Read joystick X once so the compiler keeps explicit mapping VRx->A0.
  analogRead(PIN_ANALOG_X);
  pinMode(PIN_JOY_SW, INPUT_PULLUP);

  // Keep buzzer electrically neutral when disabled.
  setBuzzer(false);

  for (byte i = 0; i < 8; ++i) {
    pinMode(segmentPins[i], OUTPUT);
    digitalWrite(segmentPins[i], LOW);
  }
  for (byte i = 0; i < 2; ++i) {
    pinMode(digitPins[i], OUTPUT);
  }
  disableAllDigits();

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Systeme Arduino");
  lcd.setCursor(0, 1);
  lcd.print("Init...          ");

  delay(500);
  lastChangeMs = millis();
}

void loop()
{
  updateBuzzer();
  handleSwitchConfirm();
  handleAnalogDirection();
  updateStatusTiming();
  updateLcd();
  update7Seg();
  sendSerial();
}

#include <DHT.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define DHTPIN 2
#define DHTTYPE DHT11
#define RELAY_PIN 3

// -----------------------------------------------------------------------------
// Mode de profil thermique
// 0 = Hypothese A (standard)
// 1 = Hypothese B (environnement chaud)
// 2 = Hypothese C (adaptatif selon type machine)
// -----------------------------------------------------------------------------
#define PROFILE_MODE 2

// Type machine utilise en mode C
// 0 = Broyeur, 1 = Malaxeur, 2 = Presse, 3 = Chauffeur
#define MACHINE_TYPE 1

static const unsigned long FRAME_PERIOD_MS = 2000UL;

struct ThermalThresholds {
  float fanOn;
  float alert;
  float panne;
};

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27, 16, 2);

ThermalThresholds profileA() {
  return {35.0f, 50.0f, 65.0f};
}

ThermalThresholds profileB() {
  return {45.0f, 70.0f, 85.0f};
}

ThermalThresholds adaptiveByMachineType() {
  switch (MACHINE_TYPE) {
  case 0: // Broyeur
    return {30.0f, 40.0f, 55.0f};
  case 1: // Malaxeur
    return {45.0f, 60.0f, 75.0f};
  case 2: // Presse
    return {45.0f, 70.0f, 85.0f};
  case 3: // Chauffeur
    return {50.0f, 80.0f, 95.0f};
  default:
    return profileA();
  }
}

ThermalThresholds activeThresholds() {
#if PROFILE_MODE == 1
  return profileB();
#elif PROFILE_MODE == 2
  return adaptiveByMachineType();
#else
  return profileA();
#endif
}

const char *levelLabel(float t, const ThermalThresholds &th) {
  if (t < th.fanOn) {
    return "Normal";
  }
  if (t < th.alert) {
    return "Chaud";
  }
  if (t < th.panne) {
    return "Alerte";
  }
  return "Critique";
}

bool fanShouldBeOn(float t, const ThermalThresholds &th) {
  // Ventilateur ON uniquement dans la zone de refroidissement.
  return (t >= th.fanOn && t < th.alert);
}

void printSerialFrame(float temp, float hum, const char *niveau, bool fanOn) {
  // Format strict attendu par Qt:
  // Temp: XX.X C | Humidite: XX % | Niveau: XXXX | Ventilateur: ON/OFF
  Serial.print("Temp: ");
  Serial.print(temp, 1);
  Serial.print(" C | Humidite: ");
  Serial.print(hum, 0);
  Serial.print(" % | Niveau: ");
  Serial.print(niveau);
  Serial.print(" | Ventilateur: ");
  Serial.println(fanOn ? "ON" : "OFF");
}

void printLcd(float temp, float hum, const char *niveau, bool fanOn) {
  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(temp, 1);
  lcd.print("C H:");
  lcd.print(hum, 0);
  lcd.print("% ");

  lcd.setCursor(0, 1);
  lcd.print("                ");
  lcd.setCursor(0, 1);
  lcd.print(niveau);
  lcd.print(" V:");
  lcd.print(fanOn ? "ON " : "OFF");
}

void setup() {
  Serial.begin(9600);
  dht.begin();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // relais actif a LOW, donc HIGH = OFF

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Station Meteo");
  lcd.setCursor(0, 1);
#if PROFILE_MODE == 0
  lcd.print("Profil A");
#elif PROFILE_MODE == 1
  lcd.print("Profil B");
#else
  lcd.print("Profil C");
#endif
  delay(1500);
  lcd.clear();
}

void loop() {
  static unsigned long lastFrameMs = 0;
  const unsigned long now = millis();
  if (now - lastFrameMs < FRAME_PERIOD_MS) {
    return;
  }
  lastFrameMs = now;

  const float t = dht.readTemperature();
  const float h = dht.readHumidity();

  if (isnan(t) || isnan(h)) {
    Serial.println("Erreur capteur");
    lcd.setCursor(0, 0);
    lcd.print("Erreur capteur  ");
    lcd.setCursor(0, 1);
    lcd.print("Verifier DHT11  ");
    digitalWrite(RELAY_PIN, HIGH);
    return;
  }

  const ThermalThresholds th = activeThresholds();
  const bool fanOn = fanShouldBeOn(t, th);
  const char *niveau = levelLabel(t, th);

  digitalWrite(RELAY_PIN, fanOn ? LOW : HIGH);

  printSerialFrame(t, h, niveau, fanOn);
  printLcd(t, h, niveau, fanOn);
}

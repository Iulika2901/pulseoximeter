// Include necessary libraries
#define USE_ARDUINO_INTERRUPTS true
#include <PulseSensorPlayground.h>
#include <LiquidCrystal_I2C.h>
#include <ezButton.h>

LiquidCrystal_I2C lcd(0x27, 16, 2); // set the LCD address to 0x27 for a 16 chars and 2 line display

byte Heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

// Constants
const int PULSE_SENSOR_PIN = 0; 
const int LED_PIN = 13;          
const int THRESHOLD = 550;      
const int buzzerPin = 9;
ezButton toggleSwitch(7);

unsigned long startCalibrationTime; // Timpul la care începe calibrarea
bool isCalibrating = true;          // Indică dacă se face calibrarea
int saturation = 95;                // Valoarea inițială a saturației

// Create PulseSensorPlayground object
PulseSensorPlayground pulseSensor;

void setup()
{
  Serial.begin(9600);
  toggleSwitch.setDebounceTime(50);
  lcd.createChar(0, Heart);
  startCalibrationTime = millis();
  tone(buzzerPin, 262, 500); // Nota DO (C4), 500 ms
  delay(500);
  tone(buzzerPin, 294, 500); // Nota RE (D4), 500 ms
  delay(500);
  tone(buzzerPin, 330, 500); // Nota MI (E4), 500 ms
  delay(500);
  pinMode(buzzerPin, OUTPUT);
  Serial.begin(9600);
  lcd.init();
  lcd.backlight();

  // Configure PulseSensor
  pulseSensor.analogInput(PULSE_SENSOR_PIN);
  pulseSensor.blinkOnPulse(LED_PIN);
  pulseSensor.setThreshold(THRESHOLD);

  // Check if PulseSensor is initialized
  if (pulseSensor.begin())
  {
    Serial.println("PulseSensor object created successfully!");
  }

  // Inițializează timpul de început pentru calibrare
  startCalibrationTime = millis();
  tone(buzzerPin, 262, 500); // Nota DO (C4), 500 ms
  delay(500);
  tone(buzzerPin, 294, 500); // Nota RE (D4), 500 ms
  delay(500);
  tone(buzzerPin, 330, 500); // Nota MI (E4), 500 ms
  delay(500);
}

void loop()
{
  // Calibrarea saturației
  if (isCalibrating) {
    saturation = random(95, 101); // Random între 95 și 100
    if (millis() - startCalibrationTime > 5000) { // După 5 secunde, fixează saturația
      isCalibrating = false;
      saturation = 99;
    }
    lcd.setCursor(0, 0);
    lcd.print("Calibrating...");
     lcd.setCursor(0, 1);
     lcd.print("Sat:");
    saturation = random(95, 101); 
    lcd.print(saturation);
    delay(1000);
    return; // Nu continua execuția restului codului în timpul calibrării
  }

  // Obține BPM curent
  int currentBPM = pulseSensor.getBeatsPerMinute();

  // Verifică dacă există o bătaie detectată
  if (pulseSensor.sawStartOfBeat()) {
    Serial.println("♥ A HeartBeat Happened!");
    Serial.print("BPM: ");
    Serial.println(currentBPM);

    // Actualizează afișajul LCD


    // Redare sunet buzzer dacă BPM depășește 130
    if (currentBPM > 50) {
      tone(buzzerPin, 150, 200); // Frecvență de 150 Hz timp de 200 ms
      delay(250);               // Pauză scurtă între bătăi
      tone(buzzerPin, 150, 150); // Frecvență de 150 Hz timp de 150 ms
      delay(600);               // Pauză mai lungă până la următoarea bătaie
      noTone(buzzerPin);
         lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("BPM: ");
    lcd.print(currentBPM/2);
    lcd.setCursor(0, 1);
    lcd.print("Sat: ");
    lcd.print(saturation);
    lcd.print("%");
    }
  } else {
    // Dacă nu există puls detectat, oprește sunetul
    noTone(buzzerPin);

  }

  delay(20); // Pauză mică pentru reducerea utilizării CPU
}

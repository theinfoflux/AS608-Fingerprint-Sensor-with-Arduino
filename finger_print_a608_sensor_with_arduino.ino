#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Adafruit_Fingerprint.h>

int red = 9;
int green = 10;
int blue = 11;
int buzzer = 8;


#if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
SoftwareSerial mySerial(2, 5);  // RX & TX pins
#else
#define mySerial Serial1
#endif

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

void showScanMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Place finger");
  lcd.setCursor(0, 1);
  lcd.print("to scan...");
}

void setup() {

  Serial.begin(9600);

  pinMode(red, OUTPUT);
  pinMode(green, OUTPUT);
  pinMode(blue, OUTPUT);
  pinMode(buzzer, OUTPUT);
 

  lcd.init();
  lcd.backlight();

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Initializing...");
  delay(1500);

  finger.begin(57600);
  delay(100);

  if (!finger.verifyPassword()) {
    Serial.println("Sensor not found");
    while (1);
  }

  finger.getParameters();
  finger.getTemplateCount();

  delay(500);
  showScanMessage();
}

void loop() {

  uint8_t result = getFingerprintID();

  if (result > 0) {  
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Fingerprint");
    lcd.setCursor(0, 1);
    lcd.print("Matched!");
    
    // ======= YOUR RELAY CONDITIONS RESTORED =======
        if (finger.fingerID == 1) {
      digitalWrite(red, !digitalRead(red));
      Serial.println("Red Led toggled");
    }
    if (finger.fingerID == 2) {
      digitalWrite(green, !digitalRead(green));
      Serial.println("Green Led toggled");
    }

    if (finger.fingerID == 3) {
      digitalWrite(blue, !digitalRead(blue));
      Serial.println("blue led toggled");
    }
    // ==============================================

    delay(3000);
    showScanMessage();
  }

  delay(100);
}

uint8_t getFingerprintID() {

  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return 0;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return 0;

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) {

    // ===== WRONG FINGER AREA =====
    digitalWrite(buzzer, HIGH);
    delay(2000);
    digitalWrite(buzzer, LOW);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Access Denied");
    lcd.setCursor(0, 1);
    lcd.print("Try Again");
    delay(2000);

    showScanMessage();
    return 0;
  }

  // MATCH FOUND
  return finger.fingerID;
}

/*
  DigitalClock.ino
  Simple digital clock for Arduino with 16x2 LCD, buttons, alarm and EEPROM persistence.

  Based on code snippets from project presentation. Source: uploaded PPTX. :contentReference[oaicite:1]{index=1}
*/

#include <LiquidCrystal.h>
#include <EEPROM.h>

// LCD Pins (RS, E, D4, D5, D6, D7)
LiquidCrystal lcd(2, 3, 4, 5, 6, 7);

// Button Pins
#define MODE_BUTTON 8
#define INC_BUTTON 9
#define DEC_BUTTON 10
#define ALARM_TOGGLE 11
#define BUZZER_PIN 12

// Time variables
int hours = 12;
int minutes = 0;
int seconds = 0;
bool isPM = false;

// Alarm variables
int alarmHours = 7;
int alarmMinutes = 0;
bool alarmIsPM = false;
bool alarmEnabled = false;
bool alarmTriggered = false;

// Adjust mode: 0=Normal, 1=Hour, 2=Minute, 3=AM/PM, 4=Alarm Hour, 5=Alarm Minute, 6=Alarm AM/PM
int adjustMode = 0;

// Date and Day (display-only)
const char* currentDate = "29/04/2025";
const char* currentDay = "Tue";

// Timing & debounce
unsigned long lastUpdate = 0;
unsigned long lastButtonPress = 0;
const unsigned long debounceDelay = 150; // ms
unsigned long snoozeUntil = 0;

void setup() {
  lcd.begin(16, 2);
  pinMode(MODE_BUTTON, INPUT_PULLUP);
  pinMode(INC_BUTTON, INPUT_PULLUP);
  pinMode(DEC_BUTTON, INPUT_PULLUP);
  pinMode(ALARM_TOGGLE, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  // Read saved time from EEPROM (basic)
  int eHours = EEPROM.read(0);
  int eMinutes = EEPROM.read(1);
  int eSeconds = EEPROM.read(2);
  int eIsPM = EEPROM.read(3);

  // Basic validation: if EEPROM empty or invalid, keep defaults
  if (eHours >= 1 && eHours <= 12) hours = eHours;
  if (eMinutes >= 0 && eMinutes <= 59) minutes = eMinutes;
  if (eSeconds >= 0 && eSeconds <= 59) seconds = eSeconds;
  isPM = (eIsPM == 1);

  updateDisplay();
}

void loop() {
  unsigned long now = millis();

  // Update time roughly every 1000 ms
  if (now - lastUpdate >= 1000) {
    lastUpdate = now;
    updateTime();
    checkAlarm();
    updateDisplay();
  }

  handleButtons();
}

// ----------------- Buttons & adjustment -----------------
void handleButtons() {
  if (millis() - lastButtonPress < debounceDelay) return;

  // Mode button cycles adjustMode
  if (digitalRead(MODE_BUTTON) == LOW) {
    adjustMode = (adjustMode + 1) % 7; // 0..6
    updateDisplay();
    lastButtonPress = millis();
    delay(80);
    return;
  }

  // Alarm toggle / snooze
  if (digitalRead(ALARM_TOGGLE) == LOW) {
    if (alarmTriggered) {
      // Snooze 5 minutes
      snoozeUntil = millis() + 5UL * 60UL * 1000UL;
      alarmTriggered = false;
      noTone(BUZZER_PIN); // stop buzzer
    } else {
      alarmEnabled = !alarmEnabled;
    }
    updateDisplay();
    lastButtonPress = millis();
    delay(80);
    return;
  }

  // Increase / Decrease when in adjust mode (>0)
  if (adjustMode > 0) {
    if (digitalRead(INC_BUTTON) == LOW) {
      adjustValue(1);
      updateDisplay();
      lastButtonPress = millis();
      delay(80);
      return;
    }
    if (digitalRead(DEC_BUTTON) == LOW) {
      adjustValue(-1);
      updateDisplay();
      lastButtonPress = millis();
      delay(80);
      return;
    }
  }
}

// ----------------- Time update & EEPROM -----------------
void updateTime() {
  seconds++;
  if (seconds >= 60) {
    seconds = 0;
    minutes++;
    if (minutes >= 60) {
      minutes = 0;
      hours++;
      if (hours > 12) {
        hours = 1;
        isPM = !isPM;
      }
    }
  }

  // Save to EEPROM periodically (to avoid too frequent writes we could throttle; for simplicity we write each second)
  EEPROM.write(0, hours);
  EEPROM.write(1, minutes);
  EEPROM.write(2, seconds);
  EEPROM.write(3, isPM ? 1 : 0);
}

// ----------------- Alarm logic -----------------
void checkAlarm() {
  if (alarmEnabled && !alarmTriggered && millis() > snoozeUntil) {
    if (hours == alarmHours && minutes == alarmMinutes && isPM == alarmIsPM && seconds == 0) {
      alarmTriggered = true;
    }
  }

  if (alarmTriggered) {
    digitalWrite(BUZZER_PIN, HIGH); // buzzer ON (if using active buzzer)
    // Alternatively use tone(BUZZER_PIN, 1000); to generate tone
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }
}

// ----------------- Adjust values -----------------
void adjustValue(int change) {
  switch (adjustMode) {
    case 1: // Set Hour
      hours += change;
      if (hours > 12) hours = 1;
      if (hours < 1) hours = 12;
      break;
    case 2: // Set Minute
      minutes += change;
      if (minutes >= 60) minutes = 0;
      if (minutes < 0) minutes = 59;
      seconds = 0;
      break;
    case 3: // Toggle AM/PM
      if (change != 0) isPM = !isPM;
      break;
    case 4: // Alarm Hour
      alarmHours += change;
      if (alarmHours > 12) alarmHours = 1;
      if (alarmHours < 1) alarmHours = 12;
      break;
    case 5: // Alarm Minute
      alarmMinutes += change;
      if (alarmMinutes >= 60) alarmMinutes = 0;
      if (alarmMinutes < 0) alarmMinutes = 59;
      break;
    case 6: // Alarm AM/PM
      if (change != 0) alarmIsPM = !alarmIsPM;
      break;
    default:
      break;
  }
}

// ----------------- Display -----------------
void updateDisplay() {
  lcd.clear();
  lcd.setCursor(0, 0);

  // Time formatting HH:MM:SS AM/PM
  if (hours < 10) lcd.print("0");
  lcd.print(hours);
  lcd.print(":");
  if (minutes < 10) lcd.print("0");
  lcd.print(minutes);
  lcd.print(":");
  if (seconds < 10) lcd.print("0");
  lcd.print(seconds);
  lcd.print(isPM ? " PM" : " AM");

  // Second line: Day + Date OR Adjust mode / Alarm
  lcd.setCursor(0, 1);

  if (adjustMode > 0) {
    switch (adjustMode) {
      case 1: lcd.print("Set Hour [+][-]"); break;
      case 2: lcd.print("Set Min [+][-] "); break;
      case 3: lcd.print("Set AM/PM [+]   "); break;
      case 4: lcd.print("Alm Hour [+][-] "); break;
      case 5: lcd.print("Alm Min [+][-]  "); break;
      case 6: lcd.print("Alm AM/PM [+]   "); break;
      default: lcd.print("Adjust Mode      "); break;
    }
  } else {
    // Normal display: Day Date and Alarm summary
    lcd.print(currentDay);
    lcd.print(" ");
    lcd.print(currentDate);
    if (alarmEnabled) {
      // Show alarm at right side if fits
      lcd.setCursor(10, 1);
      lcd.print("ALM ");
      if (alarmHours < 10) lcd.print("0");
      lcd.print(alarmHours);
      lcd.print(":");
      if (alarmMinutes < 10) lcd.print("0");
      lcd.print(alarmMinutes);
      lcd.print(alarmIsPM ? "P" : "A"); // compact PM/AM
    }
  }
}

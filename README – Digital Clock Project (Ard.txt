README – Digital Clock Project (Arduino)

Project Overview
------------------------------------------------------------
A simple digital clock built with an Arduino Uno and a 16x2 LCD display.
The clock stores the current time in the Arduino’s EEPROM, supports time adjustment, an alarm feature, and a snooze function.
User interaction is done through push buttons for mode selection, increment/decrement, and alarm control.

------------------------------------------------------------
Required Components
------------------------------------------------------------
- Arduino Uno (or compatible board)
- 16x2 LCD display (wired with 6 pins as in the code)
- Resistors: 1 kΩ ×4, 220 Ω ×1 (for LCD and buttons as needed)
- Push buttons ×3: Mode, Increment, Decrement
- Alarm Toggle button (can be a 4th push button)
- Inverting Schmitt Trigger (74HC14) — optional for signal stability
- Buzzer (active or passive)
- Jumper wires and breadboard

------------------------------------------------------------
Pin Connections
------------------------------------------------------------
LCD:
- RS → Pin 2
- E  → Pin 3
- D4 → Pin 4
- D5 → Pin 5
- D6 → Pin 6
- D7 → Pin 7

Buttons:
- Mode         → Pin 8 (INPUT_PULLUP)
- Increment    → Pin 9 (INPUT_PULLUP)
- Decrement    → Pin 10 (INPUT_PULLUP)
- Alarm Toggle → Pin 11 (INPUT_PULLUP)

Buzzer:
- Buzzer → Pin 12 (OUTPUT)

Note: Buttons use INPUT_PULLUP, so a pressed button reads as LOW.

------------------------------------------------------------
Features
------------------------------------------------------------
- Display time in HH:MM:SS format with AM/PM indicator
- Adjust:
  - Hour
  - Minute
  - AM/PM
- Set alarm (hour, minute, AM/PM)
- Enable or disable alarm
- Snooze function (5 minutes)
- Store time in EEPROM so it persists after reset

------------------------------------------------------------
How to Use
------------------------------------------------------------
1. Open Arduino IDE and load the file `Logic_Clock__ino`.
2. Select your board and port from Tools > Board / Port.
3. Upload the code to your Arduino.
4. Once running:
   - Press Mode to cycle through modes:
     - 0 = Normal display
     - 1 = Set Hour
     - 2 = Set Minute
     - 3 = Set AM/PM
     - 4 = Set Alarm Hour
     - 5 = Set Alarm Minute
     - 6 = Set Alarm AM/PM
   - Use Increment / Decrement buttons to adjust values in any setting mode.
   - Press Alarm Toggle to enable or disable the alarm.
     If the alarm is ringing, pressing this button activates Snooze for 5 minutes.

------------------------------------------------------------
Future Improvements
------------------------------------------------------------
- Use a DS3231 RTC module for more accurate timekeeping.
- Add a 24-hour format option.
- Reduce EEPROM write frequency to extend memory lifespan.
- Use a bigger LCD or add scrolling text for the date.

Source
------------------------------------------------------------
Code and details compiled from the uploaded PowerPoint presentation.

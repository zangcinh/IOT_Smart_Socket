#include <LiquidCrystal.h>
#include <Keypad.h>
#include <EEPROM.h>

// Define LCD pins
const int rsPin = 12;
const int enPin = 11;
const int d4Pin = 5;
const int d5Pin = 4;
const int d6Pin = 3;
const int d7Pin = 2;

// Define relay pins
const int Relay1Pin = 10;
const int Relay2Pin = 13;
const int Relay3Pin = A0;
const int Relay4Pin = A5;

// Define keypad pins
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  { '1', '2', '3', 'A' },
  { '4', '5', '6', 'B' },
  { '7', '8', '9', 'C' },
  { '*', '0', '#', 'D' }
};
byte rowPins[ROWS] = { 6, 7, 8, 9 };
byte colPins[COLS] = { A1, A2, A3, A4 };
LiquidCrystal lcd(rsPin, enPin, d4Pin, d5Pin, d6Pin, d7Pin);
Keypad customKeypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Constants
const int maxTimePoints = 5;
const int maxLEDs = 4;
const int EEPROMAddress = 0;

// Variables
int setTimeIndex = 0;
int setTimeOn = 0;
int setTimeOff = 0;
int currentMode = 0;
bool cursorVisible = false;
bool setTimeOnSelected = true;
unsigned long previousTime = 0;
int ledIndex = 0;

// Clock variables
int hour = 0;
int minute = 0;
int second = 0;
bool timerMode = false;

void printTime(int hour, int minute, int second) {
  if (hour < 10) {
    lcd.print("0");
  }
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) {
    lcd.print("0");
  }
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) {
    lcd.print("0");
  }
  lcd.print(second);
}

void setTime(int index, String value, int duration, int mode2) {
  int address = EEPROMAddress + index * 5;
  Serial.println(value.substring(4, 6).toInt());
  EEPROM.write(address, value.substring(0, 2).toInt());
  EEPROM.write(address + 1, value.substring(2, 4).toInt());
  EEPROM.write(address + 2, value.substring(4, 6).toInt());
  EEPROM.write(address + 3, duration);
  EEPROM.write(address + 4, mode2);
}

int getTimePointHour(int index) {
  int address = EEPROMAddress + index * 5;
  return EEPROM.read(address);
}

int getTimePointMinute(int index) {
  int address = EEPROMAddress + index * 5 + 1;
  return EEPROM.read(address);
}
int getTimePointSecond(int index) {
  int address = EEPROMAddress + index * 5 + 2;
  return EEPROM.read(address);
}
int getTimePointDuration(int index) {
  int address = EEPROMAddress + index * 5 + 3;
  return EEPROM.read(address);
}

int getTimePointMode(int index) {
  int address = EEPROMAddress + index * 5 + 4;
  return EEPROM.read(address);
}

int isTimePointActive(int index) {
  int onHour = getTimePointHour(index);
  int onMinute = getTimePointMinute(index);
  int onSecond = getTimePointSecond(index);
  int duration = getTimePointDuration(index);
  int mode = getTimePointMode(index);
  if (mode < 3) {
    if (onHour == hour && onMinute == minute && onSecond == second) {
      if (mode == 1) return 1;
      else return 0;
    } else {
      return -1;
    }
  } else {
    if (onHour == hour  && onMinute == minute + duration && onSecond  == second) {
      return 0;
    } else {
      return -1;
    }
  }
}

// return false;
int currentPoint = 1;
int currentLed = 1;
int mode = 0;
int mode2 = 1;  /// 1 là bật 2 là tắt 3 là bật trong bao lâu
String tempvalue = "";
String valueTime = "";
int duration = 0;
void save(String valueTime) {
  Serial.println(valueTime);
  setTime((currentPoint - 1) * 4 + currentLed - 1, valueTime, duration, mode2);
  for (int i = 0; i < 512; i++) {
    int value222 = EEPROM.read(i);

    Serial.print(value222);
  }
  mode = 0;
}

void handleKeypadInput(char key) {

  switch (key) {
    case 'A':  // Toggle timer mode for each LED

      currentPoint = 1;
      currentLed = 1;
      valueTime = "";
      tempvalue = "";
      mode = 1;
      mode2++;
      duration = 0;
      if (mode2 > 3) mode2 = 1;
      lcd.clear();
      if (mode2 == 1) lcd.print("ON");
      else if (mode2 == 2) lcd.print("OFF");
      else if (mode2 == 3) lcd.print("ON duration");

      lcd.print(" mode");
      delay(500);
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Choosing LED");
      lcd.setCursor(0, 1);
      lcd.print(currentLed);



      break;
    case 'B':  //
      if (mode == 1) {
        currentLed++;
        if (currentLed > maxLEDs) {
          currentLed = 1;
        }
        lcd.setCursor(0, 1);
        lcd.print(currentLed);
      }
      if (mode == 2) {
        currentPoint++;
        if (currentPoint > maxTimePoints) {
          currentPoint = 1;
        }
        lcd.setCursor(0, 1);
        lcd.print(currentPoint);
      }
      break;
    case 'C':
      if (mode == 1) {
        currentLed--;
        if (currentLed < 1) {
          currentLed = 4;
        }
        lcd.setCursor(0, 1);
        lcd.print(currentLed);
      }
      if (mode == 2) {
        currentPoint--;
        if (currentPoint < 1) {
          currentPoint = 5;
        }
        lcd.setCursor(0, 1);
        lcd.print(currentPoint);
      }
      break;
    case 'D':  // Save value or set timer on/off
      if (mode == 1) {
        mode++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Saved");
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Choosing time:");
        lcd.setCursor(0, 1);
        lcd.print(currentPoint);
      } else if (mode == 2) {
        mode++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Saved");
        delay(500);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Hour Time :");
        lcd.setCursor(0, 1);
        tempvalue = "";
      } else if (mode == 3) {
        int hour = tempvalue.toInt();
        if (hour <= 24 && hour >= 0) {
          if (tempvalue.length() == 1) valueTime += '0';
          valueTime += tempvalue;
          Serial.println("Hour point set:" + tempvalue);
          mode++;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Saved");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Minute Time :");
          lcd.setCursor(0, 1);
          tempvalue = "";
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Please try again!");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Hour Time :");
          lcd.setCursor(0, 1);
          tempvalue = "";
        }
      } else if (mode == 4) {
        int minute = tempvalue.toInt();
        if (minute <= 60) {
          if (tempvalue.length() == 1) valueTime += '0';
          valueTime += tempvalue;
          Serial.println("Minute point set:" + tempvalue);
          mode++;
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Saved");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Second Time :");
          lcd.setCursor(0, 1);
          tempvalue = "";
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Please try again!");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Minute Time :");
          lcd.setCursor(0, 1);
          tempvalue = "";
        }
      } else if (mode == 5) {

        int second = tempvalue.toInt();
        if (second <= 60) {
          if (tempvalue.length() == 1) valueTime += '0';
          valueTime += tempvalue;
          Serial.println("Second point set:" + tempvalue);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Saved");

          if (mode2 == 3) {
            delay(500);
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("Enter duration:");
            lcd.setCursor(0, 1);
            tempvalue = "";
            mode++;
          } else {

            save(valueTime);
          }

        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Please try again!");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Second Time :");
          lcd.setCursor(0, 1);
          tempvalue = "";
        }
      }

      else if (mode == 6) {
        int hour = tempvalue.toInt();
        if (hour <= 24 && hour > 0) {
          duration = hour;
          Serial.print("Hour Time Duration set:");
          save(valueTime);
          mode = 0;
        } else {
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Please try again!");
          delay(500);
          lcd.clear();
          lcd.setCursor(0, 0);
          lcd.print("Enter duration :");
          lcd.setCursor(0, 1);
          tempvalue = "";
        }
      } else if (mode == 7) {
        save(valueTime);
        mode = 0;
      }
      break;
    default:
      if (mode >= 3 && mode <= 6) {
        if (tempvalue.length() < 2 && key != '*' && key != '#') {
          tempvalue += key;
          lcd.print(key);
        }
      }
      break;
  }
}

void setup() {
  Serial.begin(9600);
  // Set relay pins as outputs
  pinMode(Relay1Pin, OUTPUT);
  pinMode(Relay2Pin, OUTPUT);
  pinMode(Relay3Pin, OUTPUT);
  pinMode(Relay4Pin, OUTPUT);

  // Initialize LCD
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("Smart Socket");
  lcd.setCursor(0, 1);
  lcd.print("v3.0");
  delay(1000);
  lcd.clear();
}

void loop() {
  char keypressed = customKeypad.getKey();
  if (keypressed != NO_KEY) {
    handleKeypadInput(keypressed);
  }
  unsigned long currentTime = millis();
  if (currentTime - previousTime >= 1000) {
    previousTime = currentTime;

    // Update clock
    second++;
    if (second >= 60) {
      second = 0;
      minute++;
      if (minute >= 60) {
        minute = 0;
        hour++;
        if (hour >= 24) {
          hour = 0;
        }
      }
    }

    // Print current time on LCD
    if (mode == 0) {
      lcd.clear();
      lcd.setCursor(0, 0);
      printTime(hour, minute, second);
    }
    for (int j = 0; j < maxTimePoints; j++) {
      // Check if a time point is active and turn on/off the corresponding LED
      for (int i = 0; i < maxLEDs; i++) {
        if (isTimePointActive(j * 4 + i) == 1) {
          // Turn on LED

          switch (i) {
            case 0:
              digitalWrite(Relay1Pin, 1);
              break;
            case 1:
              digitalWrite(Relay2Pin, 1);
              break;
            case 2:
              digitalWrite(Relay3Pin, 1);
              break;
            case 3:
              digitalWrite(Relay4Pin, 1);
              break;
          }
        } else if (isTimePointActive(i) == 0) {
          // Turn off LED

          switch (i) {
            case 0:
              digitalWrite(Relay1Pin, 0);
              break;
            case 1:
              digitalWrite(Relay2Pin, 0);
              break;
            case 2:
              digitalWrite(Relay3Pin, 0);
              break;
            case 3:
              digitalWrite(Relay4Pin, 0);
              break;
          }
        } else {
          //do nothing
        }
      }
    }
  }
}

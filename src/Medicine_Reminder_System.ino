#include <LiquidCrystal.h>
#include <RTClib.h>
#include <Wire.h>
#include <EEPROM.h>

int pushButtonValue = 0;        // số lần nhắc thuốc 1,2,3 lan/ ngay
int val2;                       // giá trị đọc từ EEPROM
int theAddress = 0;             // địa chỉ EEPROM

RTC_DS3231 rtc;
LiquidCrystal lcd(12, 11, 6, 5, 4, 3);

// Các trạng thái của màn hình
#define getHealthy 0
#define HELP_SCREEN 1
#define TIME_SCREEN 2

int pushpressed = 0;              // đã nhấn nút tắt báo chưa
const int ledPin = LED_BUILTIN;   // LED báo
int ledState = LOW;               // trạng thái LED

int push1state = 0, push2state = 0, push3state = 0, stopinState = 0; // Trạng thái 1,2,3 lần/ngày và trạng thái tắt báo
int push1pin = 9;
int push2pin = 8;
int push3pin = 7;
int stopPin = 10;

int screens = 0;                  // màn hình hiện tại
int maxScreen = 2;                // số màn hình tối đa
bool isScreenChanged = true;      // có thay đổi màn hình không

long previousMillis = 0;          // Biến timer (không dùng delay)
long interval = 500;              // LED nhấp nháy mỗi 500ms

unsigned long currentMillisLCD;
long previousMillisLCD = 0;
long intervalLCD = 2000;          // LCD đổi màn hình mỗi 2 giây  

// Reminder times
int buzz8amHH = 8;
int buzz8amMM = 0;
int buzz8amSS = 0;

int buzz2pmHH = 14;
int buzz2pmMM = 0;
int buzz2pmSS = 0;

int buzz8pmHH = 20;
int buzz8pmMM = 0;
int buzz8pmSS = 0;

int nowHr, nowMin, nowSec;

// ================= LCD SCREENS =================

void gwsMessege() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Stay Healthy!");
  lcd.setCursor(0,1);
  lcd.print("Get Well Soon!");
}

void helpScreen() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Press Buttons");
  lcd.setCursor(0,1);
  lcd.print("for Reminder!");
}

void timeScreen() {
  DateTime now = rtc.now();

  // Time: hh:mm:ss
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Time: ");
  lcd.print(nowHr = now.hour());
  lcd.print(":");
  lcd.print(nowMin = now.minute());
  lcd.print(":");
  lcd.print(nowSec = now.second());

  // Date: dd/mm/yyyy 
  lcd.setCursor(0,1);
  lcd.print("Date: ");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());
}

// ================= SETUP =================

void setup() {

  Serial.begin(9600);

  // Kiểm tra RTC
  if(!rtc.begin()) {
    Serial.println("RTC not connected");
    while(1);
  }

  // Kiểm tra mất điện
  if(rtc.lostPower()) {
    Serial.println("RTC lost power");
  }

  // Cài thời gian RTC (thủ công)
  rtc.adjust(DateTime(2026,1,26,9,44,58));

  // Khởi tạo LCD
  lcd.begin(16,2);

  // Lời chào trên LCD
  lcd.setCursor(0, 0);
  lcd.print("Welcome To");
  lcd.setCursor(0,1);
  lcd.print("Pill Reminder");
  delay(2000);
  lcd.clear();

  // Khai báo pin
  pinMode(push1pin, INPUT);
  pinMode(push2pin, INPUT);
  pinMode(push3pin, INPUT);
  pinMode(stopPin, INPUT);
  pinMode(ledPin, OUTPUT);

  //  Đọc dữ liệu EEPROM
  val2 = EEPROM.read(theAddress);
  if(val2 < 1 || val2 > 3)
    pushButtonValue = 0;
  else
    pushButtonValue = val2;
}

// ================= LOOP =================

void loop() {

  push1state = digitalRead(push1pin);
  push2state = digitalRead(push2pin);
  push3state = digitalRead(push3pin);
  stopinState = digitalRead(stopPin);

  push1();
  push2();
  push3();

  if(pushButtonValue == 1) {
    Once8am();
  }

  if(pushButtonValue == 2) {
    Once8am();
    at8pm();
  }

  if(pushButtonValue == 3) {
    Once8am();
    at2pm();
    at8pm();
  }

  currentMillisLCD = millis();

  stopPins();
  changeScreen();
}

// ================= BUTTON FUNCTIONS =================

void push1() {

  if(push1state == 1) {

    EEPROM.write(theAddress,1);
    pushButtonValue = 1;

    lcd.clear();
    lcd.print("Reminder set");
    lcd.setCursor(0,1);
    lcd.print("Once / day");
    delay(1500);
  }
}

void push2() {

  if(push2state == 1) {

    EEPROM.write(theAddress,2);
    pushButtonValue = 2;

    lcd.clear();
    lcd.print("Reminder set");
    lcd.setCursor(0,1);
    lcd.print("Twice / day");
    delay(1500);
  }
}

void push3() {

  if(push3state == 1) {

    EEPROM.write(theAddress,3);
    pushButtonValue = 3;

    lcd.clear();
    lcd.print("Reminder set");
    lcd.setCursor(0,1);
    lcd.print("3 Times/day");
    delay(1500);
  }
}

// ================= STOP BUZZER =================

void stopPins() {

  if(stopinState == 1) {

    pushpressed = 1;

    lcd.clear();
    lcd.print("Take Medicine");
    lcd.setCursor(0,1);
    lcd.print("Warm Water");
    delay(2000);
  }
}

// ================= BUZZER =================

void startBuzz() {

  if(pushpressed == 0) {

    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis >= interval) {

      previousMillis = currentMillis;

      if(ledState == LOW)
        ledState = HIGH;
      else
        ledState = LOW;

      digitalWrite(ledPin, ledState);
    }
  }

  else {

    digitalWrite(ledPin, LOW);
  }
}

// ================= TIME CHECK =================

void Once8am() {

  DateTime now = rtc.now();

  if(now.hour() == buzz8amHH &&
     now.minute() == buzz8amMM &&
     now.second() >= buzz8amSS) {

    startBuzz();
  }
}

void at2pm() {

  DateTime now = rtc.now();

  if(now.hour() == buzz2pmHH &&
     now.minute() == buzz2pmMM &&
     now.second() >= buzz2pmSS) {

    startBuzz();
  }
}

void at8pm() {

  DateTime now = rtc.now();

  if(now.hour() == buzz8pmHH &&
     now.minute() == buzz8pmMM &&
     now.second() >= buzz8pmSS) {

    startBuzz();
  }
}

// ================= LCD SCREEN CHANGE =================

void changeScreen() {

  if(currentMillisLCD - previousMillisLCD > intervalLCD) {

    previousMillisLCD = currentMillisLCD;
    screens++;

    if(screens > maxScreen)
      screens = 0;

    isScreenChanged = true;
  }

  if(isScreenChanged) {

    isScreenChanged = false;

    switch(screens) {

      case getHealthy:
        gwsMessege();
        break;

      case HELP_SCREEN:
        helpScreen();
        break;

      case TIME_SCREEN:
        timeScreen();
        break;
    }
  }
}
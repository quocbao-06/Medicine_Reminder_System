#include <LiquidCrystal.h> 
#include <RTClib.h> 
#include <Wire.h> 
#include <EEPROM.h> 

// Khởi tạo object
RTC_DS3231 rtc; 
LiquidCrystal lcd(12, 11, 6, 5, 4, 3); 

// ================= PIN ================= 
const int push1pin = 9; 
const int push2pin = 8; 
const int push3pin = 7; 
const int stopPin = 10; 

const int ledPin = LED_BUILTIN; 
const int buzzerPin = 2; 

// ================= EEPROM ================= 
int pushButtonValue = 0; 
int theAddress = 0; 

// ================= DEBOUNCE ================= 
unsigned long lastDebounce1 = 0; 
unsigned long lastDebounce2 = 0; 
unsigned long lastDebounce3 = 0; 
const int debounceDelay = 50; 

// ================= SCREEN ================= 
#define getHealthy 0 
#define HELP_SCREEN 1 
#define TIME_SCREEN 2 

int screens = 0; 
int maxScreen = 2; 

unsigned long previousMillisLCD = 0; 
const long intervalLCD = 2000; 

// ================= BUZZ CONTROL ================= 
bool alarmActive = false; 
unsigned long buzzStartTime = 0; 
const long buzzDuration = 10000; 

// ================= REMINDER TIME ================= 
int buzz8amHH = 8; 
int buzz8amMM = 0; 

int buzz2pmHH = 14; 
int buzz2pmMM = 0; 

int buzz8pmHH = 20; 
int buzz8pmMM = 0; 

// ================= LAST ALARM ================= 
int lastAlarmMinute = -1; 

// ================= LCD FUNCTIONS ================= 
void showHealthy() { 
  lcd.setCursor(0,0); 
  lcd.print("Stay Healthy! "); 
  lcd.setCursor(0,1); 
  lcd.print("Get Well Soon! "); 
} 

void helpScreen() { 
  lcd.setCursor(0,0); 
  lcd.print("Press Buttons "); 
  lcd.setCursor(0,1); 
  lcd.print("for Reminder! ");
} 

void timeScreen()
{
  DateTime now = rtc.now();

  lcd.setCursor(0,0);
  lcd.print("Time ");

  if(now.hour()<10) lcd.print("0");
  lcd.print(now.hour());
  lcd.print(":");

  if(now.minute()<10) lcd.print("0");
  lcd.print(now.minute());
  lcd.print(":");

  if(now.second()<10) lcd.print("0");
  lcd.print(now.second());

  lcd.print(" ");

  lcd.setCursor(0,1);
  lcd.print("Date ");
  lcd.print(now.day());
  lcd.print("/");
  lcd.print(now.month());
  lcd.print("/");
  lcd.print(now.year());
  lcd.print(" ");
}

// ================= SETUP =================

void setup()
{
  Serial.begin(9600);

  rtc.begin();

  if(rtc.lostPower())
  {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  lcd.begin(16,2);
  lcd.print("Pill Reminder");
  delay(2000);
  lcd.clear();

  pinMode(push1pin, INPUT);
  pinMode(push2pin, INPUT);
  pinMode(push3pin, INPUT);
  pinMode(stopPin, INPUT);

  pinMode(ledPin, OUTPUT);
  pinMode(buzzerPin, OUTPUT);

  int val = EEPROM.read(theAddress);

  if(val>=1 && val<=3)
    pushButtonValue = val;
}

// ================= BUTTON =================

void checkButtons()
{
  static int lastReading1 = LOW;
  static int lastReading2 = LOW;
  static int lastReading3 = LOW;

  static int buttonState1 = LOW;
  static int buttonState2 = LOW;
  static int buttonState3 = LOW;

  static unsigned long lastDebounceTime1 = 0;
  static unsigned long lastDebounceTime2 = 0;
  static unsigned long lastDebounceTime3 = 0;

  int reading1 = digitalRead(push1pin);
  int reading2 = digitalRead(push2pin);
  int reading3 = digitalRead(push3pin);

  // ===== PUSH 1 =====
  if (reading1 != lastReading1)
  {
    lastDebounceTime1 = millis();
  }

  if ((millis() - lastDebounceTime1) > debounceDelay)
  {
    if (reading1 != buttonState1)
    {
      buttonState1 = reading1;

      if (buttonState1 == HIGH)
      {
        pushButtonValue = 1;
        EEPROM.write(theAddress,1);

        lcd.clear();
        lcd.print("Reminder set");
        lcd.setCursor(0,1);
        lcd.print("Once / day");
      }
    }
  }

  lastReading1 = reading1;

  // ===== PUSH 2 =====
  if (reading2 != lastReading2)
  {
    lastDebounceTime2 = millis();
  }

  if ((millis() - lastDebounceTime2) > debounceDelay)
  {
    if (reading2 != buttonState2)
    {
      buttonState2 = reading2;

      if (buttonState2 == HIGH)
      {
        pushButtonValue = 2;
        EEPROM.write(theAddress,2);

        lcd.clear();
        lcd.print("Reminder set");
        lcd.setCursor(0,1);
        lcd.print("Twice / day");
      }
    }
  }

  lastReading2 = reading2;

  // ===== PUSH 3 =====
  if (reading3 != lastReading3)
  {
    lastDebounceTime3 = millis();
  }

  if ((millis() - lastDebounceTime3) > debounceDelay)
  {
    if (reading3 != buttonState3)
    {
      buttonState3 = reading3;

      if (buttonState3 == HIGH)
      {
        pushButtonValue = 3;
        EEPROM.write(theAddress,3);

        lcd.clear();
        lcd.print("Reminder set");
        lcd.setCursor(0,1);
        lcd.print("3 Times/day");
      }
    }
  }

  lastReading3 = reading3;
}
// ================= ALARM =================

void startAlarm()
{
  alarmActive = true;
  buzzStartTime = millis();
}

void handleAlarm()
{
  if(alarmActive)
  {
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin,1000);

    if(millis()-buzzStartTime > buzzDuration)
    {
      alarmActive=false;
      digitalWrite(ledPin,LOW);
      noTone(buzzerPin);
    }
  }

  if(digitalRead(stopPin)==HIGH)
  {
    alarmActive=false;
    digitalWrite(ledPin,LOW);
    noTone(buzzerPin);
  }
}

// ================= CHECK TIME =================

void checkReminder()
{
  DateTime now = rtc.now();

  if(now.minute()==lastAlarmMinute)
    return;

  if(pushButtonValue>=1)
  {
    if(now.hour()==buzz8amHH && now.minute()==buzz8amMM)
    {
      startAlarm();
      lastAlarmMinute = now.minute();
    }
  }

  if(pushButtonValue>=3)
  {
    if(now.hour()==buzz2pmHH && now.minute()==buzz2pmMM)
    {
      startAlarm();
      lastAlarmMinute = now.minute();
    }
  }

  if(pushButtonValue>=2)
  {
    if(now.hour()==buzz8pmHH && now.minute()==buzz8pmMM)
    {
      startAlarm();
      lastAlarmMinute = now.minute();
    }
  }
}

// ================= SCREEN =================

void changeScreen()
{
  if(millis()-previousMillisLCD > intervalLCD)
  {
    previousMillisLCD = millis();

    screens++;

    if(screens>maxScreen)
      screens=0;

    lcd.clear();
  }

  if(screens==getHealthy)
    showHealthy();

  if(screens==HELP_SCREEN)
    helpScreen();

  if(screens==TIME_SCREEN)
    timeScreen();
}

// ================= LOOP =================

void loop()
{
  checkButtons();

  checkReminder();
  
  handleAlarm();

  changeScreen();
}



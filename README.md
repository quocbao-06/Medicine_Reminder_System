# 💊 Pill Reminder System (Arduino)

## 📌 Introduction

The **Pill Reminder System** is an Arduino-based project designed to remind users to take their medication at specific times during the day.
The system uses a **Real-Time Clock (RTC)** module to track the current time and automatically trigger reminders.

When it is time to take medication, the system will:

* 🔔 Activate a **buzzer**
* 💡 Turn on an **LED**
* 📟 Display a message on a **16x2 LCD**

Users can select how many times per day they want to receive reminders using push buttons.

---

# ⚙️ Hardware Components

* Arduino Uno
* 16x2 LCD Display
* DS3231 RTC Module
* Buzzer
* LED
* 4 Push Buttons
* Resistors
* Breadboard and jumper wires

---

# 🔌 Pin Configuration

| Component   | Arduino Pin |
| ----------- | ----------- |
| LCD RS      | 12          |
| LCD E       | 11          |
| LCD D4      | 6           |
| LCD D5      | 5           |
| LCD D6      | 4           |
| LCD D7      | 3           |
| Button 1    | 9           |
| Button 2    | 8           |
| Button 3    | 7           |
| Stop Button | 10          |
| Buzzer      | 2           |
| LED         | LED_BUILTIN |

### RTC Connection (I2C)

| RTC Pin | Arduino |
| ------- | ------- |
| SDA     | A4      |
| SCL     | A5      |

---

# 🧠 System Operation

The system allows the user to select how many times they want to be reminded each day.

| Button   | Mode                         |
| -------- | ---------------------------- |
| Button 1 | Reminder once per day        |
| Button 2 | Reminder twice per day       |
| Button 3 | Reminder three times per day |

Default reminder times:

| Time  | Description |
| ----- | ----------- |
| 08:00 | Morning     |
| 14:00 | Afternoon   |
| 20:00 | Evening     |

When the reminder time is reached:

* The **LED turns on**
* The **buzzer sounds**
* The **LCD shows the notification**

The user can press the **Stop Button** to turn off the alarm.

---

# 💾 EEPROM Usage

The selected reminder mode is stored in **EEPROM** so that the system remembers the setting even after power is turned off.

| EEPROM Value | Meaning                      |
| ------------ | ---------------------------- |
| 1            | Reminder once per day        |
| 2            | Reminder twice per day       |
| 3            | Reminder three times per day |

---

# 📚 Required Libraries

Install the following libraries in **Arduino IDE**:

* LiquidCrystal
* RTClib
* Wire
* EEPROM

These can be installed via **Arduino Library Manager**.

---

# 🔧 Key Features

✔ Automatic medication reminders
✔ Real-time clock display
✔ Select reminder frequency
✔ EEPROM data storage
✔ LCD interface
✔ Buzzer and LED alarm

---

# 📈 Future Improvements

Possible upgrades for this project:

* Add **Bluetooth or WiFi connectivity**
* Connect with a **mobile application**
* Allow users to **customize reminder times**
* Add **battery backup**
* Improve the **user interface**

---

# 👨‍💻 Author

**Bao Nguyen**

This project was developed for learning purposes in:

* Arduino Programming
* Embedded Systems
* IoT Development


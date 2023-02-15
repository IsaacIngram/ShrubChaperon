#include "Adafruit_seesaw.h"
#include <LiquidCrystal.h>
#include <SPI.h>
#include <WiFi.h>

/// The I2C address of the moisture sensor
#define SENSOR_ADDRESS 0x36
/// Sensor touch pin
#define SENSOR_TOUCH_PIN 0
/// Arduino digital pin the display RS pin is connected to
#define LCD_RS_PIN 7
/// Arudino digital pin the display EN pin is connected to
#define LCD_EN_PIN 8
/// Arduino digitial pin the display db4 pin is connected to
#define LCD_DB4_PIN 9
/// Arduino digitial pin the display db5 pin is connected to
#define LCD_DB5_PIN 10
/// Arduino digitial pin the display db6 pin is connected to
#define LCD_DB6_PIN 11
/// Arduino digitial pin the display db7 pin is connected to
#define LCD_DB7_PIN 12
/// Number of display rows
#define LCD_ROWS 2
/// Number of display columns
#define LCD_COLUMNS 16
/// 
#define CONNECTION_DELAY_MILLIS 10000
/// The SSID of the network to connect to
char wifi_ssid[] = "NETWORK_NAME";
/// The password of the network to connect to
char wifi_password[] = "NETWORK_PASSWORD";


Adafruit_seesaw sensor;
LiquidCrystal lcd(
  LCD_RS_PIN, 
  LCD_EN_PIN, 
  LCD_DB4_PIN, 
  LCD_DB5_PIN, 
  LCD_DB6_PIN, 
  LCD_DB7_PIN
);

/**
 * Initialize serial, the LCD, and the moisture sensor.
 */
void setup() {
  // Initialize serial
  Serial.begin(115200);
  Serial.println("Program starting...");

  // Initialize LCD
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.print("Booting...");

  // Initialize moisture sensor. This loops until it is connected.
  while(!sensor.begin(SENSOR_ADDRESS)) {
    Serial.println("ERROR: Sensor not found");
    lcd.print("Error: No sensor");
    delay(500);
  }

  // Begin the wifi connection
  WiFi.begin(wifi_ssid, wifi_password);
}

long last_connect_attempt_millis = 0;

void loop() {
  // Define string later used to print data to the LCD
  String lineOne = "";
  String lineTwo = "";

  // Get moisture from sensor and format for printing
  int moisture = sensor.touchRead(SENSOR_TOUCH_PIN);
  lineOne = "Val: " + moisture;

  // Check Wifi status
  if(WiFi.status() == WL_CONNECTED) {
    //TODO send data to DataDog
    // Format printing nothing
    lineTwo = "";
  } else {
    // Format printing no wifi
    lineTwo = "No wifi";

    // Check time since the last attempted wifi connection. This uses the
    // absolute value as the millis() counter resets after 50 days of runtime
    if(abs(millis() - last_connect_attempt_millis) > CONNECTION_DELAY_MILLIS)
    {
      // Begin Wifi connection again
      WiFi.begin(wifi_ssid, wifi_password);
      last_connect_attempt_millis = millis();
    }
  }

  // Print to LCD and serial
  lcd.print(lineOne + "\n" + lineTwo);
  Serial.print(lineOne + "\n" + lineTwo + "\n");

  delay(1000);
}


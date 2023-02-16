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
#define LCD_DB4_PIN 2
/// Arduino digitial pin the display db5 pin is connected to
#define LCD_DB5_PIN 4
/// Arduino digitial pin the display db6 pin is connected to
#define LCD_DB6_PIN 12
/// Arduino digitial pin the display db7 pin is connected to
#define LCD_DB7_PIN 13
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
  Serial.begin(9600);
  Serial.println("Program starting...");

  // Initialize LCD
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.clear();
  lcd.print("SproutChaperone");
  lcd.setCursor(0, 1);
  lcd.print("Starting...");

  // Initialize moisture sensor. This loops until it is connected.
  while(!sensor.begin(SENSOR_ADDRESS)) {
    Serial.println("ERROR: Sensor not found");
    lcd.setCursor(0, 0);
    lcd.print("Error: No sensor");
    delay(500);
  }

  // Begin the wifi connection
  WiFi.begin(wifi_ssid, wifi_password);
}

long last_connect_attempt_millis = 0;

void loop() {\
  // Get moisture from sensor
  int moisture = sensor.touchRead(SENSOR_TOUCH_PIN);

  // Get WiFi status
  bool wifi_connected = WiFi.status() == WL_CONNECTED;

  if(wifi_connected) {
    //TODO send data to DataDog
  } else {
    // Check time since the last attempted wifi connection. This uses the
    // absolute value as the millis() counter resets after 50 days of runtime
    if(abs(millis() - last_connect_attempt_millis) > CONNECTION_DELAY_MILLIS)
    {
      // Begin Wifi connection again
      WiFi.begin(wifi_ssid, wifi_password);
      last_connect_attempt_millis = millis();
    }
  }

  // Add moisture level to a string to print
  String moisture_print = "Val: ";
  moisture_print = moisture_print + moisture;

  // Print moisture level on top line
  lcd.clear();
  lcd.print(moisture_print);

  // Switch to second line
  lcd.setCursor(0, 1);

  // Print no wifi if wifi is not connected
  if(!wifi_connected) {
    lcd.print("No wifi");
  }

  delay(1000);
}


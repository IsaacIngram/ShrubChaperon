#include "Adafruit_seesaw.h"
#include <LiquidCrystal.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include "secrets.h"

/*
ADDRESSES AND PINS
*/
/// The I2C address of the moisture sensor
#define SENSOR_ADDRESS 0x36
/// Sensor touch pin
#define SENSOR_TOUCH_PIN 0
/// Arduino digital pin the display RS pin is connected to
#define LCD_RS_PIN 2
/// Arudino digital pin the display EN pin is connected to
#define LCD_EN_PIN 3
/// Arduino digitial pin the display db4 pin is connected to
#define LCD_DB4_PIN 4
/// Arduino digitial pin the display db5 pin is connected to
#define LCD_DB5_PIN 5
/// Arduino digitial pin the display db6 pin is connected to
#define LCD_DB6_PIN 6
/// Arduino digitial pin the display db7 pin is connected to
#define LCD_DB7_PIN 7

/*
DISPLAY INFO
*/
/// Number of display rows
#define LCD_ROWS 2
/// Number of display columns
#define LCD_COLUMNS 16

/*
TIME DELAYS
*/
/// The amount of time to wait before trying to reconnect
#define CONNECTION_DELAY_MILLIS 10000
/// The millis to delay between each loop iteration
#define LOOP_DELAY_MILLIS 1000

// Moisture sensor
Adafruit_seesaw sensor;

// Display
LiquidCrystal lcd(
  LCD_RS_PIN, 
  LCD_EN_PIN, 
  LCD_DB4_PIN, 
  LCD_DB5_PIN, 
  LCD_DB6_PIN, 
  LCD_DB7_PIN
);

WiFiUDP udp;

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
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error: No sensor");
    Serial.println("Error: No sensor");
    delay(500);
  }

  // Begin the wifi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  udp.begin(DD_AGENT_PORT);
}

long last_connect_attempt_millis = 0;
bool was_previously_connected = false;

/**
 * Continuously get the moisture data from the moisture sensor, print it to
 * the display, and update DataDog.
 */
void loop() {
  // Get moisture from sensor
  int moisture = sensor.touchRead(SENSOR_TOUCH_PIN);

  // Get WiFi status
  bool wifi_connected = WiFi.status() == WL_CONNECTED;

  if(wifi_connected) {
    // Send a packet to the DataDog Agent
    udp.beginPacket(DD_AGENT_IP, DD_AGENT_PORT);
    char reply_buffer[100];
    sprintf(reply_buffer, "%s:%d|g", METRIC_NAME, moisture);
    udp.write(reply_buffer);
    udp.endPacket();
  } else {
    // Check time since the last attempted wifi connection. This uses the
    // absolute value as the millis() counter resets after 50 days of runtime
    if(abs(millis() - last_connect_attempt_millis) > CONNECTION_DELAY_MILLIS)
    {
      // Begin Wifi connection again
      WiFi.begin(WIFI_SSID);
      udp.begin(DD_AGENT_PORT);
      last_connect_attempt_millis = millis();
    }
  }

  // Scale the moisture to be printed
  String p_moisture;
  if(moisture < 500) {
    p_moisture = "LOW";
  } else if(moisture > 700) {
    p_moisture = "HIGH";
  } else {
    p_moisture = String(moisture % 500);
  }

  String print = "Moisture: " + p_moisture;

  // Print moisture level on top line
  lcd.clear();
  lcd.print(print);

  // Switch to second line
  lcd.setCursor(0, 1);

  // Print wifi status
  if(!wifi_connected) {
    lcd.print("No wifi");
  } else {
    lcd.print("Wifi connected");
  }



  delay(LOOP_DELAY_MILLIS);
}


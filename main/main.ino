#include "Adafruit_seesaw.h"
#include <LiquidCrystal.h>
#include <SPI.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Statsd.h>

/*
ADDRESSES AND PINS
*/
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
#define LOOP_DELAY_MILLIS 2000

/*
NETWORK INFO
*/
/// The SSID of the network to connect to
#define WIFI_SSID ""
#define WIFI_PASSWORD ""

/*
STATSD INFO
*/
/// The IP of the DataDog Agent
#define DD_AGENT_IP ""
/// The port of the DataDog Agent
#define DD_AGENT_PORT ""
/// The name of the metric
#define METRIC_NAME "plant.moisture"

// Moisture sensor
Adafruit_seesaw sensor;

// Statsd Connection for DataDog Agent
WiFiUDP udp;
Statsd statsd(udp, DD_AGENT_IP, DD_AGENT_PORT);

// Display
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
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Error: No sensor");
    delay(500);
  }

  // Begin the wifi connection
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
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
    // Check if the Wifi was connected last time this loop ran
    if(!was_previously_connected) {
      // If it wasn't previously connected, we need to restart the connection
      // to the DataDog agent
      statsd.begin();
    }
    // Log data into DataDog
    statsd.gauge(METRIC_NAME, moisture);
    was_previously_connected = true;
  } else {
    // Check time since the last attempted wifi connection. This uses the
    // absolute value as the millis() counter resets after 50 days of runtime
    if(abs(millis() - last_connect_attempt_millis) > CONNECTION_DELAY_MILLIS)
    {
      // Begin Wifi connection again
      WiFi.begin(WIFI_SSID);
      last_connect_attempt_millis = millis();
    }
    was_previously_connected = false;
  }

  // Add moisture level to a string to print
  String moisture_print = "Val: ";
  moisture_print = moisture_print + moisture;

  // Print moisture level on top line
  lcd.clear();
  lcd.print(moisture_print);

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


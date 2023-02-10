/**
 * File: main.ino
 * Author: Isaac Ingram
 * This is the main loop for controlling the moisture sensor and display.
 */

#include "Adafruit_seesaw.h"
#include <LiquidCrystal.h>

/// The I2C address of the moisture sensor
#define MOISTURE_SENSOR_ADDRESS 0x36
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

Adafruit_seesaw moisture_sensor;
LiquidCrystal lcd(
  LCD_RS_PIN, 
  LCD_EN_PIN, 
  LCD_DB4_PIN, 
  LCD_DB5_PIN, 
  LCD_DB6_PIN, 
  LCD_DB7_PIN
);

void setup() {

  // Initialize serial
  Serial.begin(115200);
  Serial.println("Program starting...");

  // Initialize LCD
  lcd.begin(LCD_COLUMNS, LCD_ROWS);
  lcd.print("Booting...");

  // Initialize moisture sensor
  while(!moisture_sensor.begin(MOISTURE_SENSOR_ADDRESS)) {
    Serial.println("ERROR: Sensor not found");
    lcd.print("Error: No sensor");
    delay(500);
  }

}
void loop() {
  // put your main code here, to run repeatedly:

}

// include the library code:
#include <SD.h>
#include <SPI.h>

#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

// define the backlight color
#define WHITE 0x7

// global vars
char time_str[16];
unsigned long total_time_min = 0;
unsigned long total_time_prev = 0;
int save_interval_in_min = 1;
File myFile;

void setup() {
  // put your setup code here, to run once

  // setup serial port
  Serial.begin(9600);

  // set up the LCD's number of columns and rows
  lcd.begin(16, 2);
  lcd.setBacklight(WHITE);

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

  // save starting voltage
  get_time();
  total_time_prev -= save_interval_in_min;
  save_data();

  Serial.println("Hello World!");
}

void loop() {
  // put code here that runs continously
  update_screen();
  save_data();
  delay(1000);
}

void save_data() {

  if ((total_time_min - total_time_prev) < save_interval_in_min) {
    return;
  }
  total_time_prev = total_time_min;

  float v = measure_voltage();
  
  myFile = SD.open("test.txt", FILE_WRITE);
  myFile.print(total_time_min);
  myFile.print(",");
  myFile.print(v);
  myFile.print("\n");
  myFile.close();
  return;
}

void get_time() {
  // get running time
  unsigned long total_time_sec = millis() / 1000;
  total_time_min = total_time_sec / 60;
  unsigned long total_time_hr = total_time_min / 60;
  int sec = total_time_sec % 60;
  int min = total_time_min % 60;
  int hr = total_time_hr;
  sprintf(time_str, "Time: %02d:%02d:%02d", hr, min, sec);
  return;
}

void update_screen() {
  // measure voltage
  float v = measure_voltage();

  // put cursor at (0, 0) position
  lcd.setCursor(0, 0);

  // print time
  get_time();
  Serial.println(time_str);
  lcd.print(time_str);

  // print voltage
  lcd.setCursor(0, 1);
  lcd.print("Volt: ");
  lcd.setCursor(6, 1);
  lcd.print(v);
  Serial.println(v);
}

float measure_voltage() {
  // read A0 pin from ADC, convert to a floating point number
  float y = float(analogRead(A0));

  // convert from ADC value to voltage
  float v = (y / 1024) * 5.0;

  return v;
}

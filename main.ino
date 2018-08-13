#include <LCD_Launchpad.h>
#include "I2C_SoftwareLibrary.h"
#include "SGP30.h"
#include "SHT21.h"
#include "GUI.h"

/********************************************
 * Define the interval of measurements here!!
 * (value in milliseconds)
 ********************************************/
#define MEAS_INTERVAL  500
/********************************************
 * Uncomment this line if you want to print
 * information in serial monitor.
 * Increases required RAM and is slower
 ********************************************/
//#define DEBUG_MODE

// Defines for I2C library
#define SDA_PIN P8_3
#define SCL_PIN P8_2
SoftwareWire Wire(SDA_PIN, SCL_PIN);

#define LED_RED     P1_7
#define LED_GREEN   P1_6

//************** Global Variables ****************
  uint8_t SHT21_CRC = 0;
  unsigned long long SGP30_serialID = 0;
  
  float temperature = 0;
  float humidity = 0;
  unsigned int SGP30_CO2 = 0;
  unsigned int SGP30_TVOC = 0;
  
  float temperature_max = 0;
  float humidity_max = 0;
  unsigned int CO2_max = 0;
  
  volatile boolean leftButton = false;
  volatile boolean rightButton = false;
  boolean show_max = false;
  uint8_t screen = 1; // start at CO2 screen
  
  // Create C++ objects
  SGP30 sgp30;
  SHT21 sht21;
  CRC crc;
  LCD_LAUNCHPAD lcd;
  GUI gui;
//*****************************************

void setup() {

  lcd.clear();
  lcd.displayScrollText("Initializing", 100);
  
  //********** Pin configuration ***********
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
  pinMode(PUSH1, INPUT_PULLUP);
  pinMode(PUSH2, INPUT_PULLUP);
  attachInterrupt(PUSH1, _button1ISR, FALLING);  
  attachInterrupt(PUSH2, _button2ISR, FALLING);

#ifdef DEBUG_MODE
  // Initialize Console
  Serial.begin(9600);
#endif
  // Initialize I2C
  Wire.begin();
  // Initialize LCD
  lcd.init();

  delay(1000);

//*****************************************

  // Create the CRC look-up table
  crc.Init();

  // Reset CO2 sensor because of undefined values after hardware reset
  sgp30.softReset();
  delay(500);
  // Self-test (sensor should return 0xD400)
  // Blink red LED if test failed
  if(sgp30.isInitialised() == false) {
    while(1) {
      digitalWrite(LED_RED, LOW);
      delay(200);
      digitalWrite(LED_RED, HIGH);
      delay(200);
    }
  }
  // Initialize CO2 and TVOC measurement
  sgp30.initializeMeasurement();
}

void loop() {

  // Read CO2 sensor
  sgp30.getMeasurementData(&SGP30_CO2, &SGP30_TVOC);
  // Read humidity and temperature sensor
  humidity = sht21.readSensor(HUMIDITY);
  temperature = sht21.readSensor(TEMP);

  // Save maximal values
  if(temperature > temperature_max)
    temperature_max = temperature;
  // If value is >40000, measurement was incorrect
  if((SGP30_CO2 > CO2_max) && (SGP30_CO2 < 40000))
    CO2_max = SGP30_CO2;
  if(humidity > humidity_max)
    humidity_max = humidity;

#ifdef DEBUG_MODE
  Serial.print("Temperature: ");
  Serial.println(temperature);
  Serial.print("Relative Humidity: ");
  Serial.println(humidity);
  Serial.print("CO2: ");
  Serial.println(SGP30_CO2);
  Serial.print("TVOC: ");
  Serial.println(SGP30_TVOC);
#endif

  if(!show_max) {
    switch(screen) {
      case SCREEN_CO2:
        gui.showCO2(SGP30_CO2); break;
      case SCREEN_TEMP:
        gui.showTemperature(temperature); break;
      case SCREEN_RH:
        gui.showHumidity(humidity); break;
      default: break;
    }
  }
  else {
    switch(screen) {
      case SCREEN_CO2:
        gui.showCO2(CO2_max); break;
      case SCREEN_TEMP:
        gui.showTemperature(temperature_max); break;
      case SCREEN_RH:
        gui.showHumidity(humidity_max); break;
      default: break;
    }
    lcd.showSymbol(LCD_SEG_MARK, true);
  }

  // Show maximum values if left button is pressed
  if(leftButton && !rightButton) {
    if(show_max)
      show_max = false;
    else
      show_max = true;
    leftButton = false;
    // Re-enable left button interrupt
    attachInterrupt(PUSH1, _button1ISR, FALLING);
  }
  //Switch to next screen if right button is pressed
  if(rightButton && !leftButton) {
    if(screen < 3)
      screen++;
    else
      screen = 1;
    rightButton = false;
    // Re-enable right button interrupt
    attachInterrupt(PUSH2, _button2ISR, FALLING);
  }
  // Delete maximum values if right button is held down
  if(rightButton && leftButton) {
    temperature_max = 0;
    humidity_max = 0;
    CO2_max = 0;
    rightButton = false;
    leftButton = false;
   
    // Wait until buttons are released
    while(!digitalRead(PUSH1) && !digitalRead(PUSH2)) {
    }
    // Re-enable left and right button interrupt
    attachInterrupt(PUSH1, _button1ISR, FALLING);
    attachInterrupt(PUSH2, _button2ISR, FALLING);
  }
  
  sleep(MEAS_INTERVAL);
}

// Function is called when button S1 is pressed
void _button1ISR() {
  leftButton = true;
  // Disable this interrupt
  detachInterrupt(PUSH1);
}

// Function is called when button S2 is changed
void _button2ISR() {
  rightButton = true;
  // Disable this interrupt
  detachInterrupt(PUSH2);
}


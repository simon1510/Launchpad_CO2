/*
   sht21.c

    Created on: 23.05.2018
        Author: HaagS
*/

#include "GUI.h"
#include "itoa.h"

extern LCD_LAUNCHPAD lcd;

//**********************************************************************************
// Prints a 32-bit float value on the display.
//
// input:     floatValue     Float value that should be printed on display
//
// output:    none
//
// return:    none
//**********************************************************************************
void GUI::printFloat(float floatValue) {

  char intStr[10];
  uint8_t i;
  uint8_t numberLength;
  uint8_t position;

  uint16_t integerValue = (uint16_t)(floatValue * 100);
  itoa(integerValue, intStr, 10);

  lcd.clear();
  lcd.showSymbol(LCD_SEG_DOT3, true);

  if(floatValue < 100) {
    numberLength = 4;
    position = 1;
  }
  else if(floatValue < 10) {
    numberLength = 3;
    position = 2;
  }
  else {
    numberLength = 5;
    position = 0;
  }
  for(i=0; i<numberLength; i++) {
    lcd.showChar(intStr[i], position+i);
  }
}

//**********************************************************************************
// Prints a 16-bit integer value on the display.
//
// input:     integerValue    Integer value that should be printed on display
//
// output:    none
//
// return:    none
//**********************************************************************************
void GUI::printInteger(uint16_t integerValue) {

  char intStr[10];
  uint8_t numberLength;
  uint8_t position;
  
  itoa(integerValue, intStr, 10);

  lcd.clear();

  if(integerValue < 1000) {
    numberLength = 3;
    position = 2;
  }
  else if(integerValue < 10000) {
    numberLength = 4;
    position = 1;
  }
  else if(integerValue >= 10000) {
    numberLength = 5;
    position = 0;
  }

  for(uint8_t i = 0; i < numberLength; i++) {
    lcd.showChar(intStr[i], position + i);
  }
}

//**********************************************************************************
// Prints a 16-bit integer CO2 value on the display and turns on 
// the related display segments.
//
// input:     co2          CO2 integer value that should be printed on display
//
// output:    none
//
// return:    none
//**********************************************************************************
void GUI::showCO2(uint16_t co2) {

  lcd.clear();
  printInteger(co2);
  lcd.showSymbol(LCD_SEG_BAT0, true);
  lcd.showSymbol(LCD_SEG_BAT1, true);
}

//**********************************************************************************
// Prints a 32-bit float temperature value on the display and turns on 
// the related display segments.
//
// input:     temperature    Temperature float value that should be printed on display
//
// output:    none
//
// return:    none
//**********************************************************************************
void GUI::showTemperature(float temperature) {

  lcd.clear();
  printFloat(temperature);
  lcd.showSymbol(LCD_SEG_BAT2, true);
  lcd.showSymbol(LCD_SEG_BAT3, true);
}

//**********************************************************************************
// Prints a 32-bit float relativ humidity value on the display and turns on 
// the related display segments.
//
// input:     humidity    Humidity float value that should be printed on display
//
// output:    none
//
// return:    none
//**********************************************************************************
void GUI::showHumidity(float humidity) {

  lcd.clear();
  printFloat(humidity);
  lcd.showSymbol(LCD_SEG_BAT4, true);
  lcd.showSymbol(LCD_SEG_BAT5, true);
}


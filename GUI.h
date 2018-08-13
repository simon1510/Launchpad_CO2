/*
 * sht21.h
 *
 *  Created on: 23.05.2018
 *      Author: HaagS
 */

#ifndef GUI_H_
#define GUI_H_

#include "Energia.h"
#include <LCD_Launchpad.h>

#define SCREEN_CO2  1
#define SCREEN_TEMP 2
#define SCREEN_RH   3

class GUI {
  private: 
    void printFloat(float floatValue);
    void printInteger(unsigned int integer);   
  public:
    void showCO2(unsigned int co2);
    void showTemperature(float temperature);
    void showHumidity(float humidity);
};

#endif /* GUI_H_ */

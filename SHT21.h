/*
 * sht21.h
 *
 *  Created on: 23.05.2018
 *      Author: HaagS
 */

#ifndef SHT21_H_
#define SHT21_H_

#include "Energia.h"
#include <stdint.h>

#include "I2C_SoftwareLibrary.h"
extern SoftwareWire Wire;

// slave address
#define SHT21_ADDRESS					    0x40

// commands
#define SHT21_TRIGGER_T_MEAS			0xF3
#define SHT21_TRIGGER_RH_MEAS			0xF5
#define SHT21_READ_USER_REG				0xE7
#define SHT21_WRITE_USER_REG			0xE6
#define SHT21_RESET						    0xFE

// measure modes
enum {
	HUMIDITY = 0x01, TEMP = 0x02
};

class SHT21 {
  private:  
    uint8_t readUserRegister(void);
    void writeUserRegister(uint8_t register_value);
    
  public:
    float readSensor(uint8_t MeasureType);
    boolean checkCRC(uint8_t *data, uint8_t numberOfBytes, uint8_t checksum);
    void softReset(void);
};

#endif /* SHT21_H_ */

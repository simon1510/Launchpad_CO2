/*
 * sht21.c
 *
 *  Created on: 23.05.2018
 *      Author: HaagS
 */

#include "SHT21.h"

//**********************************************************************************
// Calculates checksum for n bytes of data and compares it with expected checksum
//
// input: 		*data           checksum is built based on this data
// 		  		  nbrOfBytes      checksum is built for n bytes of data
//        		checksum        expected checksum
//
// output:    none
//
// return:  	0 = checksum does not match
// 				    1 = checksum matches
//**********************************************************************************
boolean SHT21::checkCRC(uint8_t *data, uint8_t numberOfBytes, uint8_t checksum){

	uint8_t crc = 0;
	uint8_t byteCtr;
	uint8_t bit;

	const unsigned int CRC_POLYNOMIAL =	0b100110001;	// 0x131

	//calculates 8-Bit checksum with given polynomial
	for(byteCtr = 0; byteCtr < numberOfBytes; ++byteCtr){
		crc ^= (data[byteCtr]);

		for(bit = 8; bit > 0; --bit){
			if(crc & 0x80) crc = (crc << 1) ^ CRC_POLYNOMIAL;
			else crc = (crc << 1);
		}
	}
	if(crc != checksum) {
    Serial.println("ERROR SHT21: Unexpected checksum value");
	  return false;
	}
	else return true;
}

//**********************************************************************************
// Performs a measurement of humidity or temperature. This function automatically
// polls result every 10 ms until measurement is ready.
//
// input: 		MeasureType     Can be 'HUMIDITY' (01h) or 'TEMP' (02h)
//
// output:    none
//     		
// return: 		result_value    Humidity/ temperature as float value
//**********************************************************************************
float SHT21::readSensor(uint8_t MeasureType){

	uint8_t received_data[3];
	uint8_t command;
	unsigned int data;
  
	// select measure type and set command
	switch (MeasureType){
		case HUMIDITY:
			command = SHT21_TRIGGER_RH_MEAS; break;
		case TEMP:
			command = SHT21_TRIGGER_T_MEAS; break;
		default: command = SHT21_RESET;	// reset if MeasureType is invalid
	}
	// transmit command
  Wire.beginTransmission(SHT21_ADDRESS);
  Wire.write(command);
  Wire.endTransmission();

	// wait for measurement time depending on measure type
	if(MeasureType == HUMIDITY)delay(35);
	else if(MeasureType == TEMP)delay(90);
	else {
    Serial.println("ERROR SHT21: Unexpected parameter (MeasureType)");
	  return 0;
	}

	// read 2 data bytes and 1 checksum byte
	// combine data to one 16-Bit raw value
  Wire.requestFrom(SHT21_ADDRESS, 3);
  received_data[0] = Wire.read();
  received_data[1] = Wire.read();
  received_data[2] = Wire.read();
  
	data = (received_data[0]<<8) | received_data[1];

	// checksum error detection
	uint8_t crc_data[2] = {received_data[0], received_data[1]};
	if(checkCRC(crc_data, 2, received_data[2]) == true);
	else Serial.println("ERROR SHT21: Unexpected checksum value");

	// calculate humidity or temperature
	// in dependence of measure type
	if(MeasureType == HUMIDITY)
		return (-6.0 + 125.0/65536 * (float)data);
	else if(MeasureType == TEMP)
		return (-46.85 + 175.72/65536 * (float)data);
	else {
    Serial.println("ERROR SHT21: Unexpected measured value");
	  return 0;
	}
}

//**********************************************************************************
// Reads the SHT21 user register (8bit)
//
// input:   none
//
// output: 	none
//
// return:	register_value    actual value of user register
//**********************************************************************************
uint8_t SHT21::readUserRegister(void){

  // send command to read user register
  Wire.beginTransmission(SHT21_ADDRESS);
  Wire.write(SHT21_READ_USER_REG);
  // read register data
  Wire.requestFrom(SHT21_ADDRESS, 1);
  uint8_t register_value = Wire.read();
  Wire.endTransmission();

	return register_value;
}

//**********************************************************************************
// Writes the SHT21 user register (8bit)
//
// input: 	register_value	  value written to user register
//
// output:  none
//
// return: 	error
//**********************************************************************************
void SHT21::writeUserRegister(uint8_t register_value){

	uint8_t transmit_data[2] = {SHT21_WRITE_USER_REG, register_value};

  // send command to write user register
  Wire.beginTransmission(SHT21_ADDRESS);
  Wire.write(transmit_data, 2);
  Wire.endTransmission();
}

//******************************************
// Performs a soft reset for SHT21
//
// input:   none
//
// output:  none
//
// return:  none
//******************************************
void SHT21::softReset(void){

  uint8_t transmit_data = SHT21_RESET;
  
	// send reset command
  Wire.beginTransmission(SHT21_ADDRESS);
  Wire.write(&transmit_data, 1);
  Wire.endTransmission();

	delay(15);	// delay for start-up

  // re-initialize I2C
  Wire.begin();
}


/*
 * SGP30.c
 *
 *  Created on: 18.05.2018
 *      Author: HaagS
 */

//#include "Wire.h"
#include "SGP30.h"

// Object of CRC calculation
extern CRC crc;

//*********************************************************
// Perform a checksum test for all receveid data
//
// input:	  *data			  received data to check
//			    byteCtr			count of received bytes (crc included)
//
// output:  none
//
// return:	boolean			false if crc is incorrect
//*********************************************************
bool SGP30::checksumCalculation(uint8_t *data, uint8_t byteCtr) {

	uint8_t i = 0;
	uint8_t k = 0;

	uint8_t tmpData[2] = {0};
	boolean crcResult[3] = {true, true, true};

	for(i=0; i<byteCtr; i=i+3) {
		tmpData[0] = data[i];
		tmpData[1] = data[i+1];
		if(crc.Fast(tmpData, 2) == data[i+2])
			crcResult[k] = true;
		else crcResult[k] = false;
		k++;
	}

	if(crcResult[0] == true && crcResult[1] == true && crcResult[2] == true)
		return true;
	else
		return false;
}

//*********************************************************
// Read out Serial ID of device
//
// input:	  none
//
// output:  none
//
// return:	serial ID (32 bits, returned in 64-bit value)
//*********************************************************
unsigned long long SGP30::getSerialID(void) {

	uint8_t id_rawData[9] = {0};
	uint8_t cmd[2] = {SGP30_GET_SERIAL_ID >> 8, SGP30_GET_SERIAL_ID & 0x00FF};

	// Get 9 bytes raw data of ID with checksums
	Wire.beginTransmission(SGP30_ADDRESS);
	Wire.write(cmd, 2);
  Wire.requestFrom(SGP30_ADDRESS, 9);
  uint8_t i = 0;
  while(Wire.available()) {
    id_rawData[i++] = Wire.read();
  }
  Wire.endTransmission();

	unsigned int id1 = id_rawData[8] << 8 | id_rawData[7];
	unsigned int id2 = id_rawData[5] << 8 | id_rawData[4];
	unsigned int id3 = id_rawData[2] << 8 | id_rawData[1];

	boolean dataValid = checksumCalculation(id_rawData, 9);

	if(!dataValid) {
		Serial.println("ERROR SGP30: Unexpected checksum value");
	}
	else return ((unsigned long long)id1 << 32) | ((unsigned long long)id2 << 16) | id3;
}

//*********************************************************
// Initialize measurement
// Needs to be called once after every power-up or reset!
//
// input:	  none
//
// output:  none
//
// return:	none
//*********************************************************
void SGP30::initializeMeasurement(void) {

	uint8_t transmitData[2] = {(SGP30_INIT_AIR_QUALITY>>8),
						   (SGP30_INIT_AIR_QUALITY&0xFF)};

	// Transmit command to initialize measurement
	Wire.beginTransmission(SGP30_ADDRESS);
  Wire.write(transmitData, 2);
  Wire.endTransmission();
}

//*********************************************************
// Perform measurement and get data
// Should be  called every second to ensure proper
// operation of internal dynamic correction algorithms
//
// input:   none
//
// output:  *CO2ppm			CO2 value
//			    *TVOCppb		TVOC value
//
// return:	none
//*********************************************************
void SGP30::getMeasurementData(unsigned int *CO2ppm, unsigned int *TVOCppb) {

	uint8_t receiveData[6] = {0};
	uint8_t cmd[2] = {SGP30_MEASURE_AIR_QUALITY >> 8, SGP30_MEASURE_AIR_QUALITY & 0x00FF};

	// Fetch measurement data
	Wire.beginTransmission(SGP30_ADDRESS);
  Wire.write(cmd, 2);
	Wire.requestFrom(SGP30_ADDRESS, 6);
  uint8_t i = 0;
  while(Wire.available()) {
    receiveData[i++] = Wire.read();
  }
	Wire.endTransmission();

	// Convert raw data
	*CO2ppm = (unsigned int)receiveData[0]<<8 | receiveData[1];
	*TVOCppb = (unsigned int)receiveData[3]<<8 | receiveData[4];

	boolean dataValid = checksumCalculation(receiveData, 6);

	if(dataValid != true) {
		Serial.println("ERROR SGP30: Unexpected checksum value");
	}
}

//*********************************************************
// Do a self-test of the sensor
// Sensor should respond 0xD400 over I2C
//
// input:	  none
//
// output:  none
//
// return:	1 - initialised correctly
//          0 - initialised incorrectly
//*********************************************************
boolean SGP30::isInitialised(void) {

	uint8_t receiveData[3] = {0};
	uint8_t cmd[2] = {SGP30_MEASURE_TEST >> 8, SGP30_MEASURE_TEST & 0x00FF};
	unsigned int checkData = 0;

	// Send command for self-test
	Wire.beginTransmission(SGP30_ADDRESS);
  Wire.write(cmd, 2);
	Wire.requestFrom(SGP30_ADDRESS, 3);
  uint8_t i = 0;
  while(Wire.available()) {
    receiveData[i++] = Wire.read();
  }
  Wire.endTransmission();

	boolean dataValid = checksumCalculation(receiveData, 3);

	if(dataValid == true) {
		// Validate received data pattern (should be 0xD400)
		checkData = ((unsigned int)receiveData[0] << 8) | receiveData[1];
	}

	if(checkData != 0xD400) {
    Serial.println("ERROR SGP30: Sensor is not initialised correctly");
		return false;
	}
	else return true;
}

//*********************************************************
// Perform a soft reset
// Warning: All devices that support general call mode
// will perform a reset!
//
// input:	  none
//
// output:  none
//
// return:	none
//*********************************************************
void SGP30::softReset(void) {

	uint8_t transmitData[2] = {(SGP30_RESET_COMMAND >> 8),
						   (SGP30_RESET_COMMAND & 0xFF)};

	// Send command for soft reset to general call address
	Wire.beginTransmission(I2C_GENERAL_CALL_ADDRESS);
  Wire.write(transmitData, 2);
  Wire.endTransmission();
}

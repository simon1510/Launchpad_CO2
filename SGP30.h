/*
 * SGP30.h
 *
 *  Created on: 18.05.2018
 *      Author: HaagS
 */

#ifndef SGP30_H_
#define SGP30_H_

#include "crc.h"
#include <stdint.h>

#include "I2C_SoftwareLibrary.h"
extern SoftwareWire Wire;

#define SGP30_ADDRESS		0x58

//***************************
// Feature Set Commands
//***************************
#define SGP30_INIT_AIR_QUALITY			  0x2003
#define SGP30_MEASURE_AIR_QUALITY		  0x2008

#define SGP30_GET_BASELINE				    0x2015		// Calibration data
#define SGP30_SET_BASELINE				    0x201E
#define SGP30_MEASURE_TEST				    0x2032
#define SGP30_GET_FEATURE_SET_VERISON	0x202F
#define SGP30_MEASURE_SIGNALS			    0x2050		// Requires a reference gas concentration

#define SGP30_GET_SERIAL_ID				    0x3682

//***************************
// Reset Commands
//***************************
#define I2C_GENERAL_CALL_ADDRESS		  0x0000		// !! All devices that support general call mode will be resetted !!
#define SGP30_RESET_SCND_BYTE			    0x0006
#define SGP30_RESET_COMMAND				    0x0006

//***************************
// Methods
//***************************
class SGP30{
  private:
    bool checksumCalculation(uint8_t *data, uint8_t byteCtr);
    
  public:
    unsigned long long getSerialID(void);
    void initializeMeasurement(void);
    void getMeasurementData(unsigned int *CO2ppm, unsigned int *TVOCppb);
    boolean isInitialised(void);
    void softReset(void);
};

#endif /* SGP30_H_ */

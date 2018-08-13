/**********************************************************************
 *
 * Filename:    crc.h
 * 
 * Description: A header file describing the various CRC standards.
 *
 * Notes:
 * CRC-Calculator (Javascript):
 * http://www.sunshine2k.de/coding/javascript/crc/crc_js.html
 **********************************************************************/

#ifndef _crc_h
#define _crc_h

#include <stdint.h>

/*
 * Select the CRC standard from the list that follows.
 */
#define CRC8
//#define CRC8_TEST

#if defined(CRC8)

typedef uint8_t crcType;

#define CRC_NAME			    "CRC-8"
#define POLYNOMIAL			  0x31
#define INITIAL_REMAINDER	0xFF
#define FINAL_XOR_VALUE		0x00
#define REFLECT_DATA		  false
#define REFLECT_REMAINDER	false
#define CRC8_TEST_VALUE		0x13		// Mit Polynom 0x1D
#define CHECK_VALUE			  0x2E

#elif defined(CRC_CCITT)

typedef unsigned int crcType;

#define CRC_NAME			  "CRC-CCITT"
#define POLYNOMIAL			  0x1021
#define INITIAL_REMAINDER	0xFFFF
#define FINAL_XOR_VALUE		0x0000
#define REFLECT_DATA		  false
#define REFLECT_REMAINDER	false
#define CHECK_VALUE			  0x29B1

#elif defined(CRC16)

typedef unsigned int crcType;

#define CRC_NAME			    "CRC-16"
#define POLYNOMIAL			  0x8005
#define INITIAL_REMAINDER	0x0000
#define FINAL_XOR_VALUE		0x0000
#define REFLECT_DATA		  false
#define REFLECT_REMAINDER	false
#define CHECK_VALUE			  0xBB3D

#elif defined(CRC32)

typedef unsigned long crcType;

#define CRC_NAME			    "CRC-32"
#define POLYNOMIAL			  0x04C11DB7
#define INITIAL_REMAINDER	0xFFFFFFFF
#define FINAL_XOR_VALUE		0x0
#define REFLECT_DATA		  true
#define REFLECT_REMAINDER	true
#define CHECK_VALUE			  0xD40420B6

#else

#error "One of CRC8, CRC_CCITT, CRC16, or CRC32 must be #define'd."

#endif

class CRC {

  public:
    void Init(void);
    crcType Slow(uint8_t const message[], unsigned int nBytes);
    crcType Fast(uint8_t const message[], unsigned int nBytes);
    uint8_t getOddParity(uint8_t p);
};

#endif /* _crc_h */

/**********************************************************************
 *
 * Filename:    crc.c
 * 
 * Description: Slow and fast implementations of the CRC standards.
 *
 * Notes:
 *
 **********************************************************************/


#include "crc.h"

/*
 * Derive parameters from the standard-specific parameters in crc.h.
 */
#define WIDTH    (8 * sizeof(crcType))
#define TOPBIT   ((unsigned long)1 << (WIDTH - 1))

#if (REFLECT_DATA == TRUE)
#undef  REFLECT_DATA
#define REFLECT_DATA(X)			((crcType) reflect((X), 8))
#else
#undef  REFLECT_DATA
#define REFLECT_DATA(X)			((crcType)(X))
#endif

#if (REFLECT_REMAINDER == TRUE)
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)	((crcType) reflect((X), WIDTH))
#else
#undef  REFLECT_REMAINDER
#define REFLECT_REMAINDER(X)	((crcType)(X))
#endif

/*********************************************************************
 *
 * Function:    reflect()
 * 
 * Description: Reorder the bits of a binary sequence, by reflecting
 *				them about the middle position.
 *
 * Notes:		No checking is done that nBits <= 32.
 *
 * Returns:		The reflection of the original data.
 *
 *********************************************************************/
static unsigned long reflect(unsigned long data, uint8_t nBits) {
	unsigned long reflection = 0x00000000;
	uint8_t bit;

	/*
	 * Reflect the data about the center bit.
	 */
	for (bit = 0; bit < nBits; ++bit) {
		/*
		 * If the LSB bit is set, set the reflection of it.
		 */
		if (data & 0x01) {
			reflection |= ((unsigned long long) 1 << ((nBits - 1) - bit));
		}

		data = (data >> 1);
	}

	return (reflection);

} /* reflect() */

/*********************************************************************
 *
 * Function:    crcSlow()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:		
 *
 * Returns:		The CRC of the message.
 *
 *********************************************************************/
crcType CRC::Slow(uint8_t const message[], unsigned int nBytes) {
	crcType remainder = INITIAL_REMAINDER;
	unsigned int byte;
	uint8_t bit;

	/*
	 * Perform modulo-2 division, a byte at a time.
	 */
	for (byte = 0; byte < nBytes; ++byte) {
		/*
		 * Bring the next byte into the remainder.
		 */
		remainder ^= ((REFLECT_DATA(message[byte])) << (WIDTH - 8));

		/*
		 * Perform modulo-2 division, a bit at a time.
		 */
		for (bit = 8; bit > 0; --bit) {
			/*
			 * Try to divide the current data bit.
			 */
			if (remainder & TOPBIT) {
				remainder = (remainder << 1) ^ POLYNOMIAL;
			} else {
				remainder = (remainder << 1);
			}
		}
	}

	/*
	 * The final remainder is the CRC result.
	 */
	return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);

} /* crcSlow() */

crcType crcTable[256];

/*********************************************************************
 *
 * Function:    crcInit()
 * 
 * Description: Populate the partial CRC lookup table.
 *
 * Notes:		This function must be rerun any time the CRC standard
 *				is changed.  If desired, it can be run "offline" and
 *				the table results stored in an embedded system's ROM.
 *
 * Returns:		None defined.
 *
 *********************************************************************/
void CRC::Init(void) {
	crcType remainder;
	unsigned long dividend;
	uint8_t bit;

	/*
	 * Compute the remainder of each possible dividend.
	 */
	for (dividend = 0; dividend < 256; ++dividend) {
		/*
		 * Start with the dividend followed by zeros.
		 */
		remainder = dividend << (WIDTH - 8);

		/*
		 * Perform modulo-2 division, a bit at a time.
		 */
		for (bit = 8; bit > 0; --bit) {
			/*
			 * Try to divide the current data bit.
			 */
			if (remainder & TOPBIT) {
				remainder = (remainder << 1) ^ POLYNOMIAL;
			} else {
				remainder = (remainder << 1);
			}
		}

		/*
		 * Store the result into the table.
		 */
		crcTable[dividend] = remainder;
	}

} /* crcInit() */

/*********************************************************************
 *
 * Function:    crcFast()
 * 
 * Description: Compute the CRC of a given message.
 *
 * Notes:		crcInit() must be called first.
 *
 * Returns:		The CRC of the message.
 *
 *********************************************************************/
crcType CRC::Fast(uint8_t const message[], unsigned int nBytes) {
	crcType remainder = INITIAL_REMAINDER;
	uint8_t data;
	unsigned int byte;

	/*
	 * Divide the message by the polynomial, a byte at a time.
	 */
	for (byte = 0; byte < nBytes; ++byte) {
		data = REFLECT_DATA(message[byte]) ^ (remainder >> (WIDTH - 8));
		remainder = crcTable[data] ^ (remainder << 8);
	}

	/*
	 * The final remainder is the CRC.
	 */
	return (REFLECT_REMAINDER(remainder) ^ FINAL_XOR_VALUE);

} /* crcFast() */

uint8_t CRC::getOddParity(uint8_t p) {
	// p = a.b.c.d.e.f.g.h
	p ^= (p >> 4);	// p = x.x.x.x.a^e.b^f.c^g.d^h  		(x = don't care bit)
	p ^= (p >> 2);	// p = x.x.x.x.x.x.a^c^e^g.b^d^f^h
	p ^= (p >> 1);	// p = x.x.x.x.x.x.x.a^b^c^d^e^f^g^h

	return !(p & 1);
}

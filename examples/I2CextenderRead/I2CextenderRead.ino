/*
 * IO Expander demo code
 *
 * This version simply walks thru the boards/ports and flashes the lights
 * first in an on/off pattern, then as a cylon, then randomly
 */

#include "Wire.h"
#include "I2Cextender.h"

/*
 *	void     init(int address, int type, unsigned config);
 *		 address: 0-7 - the individual device, not the real I2C address (0x27...)
 *		 type:
 *		      PCA9555, MCP23016, PCF8574, PCF8574A,
 *		      ARDIO_A  // Bits D0  D1  D2  D3  D4  D5   D6   D7
 *		      ARDIO_B  //      D8  D9  D10 D11 D12 D13 -nc- -nc-
 *		      ARDIO_C  //      A0  A1  A2  A3  A4  A5  -nc- -nc-
 *		 config: a bitmask where 1-bits indicate (I)nputs and 0-bits (O)utputs
 *			reads will only return valid info for "I" bits, 
 *			writes will only modify "O" bits
 *
 *	int      getSize(void);  
 *			returns either 6, 8 or 16, depending on the device type
 *	unsigned read(void) ;
 *	unsigned get(void)                  { return I2Cextender::read(); }
 *			read the device, mask off the invalid bits, return the result
 *
 *	void     write(unsigned data);
 *	void     put(unsigned data) { I2Cextender::write(data);   }
 *			mask and write out the valid bits
 *	
 *	unsigned current();
 *			the result of the last read, cached
 *	unsigned last();
 *			the previous result.
 * 	boolean  changed();
 * 			 if current != last
 *	unsigned next;
 *			a write-pending cache - useful if you build up the
 *			output data a bit at a time.  A subsequent put(void)
 *			will write out this cached value.
 *	void         put(void)          { I2Cextender::write(next);   }
 */

#define NUMPORTS 4

I2Cextender m[NUMPORTS];	// an array of ports...
void setup()
{
    randomSeed(343446);

    for (int x = 0; x < NUMPORTS; x++) {
        m[x] = I2Cextender();
    }
  
    Wire.begin();

    // as you change the NUMPORTS define, the added expanders will automatically be initialized and used
    // by the various loops below...
    switch (NUMPORTS) {
        case 16: m[15].init(7, I2Cextender::PCF8574,  B0); // 0 = output bit, 1=input
        case 15: m[14].init(6, I2Cextender::PCF8574,  B0);
        case 14: m[13].init(7, I2Cextender::PCF8574A, B0);
        case 13: m[12].init(6, I2Cextender::PCF8574A, B0);
        
        case 12: m[11].init(5, I2Cextender::PCF8574,  B0);
        case 11: m[10].init(4, I2Cextender::PCF8574,  B0);
        case 10: m[ 9].init(5, I2Cextender::PCF8574A, B0);
        case 9:  m[ 8].init(4, I2Cextender::PCF8574A, B0);
        
        case 8:  m[ 7].init(3, I2Cextender::PCF8574,  B0);
        case 7:  m[ 6].init(2, I2Cextender::PCF8574,  B0);
        case 6:  m[ 5].init(3, I2Cextender::PCF8574A, B0);
        case 5:  m[ 4].init(2, I2Cextender::PCF8574A, B0);
        
        case 4:  m[ 3].init(1, I2Cextender::PCF8574,  B0);
        case 3:  m[ 2].init(0, I2Cextender::PCF8574,  B0);
        case 2:  m[ 1].init(1, I2Cextender::PCF8574A, B0);
        case 1:  m[ 0].init(0, I2Cextender::PCF8574A, B0);
    }
    
    for (int x = 0; x < NUMPORTS; x++) {
        m[x].put(0); // this causes the port's content to be sent out to the I2C device
    }
} 

void loop() {
    onoff(2);
    cylon(3);
    rand(10000);
}

void cylon(int iterations) {
    for (int iter = 0; iter < iterations; iter++) {
        for (int b = 0; b <= 7; b++) {
            for (int x = 0; x < NUMPORTS; x++) {
                m[x].put( ~(1 << b));
            }
            delay(50);
        }
        for (int b = 7; b >= 0; b--) {
            for (int x = 0; x < NUMPORTS; x++) {
                m[x].put( ~(1 << b));
            }
            delay(50);
        }
        for (int x = 0; x < NUMPORTS; x++) {
          m[x].put(0xFFFF);
        }
        delay(50);
    }
}

void onoff(int iterations) {
    for (int iter = 0; iter <= iterations; iter++) {
        for (int x = 0; x < NUMPORTS; x++) {
            m[x].put(0);
        }
        delay(500);
        for (int x = 0; x < NUMPORTS; x++) {
            m[x].put(0xFFFF);
        }
        delay(500);
    }
}

void rand(int iterations) {
    for (int iter = 0; iter < iterations; iter++) {
	for (int x = 0; x < NUMPORTS; x++) {
  	    int r = random(0,16);
	    if (bitRead(i[x], r)) bitClear(i[x], r); else bitSet(i[x], r);
	    m[x].put(i[x]);
	}
	// delay(5);
    }
}


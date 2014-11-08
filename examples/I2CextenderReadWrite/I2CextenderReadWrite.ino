/*
 * IO Expander demo code inputs and outputs
 *
 * read bits from one bank, write them to the other
 * builds on I2Cextenderdemo
 */

#include "Wire.h"
#include "I2Cextender.h"

#define NUMPORTS 2

I2Cextender m[NUMPORTS];	// an array of ports...
void setup()
{
    for (int x = 0; x < NUMPORTS; x++) {
        m[x] = I2Cextender();
    }
  
    Wire.begin();

    m[ 1].init(1, I2Cextender::PCF8574A, 0xFFFF); // all inputs
    m[ 0].init(0, I2Cextender::PCF8574A, B0);     // all outputs
} 

void loop() {
    m[0].put(m[1].get());
}


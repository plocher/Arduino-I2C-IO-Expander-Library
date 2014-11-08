/*
 * I2Cexpander.h - Library for interfacing to various IO Expander chips
 * Copyright (c) 2014 John Plocher, released under the terms of the MIT License (MIT)
 *
 *  See example programs for usage informtion
 */

#ifndef I2Cextender_h
#define I2Cextender_h

#include "Arduino.h"


class I2Cextender {
public:
    I2Cextender(void);
    void     init(int address, int type, unsigned config);
    int      getSize(void);  
    unsigned read(void) ;
    void     write(unsigned data);
    unsigned get(void)			{ return I2Cextender::read(); }
    void 	 put(unsigned data) { I2Cextender::write(data);   }
    void 	 put(void)          { I2Cextender::write(next);   }
	
	unsigned current();
	unsigned last();
	boolean  changed();
	unsigned next;
    
    enum IOSize {
      B_UNKNOWN =  0,
      B8        =  8,
      B16       = 16,
      T_UNKNOWN =  0
    };
    enum ExpanderType {
      PCA9555   =  1,
      MCP23016  =  2,
      PCF8574   =  3,
      PCF8574A  =  4,
	  ARDIO_A   =  5,	// Bits D0  D1  D2  D3  D4  D5  D6  D7
	  ARDIO_B   =  6,	//      D8  D9  D10 D11 D12 D13 A0  A1
	  ARDIO_C   =  7,	//      A2  A3  A4  A5 - Partial byte... 
	  ARDIO12_A =  8,	//      D0  D1  D5  D6   D2  D9 D10  D11  // ArdioShield LNET 1.2
	  ARDIO12_B =  9,	//      D12 D13 A0  A1   A2  A3 D3   D4   // ArdioShield LNET 1.2
    };
    enum PCA9555Registers {
      REGISTER_INPUT  =  0,
      REGISTER_OUTPUT =  2,
      REGISTER_INVERT =  4,
      REGISTER_CONFIG =  6
    };
    enum BaseAddress {
      base9555     = 0x20,
      base23016    = 0x20,
      base8574A    = 0x38,
      base8574     = 0x20
    };
    
 private:
    int _size;
    int _chip;
    int _address;
    unsigned _config;   
	unsigned _current;
	unsigned _last;
    
    void		input8 (int i2caddr, int dir);
    void		input16(int i2caddr, int dir);
	void		inputA (int dir);
	void		inputB (int dir);
	void		inputC (int dir);
	void		input12A (int dir);
	void		input12B (int dir);
	
    void		write16(int i2caddr, unsigned data);
    void		write8 (int i2caddr, unsigned data);
	void		writeif(int port,    unsigned data, int bit);
    unsigned	read8  (int i2caddr);
    unsigned	read16 (int i2caddr);
    


};

#endif // I2Cextender_h


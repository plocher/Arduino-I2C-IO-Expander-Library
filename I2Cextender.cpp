#include "Wire.h"
#include "I2Cextender.h"

// #define I2C_EXTENDER_DEBUG
// #define I2C_EXTENDER_INVERTLOCAL  - writing a "1" puts port ON (@5v) rather than OFF @0v

I2Cextender::I2Cextender() {
    _address = -1;
    _chip = -1;
    _config = -1;   
    _last = -1;
    _current = -2;
    _size = B_UNKNOWN;
}

void I2Cextender::init(int address, int chip, unsigned config) {
#ifdef I2C_EXTENDER_DEBUG
    Serial.print("I2C:init(");
    Serial.print(address, DEC); Serial.print(", ");
    Serial.print(chip, DEC); Serial.print(", ");
    Serial.print(config, DEC);
    Serial.print(")\n");
#endif  
    _address = address;
    _chip = chip;
    _config = config;
    switch (_chip) {
    case I2Cextender::PCA9555:    _size = B16; input16(base9555  + _address, _config); break;
    case I2Cextender::MCP23016:   _size = B16; input16(base23016 + _address, _config); break;
    case I2Cextender::PCF8574A:   _size = B8;  input8(base8574A  + _address, _config); break;
    case I2Cextender::PCF8574:    _size = B8;  input8(base8574   + _address, _config); break;
    case I2Cextender::ARDIO_A:    _size = B8;  inputA(_config);                        break;
    case I2Cextender::ARDIO_B:    _size = B6;  inputB(_config);                        break;
    case I2Cextender::ARDIO_C:    _size = B6;  inputC(_config);                        break;
    case I2Cextender::ARDIO12_A:  _size = B8;  input12A(_config);                      break;
    case I2Cextender::ARDIO12_B:  _size = B8;  input12B(_config);                      break;
    default:
        _size=0;
    }
}

int I2Cextender::getSize() {
    return _size;
}

unsigned I2Cextender::current() {    
    return I2Cextender::_current;
}
unsigned I2Cextender::last() {    
    return I2Cextender::_last;
}  
boolean I2Cextender::changed() {
    return ((I2Cextender::_current & I2Cextender::_config) != (I2Cextender::_last & I2Cextender::_config));
}   

unsigned I2Cextender::read() {  
    unsigned data = 0;
    int error = 0;
    switch (_chip) {
        case I2Cextender::PCA9555:        data = read16(base9555 + _address);   break;
        case I2Cextender::MCP23016:       data = read16(base23016 + _address);  break;
        case I2Cextender::PCF8574A:       data = read8(base8574A + _address);   break;
        case I2Cextender::PCF8574:        data = read8(base8574  + _address);   break;
        case I2Cextender::ARDIO_A:        
                                          bitWrite(data,0, digitalRead(0));
                                          bitWrite(data,1, digitalRead(1));
#if !defined(__AVR_ATmega32U4__)        // don't mess with I2C pins!
                                          bitWrite(data,2, digitalRead(2));
                                          bitWrite(data,3, digitalRead(3));
#endif
                                          bitWrite(data,4, digitalRead(4));
                                          bitWrite(data,5, digitalRead(5));
                                          bitWrite(data,6, digitalRead(6));
                                          bitWrite(data,7, digitalRead(7));
					break;
        case I2Cextender::ARDIO_B:        
                                          bitWrite(data,0, digitalRead(8));
                                          bitWrite(data,1, digitalRead(9));
                                          bitWrite(data,2, digitalRead(10));
                                          bitWrite(data,3, digitalRead(11));
                                          bitWrite(data,4, digitalRead(12));
                                          bitWrite(data,5, digitalRead(13));
					break;
        case I2Cextender::ARDIO_C:        
                                          bitWrite(data,0, digitalRead(A0));
                                          bitWrite(data,1, digitalRead(A1));
                                          bitWrite(data,2, digitalRead(A2));
                                          bitWrite(data,3, digitalRead(A3));
#if defined(__AVR_ATmega32U4__)         // don't mess with I2C pins!
                                          bitWrite(data,4, digitalRead(A4));
                                          bitWrite(data,5, digitalRead(A5));
#endif
					break;
        
        case I2Cextender::ARDIO12_A:        
                                          bitWrite(data,0, digitalRead(0));
                                          bitWrite(data,1, digitalRead(1));
                                          bitWrite(data,2, digitalRead(5));
                                          bitWrite(data,3, digitalRead(6));
#if !defined(__AVR_ATmega32U4__)        // don't mess with I2C pins!
                                          bitWrite(data,4, digitalRead(2));
#else
                                          bitWrite(data,4, digitalRead(A4));
#endif
                                          bitWrite(data,5, digitalRead(9));
                                          bitWrite(data,6, digitalRead(10));
                                          bitWrite(data,7, digitalRead(11));

					break;
        case I2Cextender::ARDIO12_B:        
                                          bitWrite(data,0, digitalRead(12));
                                          bitWrite(data,1, digitalRead(13));
                                          bitWrite(data,2, digitalRead(A0));
                                          bitWrite(data,3, digitalRead(A1));
                                        
                                          bitWrite(data,4, digitalRead(A2));
                                          bitWrite(data,5, digitalRead(A3));
#if !defined(__AVR_ATmega32U4__)        // don't mess with I2C pins!
                                          bitWrite(data,6, digitalRead(3));
#else
                                          bitWrite(data,6, digitalRead(A5));
#endif
                                          bitWrite(data,7, digitalRead(4));

					break;
        default:                          error = 1;                            break;
    } 
    if (!error) {      
        I2Cextender::_last = I2Cextender::_current;
        I2Cextender::_current = data;
    } 
#ifdef I2C_EXTENDER_DEBUG
    Serial.print("I2C:read(a=");
    Serial.print(_address, DEC); Serial.print(", chip=");
    Serial.print(_chip, DEC); Serial.print(", conf=");
    Serial.print(_config, DEC);
    Serial.print(") => "); 
    if (error)              Serial.print("Error"); 
    else if (_size == B8)   Serial.print((byte)data, BIN); 
    else if (_size == B16)  Serial.print((unsigned)data, BIN); 
    else                   {Serial.print("unknown data size: "); Serial.print(data, BIN);}
    Serial.print("\n");  
#endif
    return data;
}
                       
// only write a bit if the config regiaster allows writing to it
void I2Cextender::writeif(int port, unsigned data, int bit) {
    unsigned mybit = bitRead(data, bit);
#ifdef I2C_EXTENDER_INVERTLOCAL
    mybit = !mybit;
#endif
#ifdef I2C_EXTENDER_DEBUG
    Serial.print("    I2C:writeif(p=");
    Serial.print(port, DEC); Serial.print(", data=");
    Serial.print((byte)data, BIN); Serial.print(", bit=");
    Serial.print(bit, DEC);
    Serial.print(") => ");
    Serial.print(mybit, BIN);
    Serial.print("\n");
    digitalWrite(port, mybit);
#else   
    if (bitRead(_config, bit) == 0) digitalWrite(port, mybit);
#endif
}
void I2Cextender::write(unsigned data) {
#ifdef I2C_EXTENDER_DEBUG
    Serial.print("I2C:write(a=");
    Serial.print(_address, DEC); Serial.print(", chip=");
    Serial.print(_chip, DEC); Serial.print(", conf=");
    Serial.print(_config, DEC);
    Serial.print(") => "); 
    Serial.print((byte)data, BIN); 
    Serial.print("\n");
#endif
    
    switch (_chip) {
    case I2Cextender::PCA9555:        write16(base9555  + _address, data | _config); break; 
    case I2Cextender::MCP23016:       write16(base23016 + _address, data | _config); break; 
    case I2Cextender::PCF8574A:       write8 (base8574A + _address, data | _config); break;
    case I2Cextender::PCF8574:        write8 (base8574  + _address, data | _config); break;
    case I2Cextender::ARDIO_A:        
                                      writeif( 0, data, 0);
                                      writeif( 1, data, 1);
#if !defined(__AVR_ATmega32U4__)    // don't mess with I2C pins!
                                      writeif( 2, data, 2);
                                      writeif( 3, data, 3);
#endif
                                      writeif( 4, data, 4);
                                      writeif( 5, data, 5);
                                      writeif( 6, data, 6);
                                      writeif( 7, data, 7);
                                                                                    break;
    case I2Cextender::ARDIO_B:        
                                      writeif( 8, data, 0);
                                      writeif( 9, data, 1);
                                      writeif(10, data, 2);
                                      writeif(11, data, 3);
                                      writeif(12, data, 4);
                                      writeif(13, data, 5);
                                                                                    break;
    case I2Cextender::ARDIO_C:        
                                      writeif(A0, data, 0);
                                      writeif(A1, data, 1);
                                      writeif(A2, data, 2);
                                      writeif(A3, data, 3);
#if defined(__AVR_ATmega32U4__)     // don't mess with I2C pins!
                                      writeif(A4, data, 4);
                                      writeif(A5, data, 5);
#endif
                                                                                    break;

    case I2Cextender::ARDIO12_A:        
                                      writeif( 0, data, 0);
                                      writeif( 1, data, 1);
                                      writeif( 5, data, 2);
                                      writeif( 6, data, 3);

#if !defined(__AVR_ATmega32U4__)    // don't mess with I2C pins!
                                      writeif( 2, data, 4);
#else
                                      writeif(A4, data, 4);
#endif
                                      writeif( 9, data, 5);
                                      writeif(10, data, 6);
                                      writeif(11, data, 7);
                                                                                    break;

    case I2Cextender::ARDIO12_B:        
                                      writeif(12, data, 0);
                                      writeif(13, data, 1);
                                      writeif(A0, data, 2);
                                      writeif(A1, data, 3);
                                    
                                      writeif(A2, data, 4);
                                      writeif(A3, data, 5);
#if !defined(__AVR_ATmega32U4__)    // don't mess with I2C pins!
                                      writeif( 3, data, 6);
#else
                                      writeif(A5, data, 6);
#endif
                                      writeif( 4, data, 7);
                                                                                    break;
    default:                                                                        break;
    }
    //I2Cextender::_last = I2Cextender::_current;                                   
    //I2Cextender::_current = data | _config;
}

unsigned I2Cextender::read8(int i2caddr) {
    unsigned _data = -1;
    // Wire.beginTransmission(i2caddr);
    Wire.requestFrom(i2caddr, 1);
    if(Wire.available()) {
      _data = Wire.read();
    }
    // Wire.endTransmission();  
    return _data;
}

void I2Cextender::write8(int i2caddr, unsigned data)
{ 
    Wire.beginTransmission(i2caddr);
    Wire.write(0xff & data);
    Wire.endTransmission();  
}

void I2Cextender::input8(int i2caddr, int dir) {
}

// set Arduino I/O pin direction (1=OUTPUT, 0-INPUT)
void I2Cextender::inputA(int dir) {
    pinMode(0,  bitRead(dir, 0) ? INPUT : OUTPUT);
    pinMode(1,  bitRead(dir, 1) ? INPUT : OUTPUT);
#if !defined(__AVR_ATmega32U4__)    // don't mess with I2C pins!
    pinMode(2,  bitRead(dir, 2) ? INPUT : OUTPUT);
    pinMode(3,  bitRead(dir, 3) ? INPUT : OUTPUT);
#endif
    pinMode(4,  bitRead(dir, 4) ? INPUT : OUTPUT);
    pinMode(5,  bitRead(dir, 5) ? INPUT : OUTPUT);
    pinMode(6,  bitRead(dir, 6) ? INPUT : OUTPUT);
    pinMode(7,  bitRead(dir, 7) ? INPUT : OUTPUT);
}
void I2Cextender::inputB(int dir) {
    pinMode(8,  bitRead(dir, 0) ? INPUT : OUTPUT);
    pinMode(9,  bitRead(dir, 1) ? INPUT : OUTPUT);
    pinMode(10, bitRead(dir, 2) ? INPUT : OUTPUT);
    pinMode(11, bitRead(dir, 3) ? INPUT : OUTPUT);

    pinMode(12, bitRead(dir, 4) ? INPUT : OUTPUT);
    pinMode(13, bitRead(dir, 5) ? INPUT : OUTPUT);
}
void I2Cextender::inputC(int dir) {
    pinMode(A0, bitRead(dir, 0) ? INPUT : OUTPUT);
    pinMode(A1, bitRead(dir, 1) ? INPUT : OUTPUT);
    pinMode(A2, bitRead(dir, 2) ? INPUT : OUTPUT);
    pinMode(A3, bitRead(dir, 3) ? INPUT : OUTPUT);
#if defined(__AVR_ATmega32U4__)     // don't mess with I2C pins!
    pinMode(A4, bitRead(dir, 4) ? INPUT : OUTPUT);
    pinMode(A5, bitRead(dir, 5) ? INPUT : OUTPUT);
#endif
}

// set Arduino I/O pin direction (1=OUTPUT, 0-INPUT)
void I2Cextender::input12A(int dir) {
    pinMode(0,  bitRead(dir, 0) ? INPUT : OUTPUT);
    pinMode(1,  bitRead(dir, 1) ? INPUT : OUTPUT);
    pinMode(5,  bitRead(dir, 2) ? INPUT : OUTPUT);
    pinMode(6,  bitRead(dir, 3) ? INPUT : OUTPUT);
    
#if !defined(__AVR_ATmega32U4__)    // don't mess with I2C pins!
    pinMode(2,  bitRead(dir, 4) ? INPUT : OUTPUT);
#else
    pinMode(A4, bitRead(dir, 4) ? INPUT : OUTPUT);
#endif
    pinMode(9,  bitRead(dir, 5) ? INPUT : OUTPUT);
    pinMode(10, bitRead(dir, 6) ? INPUT : OUTPUT);
    pinMode(11, bitRead(dir, 7) ? INPUT : OUTPUT);
}
void I2Cextender::input12B(int dir) {
    pinMode(12, bitRead(dir, 0) ? INPUT : OUTPUT);
    pinMode(13, bitRead(dir, 1) ? INPUT : OUTPUT);
    pinMode(A0, bitRead(dir, 2) ? INPUT : OUTPUT);
    pinMode(A1, bitRead(dir, 3) ? INPUT : OUTPUT);

    pinMode(A2, bitRead(dir, 4) ? INPUT : OUTPUT);
    pinMode(A3, bitRead(dir, 5) ? INPUT : OUTPUT);

#if !defined(__AVR_ATmega32U4__)    // don't mess with I2C pins!
    pinMode(3,  bitRead(dir, 6) ? INPUT : OUTPUT);
#else
    pinMode(A5, bitRead(dir, 6) ? INPUT : OUTPUT);
#endif
    pinMode(4,  bitRead(dir, 7) ? INPUT : OUTPUT);
}

void I2Cextender::input16(int i2caddr, int dir) {
    Wire.beginTransmission(i2caddr);
    Wire.write(REGISTER_CONFIG);
    Wire.write(0xff & dir);  // low byte
    Wire.write(dir >> 8);    // high byte
    Wire.endTransmission();  
}

void I2Cextender::write16(int i2caddr, unsigned data) {
    Wire.beginTransmission(i2caddr);
    Wire.write(REGISTER_OUTPUT);
    Wire.write(0xff & data);  //  low byte
    Wire.write(data >> 8);    //  high byte
    Wire.endTransmission();  
}

unsigned I2Cextender::read16(int i2caddr) {
    unsigned data = 0;
    Wire.beginTransmission(i2caddr);
    Wire.write(REGISTER_INPUT);
    Wire.endTransmission();  
    // Wire.beginTransmission(i2caddr);
    Wire.requestFrom(i2caddr, 2);
    if(Wire.available()) {
        data = Wire.read();
    }
    if(Wire.available()) {
        data |= (Wire.read() << 8);
    }
    // Wire.endTransmission();  
    return data;
}




#ifndef TDC7200_H
#define TDC7200_H

// parts of this code taken from https://github.com/TAPR/TICC/tree/master/TICC

#define SPIfrequency (uint32_t) 2500000
#define EXTCLOCK (uint32_t) 1000000 //Value of external CLOCK for 
#define AVG_CYCLES 10

// TDC7200 register addresses
const int CONFIG1 =		0x00;           // default 0x00
const int CONFIG2 =		0x01;           // default 0x40
const int INT_STATUS =		0x02;           // default 0x00
const int INT_MASK =		0x03;           // default 0x07
const int COARSE_CNTR_OVF_H =	0x04;       	// default 0xff
const int COARSE_CNTR_OVF_L =	0x05;       	// default 0xff
const int CLOCK_CNTR_OVF_H =	0x06;        	// default 0xff
const int CLOCK_CNTR_OVF_L =	0x07;        	// default 0xff
const int CLOCK_CNTR_STOP_MASK_H = 0x08;	// default 0x00
const int CLOCK_CNTR_STOP_MASK_L = 0x09;	// default 0x00
// gap from 0x0A thru 0x0F...
const int TIME1 =		0x10;           // default 0x00_0000
const int CLOCK_COUNT1 =	0x11;           // default 0x00_0000
const int TIME2	=		0x12;           // default 0x00_0000
const int CLOCK_COUNT2 =	0x13;           // default 0x00_0000
const int TIME3	=		0x14;           // default 0x00_0000
const int CLOCK_COUNT3 =	0x15;           // default 0x00_0000
const int TIME4	=		0x16;           // default 0x00_0000
const int CLOCK_COUNT4 =	0x17;           // default 0x00_0000
const int TIME5	=		0x18;           // default 0x00_0000
const int CLOCK_COUNT5 =	0x19;           // default 0x00_0000
const int TIME6	=		0x1A;           // default 0x00_0000
const int CALIBRATION1 =	0x1B;           // default 0x00_0000
const int CALIBRATION2 =	0x1C;           // default 0x00_0000



// Chip properties:
// Most Significant Bit is clocked first (MSBFIRST)
// clock is low when idle
// data is clocked on the rising edge of the clock (seems to be SPI_MODE0)
// max clock speed: 20 mHz

class tdc7200 {
  private:
    float clockperiod = 1.0 / EXTCLOCK;
  
  public:
    uint32_t time1 = 0; 
    uint32_t time2 = 0;
    uint32_t time3 = 0;
    uint32_t time4 = 0;
    uint32_t time5 = 0;
    uint32_t time6 = 0;
    
    uint32_t clockcount1 = 0;
    uint32_t clockcount2 = 0;
    uint32_t clockcount3 = 0;
    uint32_t clockcount4 = 0;
    uint32_t clockcount5 = 0;
    
    uint32_t cal1 = 0;
    uint32_t cal2 = 0;

    float tof1 = 0;
    float tof2 = 0;
    float tof3 = 0;
    float tof4 = 0;
    float tof5 = 0;
    
    //READ 8bit Register
    byte readReg8(byte address) {
      byte inByte = 0;
    
      SPI.beginTransaction(SPISettings(SPIfrequency, MSBFIRST, SPI_MODE0));
      // take the chip select low to select the device:
      digitalWrite(53, LOW);
    
      SPI.transfer(address & 0x1f);
      inByte = SPI.transfer(address);
    
      digitalWrite(53, HIGH);
      SPI.endTransaction();
    
      return inByte;
    }

    //READ 24bit Register
    uint32_t readReg24(byte address) {
      uint32_t long value = 0;
    
      // CSB needs to be toggled between 24-bit register reads
      SPI.beginTransaction(SPISettings(SPIfrequency, MSBFIRST, SPI_MODE0));
        digitalWrite(53, LOW);
      
        SPI.transfer(address & 0x1f);
      
        uint16_t msb = SPI.transfer(address);
        uint16_t mid = SPI.transfer(address);
        uint16_t lsb = SPI.transfer(address);
      
        value = (msb << 16) + (mid << 8) + lsb;
      
        digitalWrite(53, HIGH);
      SPI.endTransaction();
      delayMicroseconds(5);
      return value;
    }

    //WRITE REGISTER
    void writeReg(byte address, byte value) {
      uint16_t test = 0;  
      // take the chip select low to select the device:
      SPI.beginTransaction(SPISettings(SPIfrequency, MSBFIRST, SPI_MODE0));
        digitalWrite(53, LOW);
        // Force Address bit 6 to one for a write
        SPI.transfer16((address | 0x40) << 8 | value);
        test = (address | 0x40) << 8 | value;
        //Serial.print("-----test = "); Serial.println(test, BIN);
        digitalWrite(53, HIGH);
      SPI.endTransaction();
    } 

    //COMPUTE TOFs
    float getTOF(uint32_t timeN, uint32_t timeN1, uint32_t clockcountN, uint32_t cal1, uint32_t cal2) {
      float calCount = (cal2 - cal1) / (AVG_CYCLES - 1.0);
      //Serial.print("clockperiod = "); Serial.println(clockperiod,8);
      //Serial.print("calCount = "); Serial.println(calCount);
      float normLSB = (clockperiod / calCount);
      //Serial.print("normLSB = "); Serial.println(normLSB,16);
      //float fehler = timeN-timeN1
      float tof_temp = normLSB * (float(timeN) - (float)timeN1) + clockcountN * clockperiod;
      //Serial.print("tof_temp = "); Serial.println(tof_temp, 16);
      return tof_temp;
    }
    
};

#endif	/* TDC7200_H */

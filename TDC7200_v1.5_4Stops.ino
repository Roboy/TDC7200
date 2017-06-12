#include <SPI.h>

int startpin = 4;
int stoppin = 7;
int triggerpin = 8;


struct data {
  byte temp[3];
  uint32_t val;
};

data time1, time2, time3, time4, time5; //TIME1-4
data cc1, cc2, cc3, cc4;  //CLOCKCOUNT1-4
data calibr1, calibr2;  //CALIBRATION1 & 2

void setup() {
    //Initialize SPI 
  SPI.begin(); //ChipSelect = 53
  Serial.begin(9600);
  
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
 
  pinMode(2, INPUT);
  pinMode(8, INPUT);
  
  digitalWrite(3, LOW);
  delay(10); //Sensor might need time to set up ?
  digitalWrite(3, HIGH);
}

void loop() {
  byte config1;
  byte config2;

  //WRITE 00000011 INTO CONFIG 1 TO GET READY FOR MEASUREMENT
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); 
    digitalWrite(53, LOW);
    SPI.transfer(B01000000); // Write into register (CONFIG1) address 00h
    SPI.transfer(B00000011); //write 00000011 into CONFIG1
    digitalWrite(53, HIGH);

      //WRITE B01000011 INTO CONFIG2 for FOUR STOPS
           //If you want to define more other amount of stops do this here
    digitalWrite(53, LOW);
    SPI.transfer(B01000001); // WRITE
    SPI.transfer(B01000011); //write 00000011 into CONFIG2
    digitalWrite(53, HIGH);
  
     //READ CONFIG2 which has value (B01000000) as Default should be B01000011 for FOUR STOPS
    Serial.print("CONFIG2 = ");
    digitalWrite(53, LOW);
    SPI.transfer(B00000001);
    config2 = SPI.transfer(0x01); // get inhalt
    Serial.println(config2, BIN);
    digitalWrite(53, HIGH); 
  SPI.endTransaction();


  delay(1);

    //WAIT for Trigger Signal == 1
  while(digitalRead(triggerpin)) {
      break;
  }
    //SEND START
  digitalWrite(startpin, HIGH);
  digitalWrite(startpin, LOW);

  delayMicroseconds(100);
  
  //SEND 4 STOPS
  for(int i = 0; i < 4; i++) {
    digitalWrite(stoppin, HIGH);
    digitalWrite(stoppin, LOW);
    delayMicroseconds(100);  
  }

//READ REGISTERS
  SPI.beginTransaction(SPISettings(1000000, MSBFIRST, SPI_MODE0)); 
      //READ TIME1 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x10);
      time1.temp[0] = SPI.transfer(0x10); 
      time1.temp[1] = SPI.transfer(0x10);
      time1.temp[2] = SPI.transfer(0x10);  
    digitalWrite(53, HIGH);

      //READ CLOCK_COUNT1 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x11);
      cc1.temp[0] = SPI.transfer(0x11); 
      cc1.temp[1] = SPI.transfer(0x11);
      cc1.temp[2] = SPI.transfer(0x11);  
    digitalWrite(53, HIGH);

          //READ TIME2 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x12);
      time2.temp[0] = SPI.transfer(0x12); 
      time2.temp[1] = SPI.transfer(0x12);
      time2.temp[2] = SPI.transfer(0x12);  
    digitalWrite(53, HIGH);

      //READ CLOCK_COUNT2 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x13);
      cc2.temp[0] = SPI.transfer(0x13); 
      cc2.temp[1] = SPI.transfer(0x13);
      cc2.temp[2] = SPI.transfer(0x13);  
    digitalWrite(53, HIGH);

          //READ TIME3 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x14);
      time3.temp[0] = SPI.transfer(0x14); 
      time3.temp[1] = SPI.transfer(0x14);
      time3.temp[2] = SPI.transfer(0x14);  
    digitalWrite(53, HIGH);

      //READ CLOCK_COUNT3 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x15);
      cc3.temp[0] = SPI.transfer(0x15); 
      cc3.temp[1] = SPI.transfer(0x15);
      cc3.temp[2] = SPI.transfer(0x15);  
    digitalWrite(53, HIGH);

          //READ TIME4 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x16);
      time4.temp[0] = SPI.transfer(0x16); 
      time4.temp[1] = SPI.transfer(0x16);
      time4.temp[2] = SPI.transfer(0x16);  
    digitalWrite(53, HIGH);

      //READ CLOCK_COUNT4 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x17);
      cc4.temp[0] = SPI.transfer(0x17); 
      cc4.temp[1] = SPI.transfer(0x17);
      cc4.temp[2] = SPI.transfer(0x17);  
    digitalWrite(53, HIGH);

      //READ TIME5 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x18);
      time5.temp[0] = SPI.transfer(0x18); 
      time5.temp[1] = SPI.transfer(0x18);
      time5.temp[2] = SPI.transfer(0x18);  
    digitalWrite(53, HIGH);
  
      //READ CALIBRATION1 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x1B);
      calibr1.temp[0] = SPI.transfer(0x1B);
      calibr1.temp[1] = SPI.transfer(0x1B);
      calibr1.temp[2] = SPI.transfer(0x1B);
    digitalWrite(53, HIGH);
  
      //READ CALIBRATION2 Register
    digitalWrite(53, LOW);
      SPI.transfer(0x1C);
      calibr2.temp[0] = SPI.transfer(0x1C);
      calibr2.temp[1] = SPI.transfer(0x1C);
      calibr2.temp[2] = SPI.transfer(0x1C);
    digitalWrite(53, HIGH);
  SPI.endTransaction();
//END OF READ REGISTERS

  //FORMAT BYTES FROM REGISTERS TO INTS
  time1.val = ((time1.temp[0]<<16)|(time1.temp[1]<<8)|time1.temp[2]);
  cc1.val = ((cc1.temp[0]<<16)|(cc1.temp[1]<<8)|cc1.temp[2]);
  time2.val = ((time2.temp[0]<<16)|(time2.temp[1]<<8)|time2.temp[2]);
  cc2.val = ((cc2.temp[0]<<16)|(cc2.temp[1]<<8)|cc2.temp[2]);
  time3.val = ((time3.temp[0]<<16)|(time3.temp[1]<<8)|time3.temp[2]);
  cc3.val = ((cc3.temp[0]<<16)|(cc3.temp[1]<<8)|cc3.temp[2]);
  time4.val = ((time4.temp[0]<<16)|(time4.temp[1]<<8)|time4.temp[2]);
  cc4.val = ((cc4.temp[0]<<16)|(cc4.temp[1]<<8)|cc4.temp[2]);
  time5.val = ((time5.temp[0]<<16)|(time5.temp[1]<<8)|time5.temp[2]);
  
  calibr1.val = ((calibr1.temp[0]<<16)|(calibr1.temp[1]<<8)|calibr1.temp[2]);
  calibr2.val = ((calibr2.temp[0]<<16)|(calibr2.temp[1]<<8)|calibr2.temp[2]);


  //PRINTS FOR DEBUG
  Serial.print("TIME1 = "); Serial.println(time1.val);
  Serial.print("CLOCK_COUNT1 = "); Serial.println(cc1.val);
  Serial.print("TIME2 = "); Serial.println(time2.val);
  Serial.print("CLOCK_COUNT2 = "); Serial.println(cc2.val);
  Serial.print("TIME3 = "); Serial.println(time3.val);
  Serial.print("CLOCK_COUNT3 = "); Serial.println(cc3.val);
  Serial.print("TIME4 = "); Serial.println(time4.val);
  Serial.print("CLOCK_COUNT4 = "); Serial.println(cc4.val);
  Serial.print("CALIBRATION1 = "); Serial.println(calibr1.val);
  Serial.print("CALIBRATION2 = "); Serial.println(calibr2.val);

  double clockperiod = 1/2E6; //CLOCK 2MHz
  Serial.print("clockperiod = ");
  Serial.println(clockperiod, 16);
  double calCount = (calibr2.val - calibr1.val) / 9.0;
  Serial.print("calCount = ");
  Serial.println(calCount);
  double normLSB = clockperiod / calCount;
  Serial.print("normLSB = ");
  Serial.println(normLSB, 16);

  double TOF1 = normLSB * ( (double)time1.val - (double)time2.val ) + (double)cc1.val * clockperiod;
  double TOF2 = normLSB * ( (double)time1.val - (double)time3.val ) + (double)cc2.val * clockperiod;
  double TOF3 = normLSB * ( (double)time1.val - (double)time4.val ) + (double)cc3.val * clockperiod;
  double TOF4 = normLSB * ( (double)time1.val - (double)time5.val ) + (double)cc4.val * clockperiod;

  
  
  Serial.print("TOF1 = ");
  Serial.println(TOF1, 8);
  Serial.print("TOF2 = ");
  Serial.println(TOF2, 8);
  Serial.print("TOF3 = ");
  Serial.println(TOF3, 8);
  Serial.print("TOF4 = ");
  Serial.println(TOF4, 8);

  //INTERRUPT SHOULD BE SENT AT END OF MEASUREMENT
  if(digitalRead(2)) {
    Serial.println("Interupt");
  }
  
}

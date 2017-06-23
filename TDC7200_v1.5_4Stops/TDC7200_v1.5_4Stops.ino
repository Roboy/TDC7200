#include <SPI.h>
#include "tdc7200.h"



int startpin = 4;
int stoppin = 7;
int triggerpin = 8;
const byte CLOCKOUT = 11; //CLOCK ON MEGA2560
byte config1, config2, int_status;
byte int_mask, ccoh, ccol, clockcoh, clockcol, clockcstopmaskh, clockcstopmaskl;



void setup() {
  
  // Initialize 1 MHz clock on CLOCKOUT (OC1A)
  pinMode (CLOCKOUT, OUTPUT); 
  // set up Timer 1
  TCCR1A = bit (COM1A0);  // toggle OC1A on Compare Match
  TCCR1B = bit (WGM12) | bit (CS10);   // CTC, no prescaling
  OCR1A =  7;       // output every cycle
  
    //Initialize SPI 
  SPI.begin(); //chip1Select = 53
    //Initialize Serial
  Serial.begin(19200);
    //Initialize Outputs
  pinMode(3, OUTPUT);
  digitalWrite(3, LOW);
  pinMode(4, OUTPUT);
  digitalWrite(4, LOW);
  pinMode(7, OUTPUT);
  digitalWrite(7, LOW);
    //Initialize INPUTS
  pinMode(2, INPUT);
  pinMode(8, INPUT);
    //STARTUP OF TDC7200
  digitalWrite(3, LOW);
  delay(5); //Sensor needs 300microseconds to startup, 1.5ms recommended
  digitalWrite(3, HIGH);
  delay(5);

}

void loop() {
  tdc7200 chip1;

  //WRITE 00000011 INTO CONFIG 1 TO GET READY FOR MEASUREMENT1
  chip1.writeReg(CONFIG1, B00000011);
  //WRITE B01000011 INTO CONFIG2 for FOUR STOPS
            // 000: Single Stop
            // 001: Two Stops
            // 010: Three Stops
            // 011: Four Stops
            // 100: Five Stops
  chip1.writeReg(CONFIG2, B01000100);
  //READ IF CONFIG1/2 CORRECT
  config1 = chip1.readReg8(CONFIG1);
  config2 = chip1.readReg8(CONFIG2);
  
    //WAIT for Trigger Signal == 1
  while(digitalRead(triggerpin)) {
      break;
  }
    //SEND START
  digitalWrite(startpin, HIGH);
  digitalWrite(startpin, LOW);

  delayMicroseconds(100);
  
  //SEND 5 STOPS
  for(int i = 0; i < 4; i++) {
    digitalWrite(stoppin, HIGH);
    digitalWrite(stoppin, LOW);
    delayMicroseconds(100);  
  }
  delay(8);
  digitalWrite(stoppin, HIGH);
  digitalWrite(stoppin, LOW);


//READ REGISTERS AND COMPUTE VALUES
  chip1.time1 = chip1.readReg24(TIME1);
  chip1.time2 = chip1.readReg24(TIME2);
  chip1.time3 = chip1.readReg24(TIME3);
  chip1.time4 = chip1.readReg24(TIME4);
  chip1.time5 = chip1.readReg24(TIME5);
  chip1.time6 = chip1.readReg24(TIME6);
  chip1.clockcount1 = chip1.readReg24(CLOCK_COUNT1);
  chip1.clockcount2 = chip1.readReg24(CLOCK_COUNT2);
  chip1.clockcount3 = chip1.readReg24(CLOCK_COUNT3);
  chip1.clockcount4 = chip1.readReg24(CLOCK_COUNT4);
  chip1.clockcount5 = chip1.readReg24(CLOCK_COUNT5);
  chip1.cal1 = chip1.readReg24(CALIBRATION1);
  chip1.cal2 = chip1.readReg24(CALIBRATION2);  
//END OF READ REGISTERS


  chip1.tof1 = chip1.getTOF(chip1.time1, chip1.time2, chip1.clockcount1, chip1.cal1, chip1.cal2);
  chip1.tof2 = chip1.getTOF(chip1.time1, chip1.time3, chip1.clockcount2, chip1.cal1, chip1.cal2);
  chip1.tof3 = chip1.getTOF(chip1.time1, chip1.time4, chip1.clockcount3, chip1.cal1, chip1.cal2);
  chip1.tof4 = chip1.getTOF(chip1.time1, chip1.time5, chip1.clockcount4, chip1.cal1, chip1.cal2);
  chip1.tof5 = chip1.getTOF(chip1.time5, chip1.time6, chip1.clockcount5, chip1.cal1, chip1.cal2);
  
  //TESTPRINT CONFIG1/2
  //Serial.print("CONFIG1 = "); Serial.println(config1, BIN);
  //Serial.print("CONFIG2 = "); Serial.println(config2, BIN);

  Serial.println();
  Serial.print("  ****************");
  Serial.println();
  Serial.print("* TOF1 = ");
  Serial.print(chip1.tof1,8); Serial.println(" *");
  Serial.print("* TOF2 = ");
  Serial.print(chip1.tof2,8); Serial.println(" *");
  Serial.print("* TOF3 = ");
  Serial.print(chip1.tof3,8); Serial.println(" *");
  Serial.print("* TOF4 = ");
  Serial.print(chip1.tof4,8); Serial.println(" *");
  Serial.print("* TOF5 = ");
  Serial.print(chip1.tof5,8); Serial.println(" *"); 
  Serial.print("  ****************");
  Serial.println();



  //INTERRUPT SHOULD BE SENT AT END OF MEASUREMENT
        //BUT CHIP DOESNT DO THAT EVEN IF MEASUREMENT COMPLETED
  if(digitalRead(2)) {
    Serial.println("Interupt");
  }
  //delay(8); //LIGHTHOUSE TRACKING RESTARTS EVERY 8ms
}

/*
  //READ/WRITE NEW_MEASUREMENT into Interrupt
  int_status = chip1.readReg8(INT_STATUS);
  int_mask = chip1.readReg8(INT_MASK);
  ccoh = chip1.readReg8(COARSE_CNTR_OVF_H);
  ccol = chip1.readReg8(COARSE_CNTR_OVF_L);
  clockcoh = chip1.readReg8(CLOCK_CNTR_OVF_H);
  clockcol = chip1.readReg8(CLOCK_CNTR_OVF_L);
  clockcstopmaskh = chip1.readReg8(CLOCK_CNTR_STOP_MASK_H);
  clockcstopmaskl = chip1.readReg8(CLOCK_CNTR_STOP_MASK_L);
  
  Serial.print("INT_STATUS = "); Serial.println(int_status,BIN);
  Serial.print("INT_MASK = "); Serial.println(int_mask,BIN);
  Serial.print("COARSE_CNTR_OVF_H = "); Serial.println(ccoh,BIN);
  Serial.print("COARSE_CNTR_OVF_L = "); Serial.println(ccol,BIN);
  Serial.print("CLOCK_CNTR_OVF_H = "); Serial.println(clockcoh,BIN);
  Serial.print("COARSE_CNTR_OVF_L = "); Serial.println(clockcol,BIN);
  Serial.print("COARSE_CNTR_OVF_L = "); Serial.println(clockcstopmaskh,BIN);
  Serial.print("COARSE_CNTR_OVF_L = "); Serial.println(clockcstopmaskl,BIN);

  
  Serial.println(); Serial.println();


  //PRINTS FOR DEBUG
  Serial.print("TIME1 = "); Serial.println(chip1.time1);
  Serial.print("CLOCK_COUNT1 = "); Serial.println(chip1.clockcount1);
  Serial.print("TIME2 = "); Serial.println(chip1.time2);
  Serial.print("CLOCK_COUNT2 = "); Serial.println(chip1.clockcount2);
  Serial.print("TIME3 = "); Serial.println(chip1.time3);
  Serial.print("CLOCK_COUNT3 = "); Serial.println(chip1.clockcount3);
  Serial.print("TIME4 = "); Serial.println(chip1.time4);
  Serial.print("CLOCK_COUNT4 = "); Serial.println(chip1.clockcount4);
  Serial.print("TIME5 = "); Serial.println(chip1.time5);
  //Serial.print("CLOCK_COUNT5 = "); Serial.println(chip1.clockcount5);
  //Serial.print("TIME6 = "); Serial.println(chip1.time6);
  Serial.print("CALIBRATION1 = "); Serial.println(chip1.cal1);
  Serial.print("CALIBRATION2 = "); Serial.println(chip1.cal2);
*/


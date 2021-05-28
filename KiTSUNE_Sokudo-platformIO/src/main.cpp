/*
KiTSUNE - Sokudo Ver.1.0
By WOLFNEST Studio

IG : @redwolf_studio    |   @kitsune_film

used ATMEGA328P ONLY!!
Libraries requirement
  - LiquidCrystal_I2C by Frank de Brabander

*/

#include <Arduino.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include <Wire.h>
#include <SPI.h>

#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C LCD(0x26, 20, 4);

uint8_t resetButt = 3;

unsigned long StartTime, StopTime, DeltaTime;
float ShutterTime1, ShutterTime2;

volatile boolean shutterFlag = false, signalReading = false;

void setup() {
  LCD.init();
  LCD.backlight();          //Trun on backlight
  LCD.setCursor(0, 0);
  
  //Pin setup
  //interrupts INT0
  DDRD &= ~(1 << DDD2);     //Set PD2 pin to INPUT
  //Interrupts setup 
  EICRA |= (1 << ISC00);    //Set INT0 to trigger on any logic change
  EIMSK |= (1 << INT0);     //Enable INT0
  sei();                    //Enable Interrupts

  
  pinMode(resetButt, INPUT_PULLUP);

  LCD.clear();
  LCD.setCursor(7, 1);
  LCD.print("READY!");
}//Void setup

void loop() {

  //Manual reset Value
  if(digitalRead(resetButt) == LOW) {
    //RESET logic and value
    shutterFlag = false;
    signalReading = false;
    DeltaTime = 0;
    StartTime = 0;
    StopTime = 0;
    ShutterTime1 = 0;
    ShutterTime2 = 0;

    LCD.clear();
    LCD.setCursor(3, 1);
    LCD.print("RESET COMPLET!");
    delay(1500);
    LCD.clear();
    LCD.setCursor(7, 1);
    LCD.print("READY!");
  }

  //Show test results 
  if(signalReading == true) {

    DeltaTime = (StopTime - StartTime);
    ShutterTime1 = (float)DeltaTime/1000000;
    ShutterTime2 = 1/ShutterTime1;

    LCD.clear();
    LCD.setCursor(0, 0);
    LCD.print("** SPEED  SHUTTER **");
    LCD.setCursor(2, 2);
    if(ShutterTime1 >= 1) {
      LCD.print(ShutterTime1, 2);

    } else if(ShutterTime1 < 1) {
      LCD.print("1/");
      LCD.print(ShutterTime2, 2);
    }

    //RESET logic and value
    shutterFlag = false;
    signalReading = false;
    DeltaTime = 0;
    StartTime = 0;
    StopTime = 0;
    ShutterTime1 = 0;
    ShutterTime2 = 0;
  }
}//Void Loop


ISR (INT0_vect) {
  // interrupts code
  if(shutterFlag == false) {
    StartTime = micros();
    shutterFlag = true;
  } else{
    StopTime = micros();
    signalReading = true;
  }
}//ISR
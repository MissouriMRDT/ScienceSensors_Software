#ifndef SRA_SENSORS
#define SRA_SENSORS
#include <stdio.h> 
#include <stdlib.h>
#include "RoveComm.h"
#include <SPI.h>

#define UVLED_ENABLE_PIN A4

#define Computer_Serial Serial
#define RoveCom_Serial Serial3
#define O2_Serial Serial2
#define CO2_Serial Serial7
#define CH4_Serial Serial1
#define NO2_SCL 13
#define NO2_SDA 12
#define NO2_CS 10
#define NO_Ref A17
#define NO_Gas A16


#define ADC_CLK 1600000
#define ref_voltage 5
#define adc_resolution 4096

#define Photodiode1 A11
#define Photodiode2 A10
#define Photodiode3 A9

#define reverseCurrent pow(10,-15)
#define darkCurrent 0 //this is wrong but I don't think there's a way to get this number without measurement

#define Laser1 A2
#define Laser2 A1
#define Laser3 A0

#define Lights A5

String readO2Bytes(int len); //O2 Sensor output to string
void co2Reading(); //Takes ppm reading from the co2 sensor and sends to rovecomm
void o2Reading(); //Takes a  a %vol reading from o2 sensor, converts to ppm and sends to rovecomm
void ch4Reading(); //Takes a %vol reading from ch4 sensor, converts to ppm and sends to rovecomm
void no2Reading(); //Takes adc voltage reading from no2 sensor, converts to voltage and sends to rovecomm
void noReading(); //Takes voltage reading from no sensor and sends to rovecomm
void pdReading(); //Takes voltage reading from photodiodes, converts to current and sends to rovecomm
void updateLed(int msg); //Toggles UV Led based on message from rovecomm


#endif

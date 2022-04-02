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
#define NO2_SCL 13
#define NO2_SDA 12
#define NO2_CS 10


#define ADC_CLK 1600000
#define ref_voltage 5
#define adc_resolution 4096

#define Photodiode1 A11
#define Photodiode2 A10
#define Photodiode3 A9

#define Laser1 A2
#define Laser2 A1
#define Laser3 A0

#define Lights A5

String readO2Bytes(int len); //O2 Sensor output to string
void co2Reading(); //Takes ppm reading from the co2 sensor and sends to rovecomm
void o2Reading(); //Takes a temperture and ppmo2 reading and sends to rovecomm
void updateLed(int msg); //Toggles UV Led based on message from rovecomm


#endif

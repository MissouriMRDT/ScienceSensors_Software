#ifndef SRA_SENSORS
#define SRA_SENSORS
#include <stdio.h> 
#include <stdlib.h>
#include "RoveComm.h"
#include <SPI.h>

#define UVLED_ENABLE_PIN PM_6

#define Computer_Serial Serial
#define RoveCom_Serial Serial3
#define O2_Serial Serial5
#define CO2_Serial Serial7
#define NO2_SCL PB_2
#define NO2_SDA PB_3
#define NO2_CS PK_0


#define ADC_CLK 1600000
#define ref_voltage 5
#define adc_resolution 4096

#define Photodiode1 PE_0
#define Photodiode2 PE_1
#define Photodiode3 PE_2

#define Laser1 PK_7
#define Laser2 PK_6
#define Laser3 PH_1

#define Lights PQ_1

String readO2Bytes(int len); //O2 Sensor output to string
void co2Reading(); //Takes ppm reading from the co2 sensor and sends to rovecomm
void o2Reading(); //Takes a temperture and ppmo2 reading and sends to rovecomm
void updateLed(int msg); //Toggles UV Led based on message from rovecomm


#endif

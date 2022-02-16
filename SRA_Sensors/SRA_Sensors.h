#ifndef SRA_SENSORS
#define SRA_SENSORS
#include <stdio.h> 
#include <stdlib.h>
#include "RoveComm.h"

#define UVLED_ENABLE_PIN PM_6

#define Computer_Serial Serial
#define RoveCom_Serial Serial3
#define O2_Serial Serial7
#define CO2_Serial Serial5

#define Photodiode1 PE_0
#define Photodiode2 PE_1
#define Photodiode3 PE_2

#define Laser1 C2_8
#define Laser2 C2_9
#define Laser3 C2_10

#define Lights PQ_1

String readO2Bytes(int len); //O2 Sensor output to string
void co2Reading(); //Takes ppm reading from the co2 sensor and sends to rovecomm
void o2Reading(); //Takes a temperture and ppmo2 reading and sends to rovecomm
void updateLed(int msg); //Toggles UV Led based on message from rovecomm


#endif

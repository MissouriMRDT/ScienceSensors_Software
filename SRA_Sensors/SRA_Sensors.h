#ifndef SRA_SENSORS
#define SRA_SENSORS
#include <stdio.h>
#include <stdlib.h>
#include "RoveComm.h"
#include <SPI.h>

// Serial assignments
#define COMPUTER_SERIAL Serial
#define ROVECOMM_SERIAL Serial3
#define O2_SERIAL Serial2
#define CO2_SERIAL Serial1
#define CH4_SERIAL Serial6

// Pin assignments
#define CCD_MCS A2
#define CCD_OS A3
#define CCD_ICG A16
#define CCD_SH A15
#define NO2_CS CS
#define NO2_SCK SCK
#define NO2_MISO MISO

#define UVLED_260 A4
#define UVLED_275 A5
#define UVLED_280 A6
#define UVLED_310 A7

// Variable definitions
#define ADC_CLK 1600000
#define MIN_ADC 0
#define MAX_ADC 4095
#define MIN_NO_PPM 100
#define MAX_NO_PPM 30000

#define CO2_BAUD 19200
#define O2_BAUD 9600
#define CH4_BAUD 38400

#define TELEM_TIMER 150000

bool o2ReadingOkay;
float o2readings;

bool co2ReadingOkay;
short co2reading;

bool ch4Available;
float ch4reading;

float nh3Value;

float no2reading;

// Function definitions
String readO2Bytes(int len); // O2 Sensor output to string
void co2Read();           // Takes ppm reading from the co2 sensor and sends to rovecomm
void o2Read();            // Takes a  a %vol reading from o2 sensor, converts to ppm and sends to rovecomm
void ch4Read();           // Takes a voltage reading from ch4 sensor and sends to rovecomm
void ch4StartMeasurement();
void ch4init();
void no2Read();           // Takes voltage reading from no2 sensor, converts to ppb and sends to rovecomm
void nh3Read();           // Takes voltage reading from nh3 sensor and sends to rovecomm
void fluroRead();         // Reads CCDs and sends to rovecomm
void telemetry();

void LEDControl();           // Controls UV LEDs
void microControl();         // Controls Microscope servo

RoveCommEthernet RoveComm;
rovecomm_packet packet;
EthernetServer TCPServer(RC_ROVECOMM_SCIENCESENSORSBOARD_PORT);

IntervalTimer Telemetry;

#endif

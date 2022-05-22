#ifndef SRA_SENSORS
#define SRA_SENSORS
#include <stdio.h>
#include <stdlib.h>
#include "RoveComm.h"
#include <SPI.h>
#include "USBHost_t36.h"

#define UVLED_ENABLE_PIN A4

#define COMPUTER_SERIAL Serial
#define ROVECOMM_SERIAL Serial3
#define O2_SERIAL Serial2
#define CO2_SERIAL Serial7
#define CH4_SERIAL Serial1
#define NO2_SCL 13
#define NO2_SDA 12
#define NO2_CS 10
#define NO_REF A17
#define NO_GAS A16

#define ADC_CLK 1600000
#define MIN_ADC 0
#define MAX_ADC 4095
#define MIN_NO_PPM 100
#define MAX_NO_PPM 30000

#define CO2_BAUD 19200
#define O2_BAUD 9600
#define USERIAL_BAUD 115200

#define TELEM_TIMER 150000

#define Photodiode1 A11
#define Photodiode2 A10
#define Photodiode3 A9

#define Laser1 A2
#define Laser2 A1
#define Laser3 A0

#define Lights A5

String readO2Bytes(int len); // O2 Sensor output to string
void co2Reading();           // Takes ppm reading from the co2 sensor and sends to rovecomm
void o2Reading();            // Takes a  a %vol reading from o2 sensor, converts to ppm and sends to rovecomm
void ch4Reading();           // Takes a %vol reading from ch4 sensor, converts to ppm and sends to rovecomm
void noReading();            // Takes voltage reading from no sensor and sends to rovecomm
void telemetry();

USBHost myusb;
USBSerial userial(myusb);
RoveCommEthernet RoveComm;
rovecomm_packet packet;
EthernetServer TCPServer(RC_ROVECOMM_SCIENCESENSORSBOARD_PORT);

IntervalTimer Telemetry;

#endif

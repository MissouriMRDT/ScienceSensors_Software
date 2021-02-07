#ifndef SRA_SENSORS
#define SRA_SENSORS

#define UVLED_ENABLE_PIN 58

#define Computer_Serial Serial
#define RoveCom_Serial Serial3
#define O2_Serial Serial4
#define Methane_Serial Serial5
#define CO2_Serial Serial7

String readO2Bytes(int len); //O2 Sensor output to string
void co2Reading(); //Takes ppm reading from the co2 sensor and sends to rovecomm
void o2Reading(); //Takes a temperture and ppmo2 reading and sends to rovecomm
void updateLed(int msg); //Toggles UV Led based on message from rovecomm


#endif

#include <Adafruit_Sensor.h>

//Code: Austin Christman
//SRA Software

//Libraries
#include "DHT_U.h" //AM2302 Sensor .h files
#include "DHT.h"
#include "SHT1x.h"// SHT-10 Sensor .h files
#include <math.h>
#include "RoveComm.h"

//Fuction Declarlations
void readSHT10();
void readAM2302();
void readMQ4();
void indicateSensorErrorSHT10();//float moisture, float temperature_soil
void indicateSensorErrorAM2302();//float humidity_atm, float temperature_atm
void runSpectrometer();

//pin layout for Tiva C w/ tm4c129
#define SOIL_DATA_PIN          PE_5// Pin located in section X_9:A1, pin PE5
#define CCD_INT_PIN            PE_0// Pin located in section X_9:B1, pin PE0
#define METHANE_VOUT_PIN       PE_2// Pin located in section X_9:B1, pin PE2
#define ATM_DOUT_PIN           PN_2// Pin located in section X_9:D1, pin PN2
#define SW_FLAG1_PIN           PP_4// Pin located in section X_7:A2, pin PP4
#define SW_FLAG2_PIN           PN_5// Pin located in section X_7:A2, pin PN5
#define SW_ERROR_PIN           PN_4// Pin located in section X_7:A2, pin PN4
#define SOIL_SCK_PIN           PB_5// Pin located in seciotn X_7:B2, pin PB5
#define CCD_OS_PIN             PK_3// Pin located in section X_7:B2, pin PK3
#define UV_TOGGLE_PIN          PP_5// Pin located in section X_7:D2, pin PP5
#define DHTTYPE DHT22              // DHT 22 sensor/AM2302 sensor

//Variable Declarations
uint16_t SRADATA[RC_SRASENSORSBOARD_SENSORDATA_DATACOUNT];
int sensorValue = 0;
float humidity_atm = 0;
float temperature_atm = 0;
int methanePpm;
float temperature_soil = 0;
float moisture = 0;

//Rovecomm
RoveCommEthernetUdp Rovecomm;
rovecomm_packet packet;
#define ROVECOMM_DELAY    5        //Delay of 5ms

SHT1x th_sensor(SOIL_DATA_PIN, SOIL_SCK_PIN);
DHT dht(ATM_DOUT_PIN, DHTTYPE);

void setup() 
{
  Serial.begin(9600);
  dht.begin();
  Rovecomm.begin(RC_SRASENSORSBOARD_FOURTHOCTET);// SRA board IP: 138
  delay(ROVECOMM_DELAY);
  
  pinMode(SW_ERROR_PIN, OUTPUT);
  pinMode(SW_FLAG1_PIN, OUTPUT);
  pinMode(SW_FLAG2_PIN,OUTPUT);
  pinMode(METHANE_VOUT_PIN,INPUT);
  pinMode(ATM_DOUT_PIN,INPUT);
  pinMode(SOIL_SCK_PIN, OUTPUT);
  pinMode(SOIL_DATA_PIN, INPUT);
  
  //inizalise all output pins outputs and input.
  digitalWrite(SW_ERROR_PIN, LOW);  
  digitalWrite(SW_FLAG1_PIN, LOW);  
  digitalWrite(SW_FLAG2_PIN, LOW);
  digitalWrite(SOIL_SCK_PIN, LOW); 

  Serial.println("Setup Complete");
}

void loop() 
{ 
  readAM2302();
  indicateSensorErrorAM2302();//humidity_atm, temperature_atm
  
  readMQ4();
  
  readSHT10();
  indicateSensorErrorSHT10();
  
  SRADATA[RC_SRASENSORSBOARD_SENSORDATA_AIRTENTRY] = temperature_atm;
  SRADATA[RC_SRASENSORSBOARD_SENSORDATA_AIRMENTRY] = humidity_atm;
  SRADATA[RC_SRASENSORSBOARD_SENSORDATA_SOILTENTRY] = temperature_soil;
  SRADATA[RC_SRASENSORSBOARD_SENSORDATA_SOILMENTRY] = moisture;
  SRADATA[RC_SRASENSORSBOARD_SENSORDATA_METHANEENTRY] = methanePpm;
  
  packet = Rovecomm.read();
    if(packet.data_id!=0)
    {
      Serial.println(packet.data_id);
      Serial.println(packet.data_count);
      for(int i = 0; i<packet.data_count; i++)
      {
        Serial.print(packet.data[i]);
      }//end for
      switch(packet.data_id)
      {
        case RC_SRASENSORSBOARD_SPECTROMETERRUN_DATAID:
        {
          runSpectrometer();
          break;
        }
        case RC_SRASENSORSBOARD_UVLEDENABLE_DATAID:
        {
          toggleUvLed();
          break;
        }
      }
    }
  Rovecomm.write(RC_SRASENSORSBOARD_SENSORDATA_HEADER, SRADATA);
  delay(ROVECOMM_DELAY); //add rovecome delay here  X
}
void readSHT10()//SHT-10 Sensor code:
{
  moisture = th_sensor.readHumidity();
  temperature_soil = th_sensor.readTemperatureC();
  Serial.print("Temperature: ");
  Serial.print(temperature_soil);
  Serial.print(" C, Humidity: ");
  Serial.print(moisture);
  Serial.println(" ");
}
void readAM2302()//AM2302 Code for Temp and Humidity.
{
  humidity_atm = dht.readHumidity();
  temperature_atm = dht.readTemperature();
  Serial.print(F("Humidity: "));
  Serial.print(humidity_atm);
  Serial.print(F("% Temperature: "));
  Serial.print(temperature_atm);
  Serial.print(F(" C "));
  Serial.println((""));
}
void readMQ4()//MQ-4 Code Start
{
  delay(600); //Long delay will slow down code immensly
  digitalWrite(METHANE_VOUT_PIN, HIGH);
  methanePpm = map(analogRead(METHANE_VOUT_PIN), 0, 1023, 300, 10000);//300 - 10,000 are in parts per million.(units)
  Serial.print(methanePpm, DEC);
  Serial.println(" Parts per million, Methane");
}
void indicateSensorErrorSHT10()//float moisture, float temperature_soil
{
  if((isnan(temperature_soil) && isnan(moisture)))
  {
    //Serial.println(("Failed to read from the SHT10 sensor!"));
    digitalWrite(SW_ERROR_PIN, HIGH);
    delay (600);
    digitalWrite(SW_ERROR_PIN,LOW);
    delay (300);
    return;
  }
}
void indicateSensorErrorAM2302()//float humidity_atm, float temperature_atm
{
  if((isnan(temperature_atm) && isnan(humidity_atm)))
  {
    //Serial.println(("Failed to read from the AM2302 sensor!"));
    digitalWrite(SW_ERROR_PIN, HIGH);
    delay (600);
    digitalWrite(SW_ERROR_PIN,LOW);
    delay (300);
    return;
  }
}
void runSpectrometer()
{
  Serial.println("Running Spectometer");

  return;
}
void toggleUvLed()
{
  bool uvEnable;
  uvEnable = packet.data[0];
  if(uvEnable == RC_SRASENSORSBOARD_UVLEDENABLE_ENABLED)
  {
    digitalWrite(UV_TOGGLE_PIN, HIGH);
  }
  else
  {
    digitalWrite(UV_TOGGLE_PIN, LOW);
  }
}
/*
Create more void functions for each sesnors
define variables for each sensor globally
Rove comm telemetry with constants, SRADATA numbers
*/

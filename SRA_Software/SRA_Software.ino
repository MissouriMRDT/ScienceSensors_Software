#include <Adafruit_Sensor.h>

//Code: Austin Christman
//SRA Software

//.h Files

#include "DHT_U.h" //AM2302 Sensor .h files
#include "DHT.h"
#include "SHT1x.h"// SHT-10 Sensor .h files
#include <math.h>
#include "RoveComm.h"

//pin layout for Tiva C w/ tm4c129
#define soil_Data PE_5    // Pin located in section X_9:A1, pin PE5
#define ccd_Int PE_0      // Pin located in section X_9:B1, pin PE0
#define methane_Vout PE_2 // Pin located in section X_9:B1, pin PE2
#define atm_Dout PN_2     // Pin located in section X_9:D1, pin PN2
#define sw_Flag1 PP_4     // Pin located in section X_7:A2, pin PP4
#define sw_Flag2 PN_5     // Pin located in section X_7:A2, pin PN5
#define sw_Error PN_4     // Pin located in section X_7:A2, pin PN4
#define soil_Sck PB_5     // Pin located in seciotn X_7:B2, pin PB5
#define ccd_OS PK_3       // Pin located in section X_7:B2, pin PK3
#define uv_Toggle PP5     // Pin located in section X_7:D2, pin PP5
#define DHTTYPE DHT22     // DHT 22 sensor/AM2302 sensor

int sensorValue = 0;
int methanePpm;

//Rovecomm
RoveCommEthernetUdp Rovecomm;

SHT1x th_sensor(soil_Data, soil_Sck);
DHT dht(atm_Dout, DHTTYPE);

void setup() {
  Serial.begin(9600);
  Rovecomm.begin(138);// SRA board IP: 138
  
  Serial.println("Sensors Begin");
  pinMode(sw_Error, OUTPUT);
  
  dht.begin();
}
void loop() {
  delay(2000);
  digitalWrite(methane_Vout, HIGH);
//AM2302 Code for Temp and Humidity.
  float humidity_atm= dht.readHumidity();
  float temperature_atm= dht.readTemperature();
  indicateSensorErrorAM2302(humidity_atm, temperature_atm);
 
  Serial.print(F("Humidity: "));
  Serial.print(humidity_atm);
  Serial.print(F("% Temperature: "));
  Serial.print(temperature_atm);
  Serial.print(F(" C "));
  Serial.println((""));

//MQ-4 Code Start:
  sensorValue = analogRead(methane_Vout);
  methanePpm = map(methane_Vout, 0, 1023, 300, 10000);//300 - 10,000 are in parts per million.(units)
  
  Serial.print(methanePpm, DEC);
  Serial.print(" Parts per million, Methane");
  Serial.println("");

//SHT-10 Sensor code:
  float temperature_soil = 0;
  float moisture = 0;

  moisture = th_sensor.readHumidity();
  temperature_soil = th_sensor.readTemperatureC();
  //indicateSensorErrorSHT10(moisture,temperature_soil);
  
  Serial.print("Temperature: ");
  Serial.print(temperature_soil);
  Serial.print(" C, Humidity: ");
  Serial.print(moisture);
  Serial.println(" ");

  uint16_t SRADATA[5];
  SRADATA[0]=temperature_atm;
  SRADATA[1]=humidity_atm;
  SRADATA[2]=moisture;
  SRADATA[3]=temperature_soil;
  SRADATA[4]= methanePpm;
  Rovecomm.read();
  Rovecomm.write(RC_SRASENSORSBOARD_SENSORDATA_DATAID, RC_SRASENSORSBOARD_SENSORDATA_DATACOUNT, SRADATA);
}

/*void indicateuv_Toggle ()
{
  digitalWrite(, HIGH);
  return;
}
void indicateccd_OS ()
{
  digitalWrite(,HIGH);
  return;
}
*/
void indicateSensorErrorAM2302(float humidity_atm, float temperature_atm)
{
  if((isnan(temperature_atm) && isnan(humidity_atm)))
  {
    //Serial.println(("Failed to read from the AM2302 sensor!"));
    digitalWrite(sw_Error, HIGH);
    delay (600);
    digitalWrite(sw_Error,LOW);
    delay (300);
    return;
  }
}

/*
void indicateSensorErrorSHT10(float moisture, float temperature_soil)
{
  if((isnan(temperature_soil) && isnan(moisture)))
  {
    //Serial.println(("Failed to read from the SHT10 sensor!"));
    digitalWrite(sw_Error, HIGH);
    delay (600);
    digitalWrite(sw_Error,LOW);
    delay (300);
    return;
  }
  */

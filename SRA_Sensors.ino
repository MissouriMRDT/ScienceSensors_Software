#include "RoveComm.h"
//Commands//////////////////////////////////////////////////////////////////////////////////////////////
//Spectrometer Run
#define RC_SRASENSORSBOARD_SPECTROMETERRUN_DATAID       00+_TYPE_COMMAND+_SRASENSORS_BOARDNUMBER
#define RC_SRASENSORSBOARD_SPECTROMETERRUN_DATATYPE     uint8_t 
#define RC_SRASENSORSBOARD_SPECTROMETERRUN_DATACOUNT    1   
#define RC_SRASENSORSBOARD_SPECTROMETERRUN_HEADER   RC_SRASENSORSBOARD_SPECTROMETERRUN_DATAID,RC_SRASENSORSBOARD_SPECTROMETERRUN_DATACOUNT
#define RC_SRASENSORSBOARD_SPECTROMETERRUN_ENABLED    1
#define RC_SRASENSORSBOARD_SPECTROMETERRUN_DISABLED   0

//UV LED Enable
#define RC_SRASENSORSBOARD_UVLEDENABLE_DATAID       01+_TYPE_COMMAND+_SRASENSORS_BOARDNUMBER
#define RC_SRASENSORSBOARD_UVLEDENABLE_DATATYPE     uint8_t 
#define RC_SRASENSORSBOARD_UVLEDENABLE_DATACOUNT    1   
#define RC_SRASENSORSBOARD_UVLEDENABLE_HEADER   RC_SRASENSORSBOARD_UVLEDENABLE_DATAID,RC_SRASENSORSBOARD_UVLEDENABLE_DATACOUNT
#define RC_SRASENSORSBOARD_UVLEDENABLE_ENABLED    1
#define RC_SRASENSORSBOARD_UVLEDENABLE_DISABLED   0

//Telemetry//////////////////////////////////////////////////////////////////////////////////////////////
//Sensor Data
#define RC_SRASENSORSBOARD_SENSORDATA_DATAID      00+_TYPE_TELEMETRY+_SRASENSORS_BOARDNUMBER
#define RC_SRASENSORSBOARD_SENSORDATA_DATATYPE    uint16_t  
#define RC_SRASENSORSBOARD_SENSORDATA_DATACOUNT   5   //[AirT, AirM, SoilT, SoilM, Methane] 
#define RC_SRASENSORSBOARD_SENSORDATA_HEADER    RC_SRASENSORSBOARD_SENSORDATA_DATAID,RC_SRASENSORSBOARD_SENSORDATA_DATACOUNT
#define RC_SRASENSORSBOARD_SENSORDATA_AIRTENTRY   0
#define RC_SRASENSORSBOARD_SENSORDATA_AIRMENTRY   1
#define RC_SRASENSORSBOARD_SENSORDATA_SOILTENTRY  2
#define RC_SRASENSORSBOARD_SENSORDATA_SOILMENTRY  3
#define RC_SRASENSORSBOARD_SENSORDATA_METHANEENTRY  4

String readO2Bytes(int len); //O2 Sensor output to string
uint16_t output[RC_SRASENSORSBOARD_SENSORDATA_DATACOUNT]; //Output array to rovecomm
RoveCommEthernetUdp RoveComm;
void setup(){
  //start serial connection
  Serial.begin(9600);//Computer
  Serial7.begin(19200);//CO2
  Serial3.begin(115200);//RoveCom
  Serial4.begin(9600);//O2
  Serial7.begin(19200);//CO2
  //Start RoveComm
  RoveComm.begin(RC_SRASENSORSBOARD_FOURTHOCTET);
  pinMode(58,OUTPUT); //Setup UvLed
  delay(100);
}

void loop(){
  //Send request to co2 sensor for data
  delay(100);
  Serial7.write(0xFF);
  Serial7.write(0xFE);
  Serial7.write(0x02);
  Serial7.write(0x02);
  Serial7.write(0x03);
  delay(20);
  
  //Read data from co2 sensor
  //First three reads are headers
  Serial7.read();
  Serial7.read();
  Serial7.read();
  byte msb=Serial7.read(); //Sensor data is sent in two bytes
  byte lsb=Serial7.read();
  short reading=(msb<<8)|(lsb&0xff);//Combine the data
  
  if(reading!=-1)//If we got co2 reading then output
  {
    output[0]=(int)reading;
    Serial.println((int)reading);
  }

  for(int i=0;i<25;i++)
  {
    if(Serial4.read()=='O') //Start at the beginning of the o2 sensor output
    {
      readO2Bytes(1);
      double ppo2 =  strtod(readO2Bytes(6).c_str(),NULL);
      readO2Bytes(4); 
      double temperature = strtod(readO2Bytes(4).c_str(),NULL);
      readO2Bytes(3);
      double pressure = strtod(readO2Bytes(4).c_str(),NULL);
      readO2Bytes(3);
      double o2Percent = strtod(readO2Bytes(6).c_str(),NULL);
      readO2Bytes(11);
      if(temperature!=0&&o2Percent!=0)
      {
        output[1]=temperature;
        output[2]=ppo2;
      }
    }
  }
  
  //Check if uv led needs to be turned on or off
  rovecomm_packet packet = RoveComm.read();
  if(packet.data_id!=0)
  {
    for(int i = 0; i<packet.data_count; i++)
    {
      Serial.println((int)packet.data[0]);
    }
    if(packet.data_id==RC_SRASENSORSBOARD_UVLEDENABLE_DATAID)
    {
      if((int)packet.data[0]==RC_SRASENSORSBOARD_UVLEDENABLE_ENABLED)
      {
        digitalWrite(58,HIGH);
        Serial.println("Setting High");
      }
      else
      {
        digitalWrite(58,LOW);
        Serial.println("Setting Low");
      }
    }
  }

  //Send sensor info to RoveComm
  RoveComm.write(RC_SRASENSORSBOARD_SENSORDATA_DATAID,RC_SRASENSORSBOARD_SENSORDATA_DATACOUNT,output);  
  delay(100);
  
}

//Returns a string of the next "len" bytes read from 02 sensor
String readO2Bytes(int len)
{
  String output="";
  for(int i=0;i<len;i++)
    output+=(char)Serial4.read();
   return output;
}

#include <stdio.h> 
#include <stdlib.h>
#include "RoveComm.h"
#include "SRA_Sensors.h"

int timing; //Variable the keeps count to make sure sensor data is only sent every second

RoveCommEthernetUdp RoveComm;
void setup(){
  timing = 0;//Initialize the timing variable
  
  //start serial connection
  Computer_Serial.begin(9600);//Computer
  CO2_Serial.begin(19200);//CO2
  RoveCom_Serial.begin(115200);//RoveCom
  O2_Serial.begin(9600);//O2
  Methane_Serial.begin(115200); //Methane
  
  //Start RoveComm
  RoveComm.begin(RC_SRASENSORSBOARD_FOURTHOCTET);
  
  pinMode(UVLED_ENABLE_PIN,OUTPUT); //Setup UvLed
  
  delay(100);
}

void loop(){
  //Read Sensor data only every second
  if(timing%10==0)
  {
    co2Reading();
    o2Reading();
  }

  if(timing%210==0)
  {
    
  }
  
  //Read from RoveComm
  rovecomm_packet packet = RoveComm.read();
  if(packet.data_id!=0)
  {
    switch (packet.data_id)
    {
      case RC_SRASENSORSBOARD_UVLEDENABLE_DATAID: //Switch UVLED on or off
        updateLed((int)packet.data[0]);
        break;
    }
  }
  timing++; //Increment the timer //TODO: Create constants for polling rates?
  delay(100);
}

void updateLed(int msg)
{
  if(msg==RC_SRASENSORSBOARD_UVLEDENABLE_ENABLED)
  {
    digitalWrite(UVLED_ENABLE_PIN,HIGH);
    Computer_Serial.println("Setting High");
  }
  else
  {
    digitalWrite(UVLED_ENABLE_PIN,LOW);
    Computer_Serial.println("Setting Low");
  }
}
void o2Reading(){
  for(int i=0;i<25;i++)
  {
    if(O2_Serial.read()=='O') //Start at the beginning of the o2 sensor output
    {
      float o2readings[4];
      readO2Bytes(1);
      o2readings[0] =  strtof(readO2Bytes(6).c_str(),NULL); //Partial Pressure in mBar?
      readO2Bytes(4); 
      o2readings[1] = strtof(readO2Bytes(4).c_str(),NULL); //Temperature in celsius
      readO2Bytes(3);
      o2readings[2] = strtof(readO2Bytes(4).c_str(),NULL); //Pressure in mBar?
      readO2Bytes(3);
      o2readings[3] = strtof(readO2Bytes(6).c_str(),NULL)*10000; //Concentration - read in percent, converted to ppm
      readO2Bytes(11);

      RoveComm.write(RC_SRASENSORSBOARD_O2_DATA_DATAID,RC_SRASENSORSBOARD_O2_DATA_DATACOUNT, o2readings);
      break;
    }
  }
}

void co2Reading(){
  //Send request to co2 sensor for data
  CO2_Serial.write(0xFF);
  CO2_Serial.write(0xFE);
  CO2_Serial.write(0x02);
  CO2_Serial.write(0x02);
  CO2_Serial.write(0x03);
  delay(20);
  
  //Read data from co2 sensor
  //First three reads are headers
  CO2_Serial.read();
  CO2_Serial.read();
  CO2_Serial.read();
  byte msb=CO2_Serial.read(); //Sensor data is sent in two bytes
  byte lsb=CO2_Serial.read();
  short reading=(msb<<8)|(lsb&0xff);//Combine the data
  
  if(reading!=-1)//If we got co2 reading then output
  {
    RoveComm.write(RC_SRASENSORSBOARD_CO2_DATA_DATAID,RC_SRASENSORSBOARD_CO2_DATA_DATACOUNT,(float)reading);
  }
}

//Returns a string of the next "len" bytes read from 02 sensor
String readO2Bytes(int len)
{
  String output="";
  for(int i=0;i<len;i++)
    output+=(char)O2_Serial.read();
   return output;
}

#include <stdio.h> 
#include <stdlib.h>
#include "RoveComm.h"


String readO2Bytes(int len); //O2 Sensor output to string
void co2Reading(); //Takes ppm reading from the co2 sensor and sends to rovecomm
void o2Reading(); //Takes a temperture and ppmo2 reading and sends to rovecomm
void updateLed(int msg); //Toggles UV Led based on message from rovecomm

int timing; //Variable the keeps count to make sure sensor data is only sent every second

RoveCommEthernetUdp RoveComm;
void setup(){
  timing = 0;//Initialize the timing variable
  
  //start serial connection
  Serial.begin(9600);//Computer
  Serial7.begin(19200);//CO2
  Serial3.begin(115200);//RoveCom
  Serial4.begin(9600);//O2
  
  //Start RoveComm
  RoveComm.begin(RC_SRASENSORSBOARD_FOURTHOCTET);
  
  pinMode(58,OUTPUT); //Setup UvLed
  
  delay(100);
}

void loop(){
  //Read Sensor data only every second
  if(timing%10==0)
  {
    co2Reading();
    o2Reading();
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
  timing++; //Increment the timer
  delay(100);
}

void updateLed(int msg)
{
  if(msg==RC_SRASENSORSBOARD_UVLEDENABLE_ENABLED)
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
void o2Reading(){
  for(int i=0;i<25;i++)
  {
    if(Serial4.read()=='O') //Start at the beginning of the o2 sensor output
    {
      readO2Bytes(1);
      float ppo2 =  strtof(readO2Bytes(6).c_str(),NULL);
      readO2Bytes(4); 
      float temperature = strtof(readO2Bytes(4).c_str(),NULL);
      readO2Bytes(3);
      float pressure = strtof(readO2Bytes(4).c_str(),NULL);
      readO2Bytes(3);
      float o2Percent = strtof(readO2Bytes(6).c_str(),NULL);
      readO2Bytes(11);
      if(temperature!=0&&o2Percent!=0)
      {
        RoveComm.write(RC_SRASENSORSBOARD_AIR_TEMPERATURE_DATA_DATAID,RC_SRASENSORSBOARD_AIR_TEMPERATURE_DATA_DATACOUNT,temperature);
        RoveComm.write(RC_SRASENSORSBOARD_O2_CONCENTRATION_DATA_DATAID,RC_SRASENSORSBOARD_O2_CONCENTRATION_DATA_DATACOUNT,ppo2);
        break;
      }
    }
  }
}

void co2Reading(){
  //Send request to co2 sensor for data
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
    RoveComm.write(RC_SRASENSORSBOARD_CO2_CONCENTRATION_DATA_DATAID,RC_SRASENSORSBOARD_CO2_CONCENTRATION_DATA_DATACOUNT,(float)reading);
  }
}

//Returns a string of the next "len" bytes read from 02 sensor
String readO2Bytes(int len)
{
  String output="";
  for(int i=0;i<len;i++)
    output+=(char)Serial4.read();
   return output;
}

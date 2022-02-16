#include "SRA_Sensors.h"

int timing;   //variable to keep count

RoveCommEthernet RoveComm; //what is this?
rovecomm_packet packet;

EthernetServer TCPServer(RC_ROVECOMM_SCIENCESENSORSBOARD_PORT);

void setup(){
  timing = 0;   //Initialize the timing variable

  Computer_Serial.begin(9600);
  CO2_Serial.begin(19200);
  O2_Serial.begin(9600);  

  //start RoveComm
  RoveComm.begin(RC_SCIENCESENSORSBOARD_FOURTHOCTET, &TCPServer);

  pinMode(UVLED_ENABLE_PIN,OUTPUT); //setup UVLED

  delay(100);
}

void loop(){
  //Read sensor data only every second
  if(timing%10==0)
  {
    o2Reading();
    co2Reading();
  }

  //Read from RoveComm
  packet = RoveComm.read();
  if(packet.data_id != 0)
  {
    switch(packet.data_id)
    {
      case RC_SCIENCESENSORSBOARD_LIGHTS_DATA_ID:    //Switch UVLED on or off
        updateLed((int)packet.data[0]);
        break;
    }
  }
  timing++;   //Increment the timer
  delay(100);
}

void updateLed(int msg)
{
  if(msg==RC_SCIENCESENSORSBOARD_LIGHTS_DATA_ID)
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

void o2Reading()
{
  for(int i=0;i<25;i++)
  {
    if(O2_Serial.read()=='O') //Start at the beginning of the o2 sensor output
    {
      float o2readings[1];
      readO2Bytes(25);
      o2readings[0] = strtof(readO2Bytes(6).c_str(),NULL)*10000; //Concentration - read in percent, converted to ppm
      readO2Bytes(11);

      RoveComm.write(RC_SCIENCESENSORSBOARD_O2_DATA_ID,RC_SCIENCESENSORSBOARD_O2_DATA_COUNT, o2readings);
      break;
    }
  }
}

void co2Reading()
{
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
    RoveComm.write(RC_SCIENCESENSORSBOARD_CO2_DATA_ID,RC_SCIENCESENSORSBOARD_CO2_DATA_COUNT,(float)reading);
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
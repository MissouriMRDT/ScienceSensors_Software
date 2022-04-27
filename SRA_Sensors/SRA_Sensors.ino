#include "SRA_Sensors.h"

USBHost myusb;
USBSerial userial(myusb);
RoveCommEthernet RoveComm; //what is this?
rovecomm_packet packet;
EthernetServer TCPServer(RC_ROVECOMM_SCIENCESENSORSBOARD_PORT);

IntervalTimer Telemetry;

void setup()
{
  CO2_Serial.begin(19200);
  O2_Serial.begin(9600);
  myusb.begin();
  userial.begin(115200);

  //start RoveComm
  RoveComm.begin(RC_SCIENCESENSORSBOARD_FOURTHOCTET, &TCPServer, RC_ROVECOMM_SCIENCESENSORSBOARD_MAC);

  pinMode(UVLED_ENABLE_PIN,OUTPUT); //setup UVLED
  pinMode(Laser1,OUTPUT);
  pinMode(Laser2,OUTPUT);
  pinMode(Laser3,OUTPUT);

  digitalWrite(Laser1, LOW);
  digitalWrite(Laser2, LOW);
  digitalWrite(Laser3, LOW);
  Telemetry.begin(telemetry,1500000);
}

void loop()
{
  //Read sensor data only every second
  //myusb.Task();

  //Read from RoveComm
  packet = RoveComm.read();
  if(packet.data_id != 0)
  {
    switch(packet.data_id)
    {
      case RC_SCIENCESENSORSBOARD_LIGHTS_DATA_ID:    //Switch UVLED on or off
        //updateLed((int)packet.data[0]);
        break;
      case RC_SCIENCESENSORSBOARD_FLASERS_DATA_ID:
        uint8_t* lasersOn = (uint8_t*)packet.data;
        if(lasersOn[0] != 0)
        {
          digitalWrite(Laser1, HIGH);
          digitalWrite(Laser2, HIGH);
          digitalWrite(Laser3, HIGH); 
        }
    }
  }
}

void telemetry()
{
    o2Reading();
    co2Reading();
    noReading();
    //no2Reading();
    ch4Reading();
    //pdReading();
}
void updateLed(int msg)
{/*
  if(msg==RC_SCIENCESENSORSBOARD_LIGHTS_DATA_ID)
  {
    digitalWrite(UVLED_ENABLE_PIN,HIGH);
    Computer_Serial.println("Setting High");
  }
  else
  {
    digitalWrite(UVLED_ENABLE_PIN,LOW);
    Computer_Serial.println("Setting Low");
  }*/
}

void pdReading()
{/*
  float pdreadings[3];

  //gets adc values from sensor output
  uint16_t raw1 = analogRead(Photodiode1);
  uint16_t raw2 = analogRead(Photodiode2);
  uint16_t raw3 = analogRead(Photodiode3);

  // get voltage values
  float v1 = raw1 * ref_voltage / adc_resolution;
  float v2 = raw2 * ref_voltage / adc_resolution;
  float v3 = raw3 * ref_voltage / adc_resolution;

  Serial.println((float)v1);
  Serial.println((float)v2);
  Serial.println((float)v3);

  //get current values from voltage values from formula
  pdreadings[0] = reverseCurrent*(exp(-(50/3) * v1)-1) - darkCurrent; 
  pdreadings[1] = reverseCurrent*(exp(-(50/3) * v2)-1) - darkCurrent;
  pdreadings[2] = reverseCurrent*(exp(-(50/3) * v3)-1) - darkCurrent;

  RoveComm.write(RC_SCIENCESENSORSBOARD_FLUOROMETERDATA_DATA_ID,RC_SCIENCESENSORSBOARD_FLUOROMETERDATA_DATA_COUNT, pdreadings);*/
}

void o2Reading()
{
    if(O2_Serial.read()=='%') //Start at the beginning of the o2 Percentage sensor output
    {
      float o2readings[1];
      O2_Serial.read();
      o2readings[0] = strtof(readO2Bytes(6).c_str(),NULL)*10000.0; //Concentration - read in percent, converted to ppm
      RoveComm.write(RC_SCIENCESENSORSBOARD_O2_DATA_ID,RC_SCIENCESENSORSBOARD_O2_DATA_COUNT, o2readings);
    }
}

void ch4Reading()
{
    if(userial.read() =='C') //Start at the beginning of the ch4 sensor output
    {
      if(userial.read() ==':') //Start at the beginning of the ch4 sensor output
      {
        userial.read();
        float ch4readings[2];
        ch4readings[0] = strtof(readCh4Bytes(4).c_str(),NULL); //Concentration - read in percent, converted to ppm
        while (userial.read() != 'T');
        if(userial.read() == ':')
        {
          userial.read();
          ch4readings[1] = strtof(readCh4Bytes(3).c_str(),NULL);
        }
        RoveComm.write(RC_SCIENCESENSORSBOARD_CH3_DATA_ID,RC_SCIENCESENSORSBOARD_CH3_DATA_COUNT, ch4readings);
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

//Returns a string of the next "len" bytes read from Ch4 sensor
String readCh4Bytes(int len)
{
  String output="";
  for(int i=0;i<len;i++)
    output+=(char)userial.read();
   return output;
}

void noReading()
{
  //gets adc value from sensor output
  uint16_t raw = analogRead(NO_Gas);

  // get analog value
  float val = map(raw, MIN_ADC, MAX_ADC, MIN_NO_PPM, MAX_NO_PPM); //* ref_voltage / adc_resolution;
  val /= 1000.0; 
  //Serial.println((float)val);
  RoveComm.write(RC_SCIENCESENSORSBOARD_NO_DATA_ID,RC_SCIENCESENSORSBOARD_NO_DATA_COUNT,val);

  delay(100);
}

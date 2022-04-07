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
  CH4_Serial.begin(115200);

  //start RoveComm
  RoveComm.begin(RC_SCIENCESENSORSBOARD_FOURTHOCTET, &TCPServer);

  pinMode(UVLED_ENABLE_PIN,OUTPUT); //setup UVLED
  pinMode(Laser1,OUTPUT);
  pinMode(Laser2,OUTPUT);
  pinMode(Laser3,OUTPUT);

  digitalWrite(Laser1, LOW);
  digitalWrite(Laser2, LOW);
  digitalWrite(Laser3, LOW);

  // configure PIN mode
  pinMode(NO2_CS, OUTPUT);

  // set initial PIN state
  digitalWrite(NO2_CS, HIGH);

  // initialize SPI interface for MCP3208
  SPISettings settings(ADC_CLK, MSBFIRST, SPI_MODE0);
  SPI.begin();
  SPI.beginTransaction(settings);

  delay(100);
}

void loop(){
  //Read sensor data only every second
  if(timing%10==0)
  {
    o2Reading();
    co2Reading();
    noReading();
    no2Reading();
    ch4Reading();
    pdReading();
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

void pdReading()
{
  float pdreadings[3];
  //gets adc value from sensor output
  uint16_t raw1 = analogRead(Photodiode1);
  uint16_t raw2 = analogRead(Photodiode2);
  uint16_t raw3 = analogRead(Photodiode3);

  // get current value
  float v1 = raw1 * ref_voltage / adc_resolution;
  float v2 = raw2 * ref_voltage / adc_resolution;
  float v3 = raw3 * ref_voltage / adc_resolution;

  serial.println((float)v1);
  serial.println((float)v2);
  serial.println((float)v3);

  RoveComm.write(RC_SCIENCESENSORSBOARD_FLUOROMETERDATA_DATA_ID,RC_SCIENCESENSORSBOARD_FLUOROMETERDATA_DATA_COUNT, pdreadings);
}

void o2Reading()
{
  for(int i=0;i<25;i++)
  {
    if(O2_Serial.read()=='O') //Start at the beginning of the o2 sensor output
    {
      float o2readings[1];
      readO2Bytes(25); // skipping to the percent concentration
      o2readings[0] = strtof(readO2Bytes(6).c_str(),NULL)*10000; //Concentration - read in percent, converted to ppm
      readO2Bytes(11); //skipping rest of string
      if(o2readings[0]>500000)
      {
        o2readings[0] = o2readings[0] >> 1
      }
      RoveComm.write(RC_SCIENCESENSORSBOARD_O2_DATA_ID,RC_SCIENCESENSORSBOARD_O2_DATA_COUNT, o2readings);
      break;
    }
  }
}

void ch4Reading()
{
  for(int i=0;i<25;i++)
  {
    if(CH4_Serial.read()=='A') //Start at the beginning of the ch4 sensor output
    {
      float ch4readings[2];
      readO2Bytes(18); // skipping to the percent concentration
      ch4readings[0] = strtof(readCh4Bytes(4).c_str(),NULL)*10000; //Concentration - read in percent, converted to ppm
      readO2Bytes(17); //skipping to temperature
      ch4readings[1] = strtof(readCh4Bytes(2).c_str(),NULL)
      readO2Bytes(17); //skipping rest of string

      RoveComm.write(RC_SCIENCESENSORSBOARD_CH3_DATA_ID,RC_SCIENCESENSORSBOARD_CH3_DATA_COUNT, ch4readings);
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

//Returns a string of the next "len" bytes read from Ch4 sensor
String readCh4Bytes(int len)
{
  String output="";
  for(int i=0;i<len;i++)
    output+=(char)CH4_Serial.read();
   return output;
}

void noReading()
{
  //gets adc value from sensor output
  uint16_t raw = analogRead(NO_Gas);

  // get analog value
  float val = raw * ref_voltage / adc_resolution;

  serial.println((float)val);
  RoveComm.write(RC_SCIENCESENSORSBOARD_NO_DATA_ID,RC_SCIENCESENSORSBOARD_NO_DATA_COUNT,(float)val);

  delay(100);
}

void no2Reading()
{
  //gets adc value from sensor output
  uint16_t raw = readNo2();

  // get analog value
  float val = raw * ref_voltage / adc_resolution;

  serial.println((float)val);
  RoveComm.write(RC_SCIENCESENSORSBOARD_NO2_DATA_ID,RC_SCIENCESENSORSBOARD_NO2_DATA_COUNT,(float)val);

  delay(100);
}

uint16_t readNo2()
{
  byte first = 0;
  byte last = 0;
  uint16_t no2_reading = 0;

  digitalWrite(NO2_CS, LOW);

  // receive first(msb) 5 bits
  first = SPI.transfer(0x00);
  Computer_Serial.println(first);
  // receive last(lsb) 8 bits
  last = SPI.transfer(0x00);
  Computer_Serial.println(last);

  // deactivate ADC with slave select
  digitalWrite(NO2_CS, HIGH);

  // correct bit offset
  // |x|x|x|11|10|9|8|7| |6|5|4|3|2|1|0|1
  no2_reading =((first<<8)|(last&0xff));//Combine the data
  return (no2_reading >> 1);
}

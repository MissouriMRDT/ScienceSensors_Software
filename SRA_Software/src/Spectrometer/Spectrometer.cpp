#include <Ethernet.h>
#include "RoveComm.h"
#include "Spectrometer.h"

EthernetServer DataServer(11001);

void spectrometerSetup(int analogReadPin, bool printSerial)
{
  if(printSerial) Serial.println("Spectrometer Initializing");
  //pinMode(A4, INPUT);
  pinMode(PE_0, INPUT);

  DataServer.begin();
  if(printSerial) Serial.println("DataServer Init");
}

bool spectrometerRun(int analogReadPin, bool printSerial) {
  if(printSerial) Serial.println("---Spectrometer Run---");

  int count = 0;
  int16_t data[MAX_DATA_LENGTH];

  //---Begin data capture---
  //Wait for previous data to finish
  while(HWREG(GPIO_PORTE_BASE + (B00000001 << 2)) == (B00000001))
  {
    //Serial.println('a');
  }
  //Wait for interrupt to go high
  while(HWREG(GPIO_PORTE_BASE + (B00000001 << 2)) == (B00000000))
  {
    //Serial.println('b');
  }
  //While interrput is high
  while(HWREG(GPIO_PORTE_BASE + (B00000001 << 2)) == (B00000001))
  {
    //Capture like your life depends on it
    data[count] = analogRead(A19);
    count++;
  }

  //---End data capture---

  Serial.print("Count: "); Serial.println(count);
  EthernetClient client = DataServer.available();

  //Print Data
  for(int i = 0; i<count; i++)
  {
	if(printSerial)
	{
	  Serial.print(i);
	  Serial.print(",");
      Serial.println(data[i]);
	}

    if(client)
    {
      DataServer.print(i);
      DataServer.print(",");
      DataServer.println(data[i]);
    }
  }
  if(printSerial) Serial.println("---End of Spectrometer---");
  if(!client)
  {
    if(printSerial) Serial.println("XXX Client Error XXX");
    return(false);
  }
  else
  {
    client.stop();
    if(printSerial) Serial.println("Clinet Sent");
    return(true);
  }
}

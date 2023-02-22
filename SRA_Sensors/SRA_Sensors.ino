#include "SRA_Sensors.h"

void setup()
{
    CO2_SERIAL.begin(CO2_BAUD);
    O2_SERIAL.begin(O2_BAUD);
    CH4_SERIAL.begin(CH4_BAUD);
    delay(100);
    // start RoveComm
    RoveComm.begin(RC_SCIENCESENSORSBOARD_FOURTHOCTET, &TCPServer, RC_ROVECOMM_SCIENCESENSORSBOARD_MAC);

    SPI.begin();
    SPI.setClockDivider(SPI_CLOCK_DIV64);

    /*
    pinMode(UVLED_260, OUTPUT); // setup UVLEDs
    pinMode(UVLED_275, OUTPUT);
    pinMode(UVLED_280, OUTPUT);
    pinMode(UVLED_310, OUTPUT);

    digitalWrite(UVLED_260, LOW);
    digitalWrite(UVLED_275, LOW);
    digitalWrite(UVLED_280, LOW);
    digitalWrite(UVLED_310, LOW);
    */
    
    Telemetry.begin(telemetry, TELEM_TIMER);
}



void loop()
{
    packet = RoveComm.read();
    switch (packet.data_id)
    {
    case RC_SCIENCESENSORSBOARD_MICROSCOPESERVO_DATA_ID:
        microControl();
        break;
    }

    o2Read();
    co2Read();
    ch4Read();
    no2Read();
    nh3Read();
    telemetry();
}



void telemetry()
{
    if (o2ReadingOkay) {
        RoveComm.write(RC_SCIENCESENSORSBOARD_O2_DATA_ID, RC_SCIENCESENSORSBOARD_O2_DATA_COUNT, o2readings);
    }
    if (co2ReadingOkay) {
        RoveComm.write(RC_SCIENCESENSORSBOARD_CO2_DATA_ID, RC_SCIENCESENSORSBOARD_CO2_DATA_COUNT, (float)co2reading);
    }
    RoveComm.write(RC_SCIENCESENSORSBOARD_NH3_DATA_ID, RC_SCIENCESENSORSBOARD_NH3_DATA_COUNT, nh3Value);
    RoveComm.write(RC_SCIENCESENSORSBOARD_CH4_DATA_ID, RC_SCIENCESENSORSBOARD_CH4_DATA_COUNT, ch4readings);
    RoveComm.write(RC_SCIENCESENSORSBOARD_NO2_DATA_ID, RC_SCIENCESENSORSBOARD_NO2_DATA_COUNT, no2reading);
}

    

void o2Read()
{
    if (O2_SERIAL.read() == 'O') // Start at the beginning of the o2 Percentage sensor output
    {
        readO2Bytes(25);
        o2readings = strtof(readO2Bytes(6).c_str(), NULL) * 10000.0; // Concentration - read in percent, converted to ppm
        readO2Bytes(11);

        if (o2readings < 500000 && o2readings > 50000) o2ReadingOkay = true;
        else o2ReadingOkay = false;
    }
}



String readO2Bytes(int len) // Returns a string of the next "len" bytes read from 02 sensor
{
    String output = "";
    for (int i = 0; i < len; i++)
        output += (char)O2_SERIAL.read();
    return output;
}



void co2Read()
{
    // Send request to co2 sensor for data
    CO2_SERIAL.write(0xFF);
    CO2_SERIAL.write(0xFE);
    CO2_SERIAL.write(0x02);
    CO2_SERIAL.write(0x02);
    CO2_SERIAL.write(0x03);
    delay(20);

    // Read data from co2 sensor
    // First three reads are headers
    CO2_SERIAL.read();
    CO2_SERIAL.read();
    CO2_SERIAL.read();
    byte msb = CO2_SERIAL.read(); // Sensor data is sent in two bytes
    byte lsb = CO2_SERIAL.read();
    short co2reading = ((msb << 8) | (lsb & 0xff)); // Combine the data

    if (co2reading != -1) co2ReadingOkay = true; // If we got co2 reading then output
    else co2ReadingOkay = false;
}



void ch4Read()
{
    if (userial.read() == 'C') // Start at the beginning of the ch4 sensor output
    {
        if (userial.read() == ':') // Start at the beginning of the ch4 sensor output
        {
            userial.read();
            float ch4readings[2];
            ch4readings[0] = strtof(readCh4Bytes(4).c_str(), NULL); // Concentration - read in percent, converted to ppm
            while (userial.read() != 'T')
                ;
            if (userial.read() == ':')
            {
                userial.read();
                ch4readings[1] = strtof(readCh4Bytes(3).c_str(), NULL);
            }
        }
    }
}



void nh3Read()
{
    // gets adc value from sensor output
    uint16_t raw = analogRead(NO_GAS);

    // get analog value
    float nh3Value = map(raw, MIN_ADC, MAX_ADC, MIN_NO_PPM, MAX_NO_PPM);
    
    delay(100);
}



void no2Read()
{
    // our current sensor has a 200s response time, so no
}



void microControl()
{
    // code go brrr
}
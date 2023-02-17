#include "SRA_Sensors.h"

void setup()
{
    CO2_SERIAL.begin(CO2_BAUD);
    O2_SERIAL.begin(O2_BAUD);
    CH4_SERIAL.begin(CH4_BAUD);

    // start RoveComm
    RoveComm.begin(RC_SCIENCESENSORSBOARD_FOURTHOCTET, &TCPServer, RC_ROVECOMM_SCIENCESENSORSBOARD_MAC);

    bool ch4Available false;

    pinMode(UVLED_260, OUTPUT); // setup UVLEDs
    pinMode(UVLED_275, OUTPUT);
    pinMode(UVLED_280, OUTPUT);
    pinMode(UVLED_310, OUTPUT);

    digitalWrite(UVLED_260, LOW);
    digitalWrite(UVLED_275, LOW);
    digitalWrite(UVLED_280, LOW);
    digitalWrite(UVLED_310, LOW);
    Telemetry.begin(telemetry, TELEM_TIMER);
}

void loop()
{
    packet = RoveComm.read();

    switch (packet.data_id)
    {
    case RC_SCIENCESENSORSBOARD_FLUOROMETERLEDS_DATA_ID:
        LEDControl();


        fluroReading();
        break;
    case RC_SCIENCESENSORSBOARD_MICROSCOPESERVO_DATA_ID:
        microControl();
        break;
    }


    o2Reading();
    co2Reading();
    ch4Reading();
    no2Reading();
    nh3Reading();
    telemetry();
}

void telemetry()
{
    if (RC_SCIENCESENSORSBOARD_FLUOROMETERLEDS_DATA_ID) 
    {
        RoveComm.write(RC_SCIENCESENSORSBOARD_FLUOROMETERDATA_DATA_ID, RC_SCIENCESENSORSBOARD_FLUOROMETERDATA_DATA_COUNT, );
    }
}

    

void o2Reading()
{
    if (O2_SERIAL.read() == 'O') // Start at the beginning of the o2 Percentage sensor output
    {
        float o2readings;
        readO2Bytes(25);
        o2readings = strtof(readO2Bytes(6).c_str(), NULL) * 10000.0; // Concentration - read in percent, converted to ppm
        readO2Bytes(11);
        if (o2readings < 500000 && o2readings > 50000)
        {
            RoveComm.write(RC_SCIENCESENSORSBOARD_O2_DATA_ID, RC_SCIENCESENSORSBOARD_O2_DATA_COUNT, o2readings);
        }
    }
}

// Returns a string of the next "len" bytes read from 02 sensor
String readO2Bytes(int len)
{
    String output = "";
    for (int i = 0; i < len; i++)
        output += (char)O2_SERIAL.read();
    return output;
}

void co2Reading()
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
    short reading = (msb << 8) | (lsb & 0xff); // Combine the data

    if (reading != -1) // If we got co2 reading then output
    {
        RoveComm.write(RC_SCIENCESENSORSBOARD_CO2_DATA_ID, RC_SCIENCESENSORSBOARD_CO2_DATA_COUNT, (float)reading);
    }
}

void ch4Reading()
{
    if (ch4Available)
    {

    }
}

void ch4StartMeasurement()
{
    CH4_SERIAL.write(0x61);
    CH4_SERIAL.write(0x00);
    CH4_SERIAL.write(0x01);
    CH4_SERIAL.write(0x00);
    CH4_SERIAL.write(0x00);
    CH4_SERIAL.write(0x00);
    CH4_Serial.write(checksum);
    CH4_SERIAL.write(0x02);
}

void ch4init()
{
    
}

void nh3Reading()
{
    // gets adc value from sensor output
    uint16_t raw = analogRead(NO_GAS);

    // get analog value
    float val = map(raw, MIN_ADC, MAX_ADC, MIN_NO_PPM, MAX_NO_PPM);
    RoveComm.write(RC_SCIENCESENSORSBOARD_NH3_DATA_ID, RC_SCIENCESENSORSBOARD_NH3_DATA_COUNT, val);

    delay(100);
}


#include "SRA_Sensors.h"

void setup()
{
    CO2_SERIAL.begin(CO2_BAUD);
    O2_SERIAL.begin(O2_BAUD);
    myusb.begin();
    userial.begin(USERIAL_BAUD);

    // start RoveComm
    RoveComm.begin(RC_SCIENCESENSORSBOARD_FOURTHOCTET, &TCPServer, RC_ROVECOMM_SCIENCESENSORSBOARD_MAC);

    pinMode(UVLED_ENABLE_PIN, OUTPUT); // setup UVLED
    pinMode(Laser1, OUTPUT);
    pinMode(Laser2, OUTPUT);
    pinMode(Laser3, OUTPUT);

    digitalWrite(Laser1, LOW);
    digitalWrite(Laser2, LOW);
    digitalWrite(Laser3, LOW);
    Telemetry.begin(telemetry, TELEM_TIMER);
}

void loop()
{
    // Lasers and lights didn't end up happening for this board
    // RoveComm.read()
}

void telemetry()
{
    o2Reading();
    co2Reading();
    noReading();
    ch4Reading();
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

void ch4Reading()
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
            RoveComm.write(RC_SCIENCESENSORSBOARD_CH3_DATA_ID, RC_SCIENCESENSORSBOARD_CH3_DATA_COUNT, ch4readings);
        }
    }
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

// Returns a string of the next "len" bytes read from 02 sensor
String readO2Bytes(int len)
{
    String output = "";
    for (int i = 0; i < len; i++)
        output += (char)O2_SERIAL.read();
    return output;
}

// Returns a string of the next "len" bytes read from Ch4 sensor
String readCh4Bytes(int len)
{
    String output = "";
    for (int i = 0; i < len; i++)
        output += (char)userial.read();
    return output;
}

void noReading()
{
    // gets adc value from sensor output
    uint16_t raw = analogRead(NO_GAS);

    // get analog value
    float val = map(raw, MIN_ADC, MAX_ADC, MIN_NO_PPM, MAX_NO_PPM);
    RoveComm.write(RC_SCIENCESENSORSBOARD_NO_DATA_ID, RC_SCIENCESENSORSBOARD_NO_DATA_COUNT, val);

    delay(100);
}

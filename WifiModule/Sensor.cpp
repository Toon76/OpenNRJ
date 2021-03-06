#include "Arduino.h"
#include "Sensor.h"

#include <OneWire.h>
#include <DallasTemperature.h>


Adafruit_ADS1015 ads1015;

OneWire oneWire(0);  //data wire connected to GPIO0 (D3)
DallasTemperature sensors(&oneWire);

/* must be synchro with SensorInput_t  enum */
uint8_t arr_gpiodef[7] = {D0,D3,D4,D5,D6,D7,D8};

Sensor::Sensor(void)
{
  m_name[0] = '\0';
  m_input = UNDEFINED_I;
  m_type = UNDEFINED_T;
  m_value = 0;
}

void printAddress(DeviceAddress deviceAddress) {
  for (uint8_t i = 0; i < 8; i++){
    if (deviceAddress[i] < 16) Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
  }
}

void Sensor::init(char* name, SensorInput_t input, SensorType_t type)
{
  strcpy(m_name,name);
  m_input = input;
  m_type = type;
  m_value = 0;
  buffer.clear();
  bufferday.clear();
  
  if(m_input>=ADS1015_ADC0 && m_input<=ADS1015_ADC3)
  {
    Serial.println("init ADS1015");
    ads1015.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV
    ads1015.begin();
  }
  else if(m_input>=ESP8266_D0 && m_input<=ESP8266_D8)
  {
    if(m_type == DIGITAL_IN)
      pinMode(arr_gpiodef[m_input-ESP8266_D0], INPUT);
    else if(m_type == DIGITAL_OUT)
      pinMode(arr_gpiodef[m_input-ESP8266_D0], OUTPUT);
    else if(m_type == PWM)
    {
      pinMode(arr_gpiodef[m_input-ESP8266_D0], OUTPUT);
      //analogWriteFreq(new_frequency); //default 1000
      //analogWriteRange(new_range); //default 1023
      analogWrite(arr_gpiodef[m_input-ESP8266_D0], 0);
    }
  }
  else if(m_input>=DS18B20_1 && m_input<=DS18B20_4)
  {
    Serial.println("init DS18B20");
    DeviceAddress tempDeviceAddress; 
    sensors.begin();
    int numberOfDevices = sensors.getDeviceCount();
    for(int i=0;i<numberOfDevices; i++){
      // Search the wire for address
      if(sensors.getAddress(tempDeviceAddress, i)){
        Serial.print("Found device ");
        Serial.print(i, DEC);
        Serial.print(" (");
        printAddress(tempDeviceAddress);
        Serial.println(")");
      } else {
        Serial.print("Found ghost device at ");
        Serial.print(i, DEC);
        Serial.print(" but could not detect address. Check power and cabling");
      }
    }
  }
}


void Sensor::update(void)
{
  buffer.push(readValue());
}

int16_t Sensor::readValue(void)
{
  float value = 0;

  switch(m_input){
    case ESP8266_ADC:
      value = 0;//ads.readADC_SingleEnded(0);
    break;
    case ADS1015_ADC0:
    case ADS1015_ADC1:
    case ADS1015_ADC2:
    case ADS1015_ADC3:
      value = ads1015.readADC_SingleEnded(m_input - ADS1015_ADC0);
    break;
    case ESP8266_D0:
    case ESP8266_D3:
    case ESP8266_D4:
    case ESP8266_D5:
    case ESP8266_D6:
    case ESP8266_D7:
    case ESP8266_D8:
      value = digitalRead(arr_gpiodef[m_input-ESP8266_D0]);
      //ou analogRead pour PWM ?
    break;
    case DS18B20_1:
    case DS18B20_2:
    case DS18B20_3:
    case DS18B20_4:

      sensors.requestTemperatures(); // Send the command to get temperatures
      //sensors.requestTemperaturesByIndex(m_input-DS18B20_1);
      value = sensors.getTempCByIndex(m_input-DS18B20_1);
      Serial.print("Temp C: ");
      Serial.println(value);

      /*sensors.requestTemperatures(); // Send the command to get temperatures
      // Search the wire for address
      DeviceAddress tempDeviceAddress; 
      if(sensors.getAddress(tempDeviceAddress, m_input-DS18B20_1)){
      // Print the data
        value = sensors.getTempC(tempDeviceAddress);
        Serial.print("Temp C: ");
        Serial.println(value);
      }*/

    break;
    
    //value = m_value if output
  }

//Sensors
//PT1000 sensor charts
//http://embeddedooo.blogspot.com/2017/04/pt1000-temperature-sensor-chart.html
//https://www.thinksrs.com/downloads/programs/therm%20calc/ntccalibrator/ntccalculator.html

  switch(m_type)
  {
    case ANALOG_mV:
      value = value * 3260;
      value = value / 4096;
      m_value = value;
      Serial.print("Voltage(mV): ");Serial.println(m_value);
    break;
    case TEMP_NTC:
      {
        float R = 10000 * (value*2 / (3260-value*2));
        float logR, T;
        float c1 = 0.8880739089e-03, c2 = 2.514251712e-04, c3 = 1.922794488e-07;
        logR = log(R);
        T = (1.0 / (c1 + c2*logR + c3*logR*logR*logR));
        T = T - 273.15;
        m_value = (int16_t)((T+0.05)*10); //in tenth of °C and rounded
        Serial.print("Temperature(TEMP_NTC): "); Serial.print(T);Serial.println(" C");
      break;
      }
    case TEMP_DS18B20:
      m_value = (int16_t)((value+0.05)*10);
    break;
  }
  return m_value;
}


int16_t Sensor::getValue(void)
{
  return m_value;
}

void Sensor::setValue(int16_t val)
{
  m_value = val;
  switch(m_input){
    case ESP8266_D0:
    case ESP8266_D3:
    case ESP8266_D4:
    case ESP8266_D5:
    case ESP8266_D6:
    case ESP8266_D7:
    case ESP8266_D8:
      if(m_type == DIGITAL_OUT)
        digitalWrite(arr_gpiodef[m_input-ESP8266_D0], m_value);
    break;
  }
  //+ change gpio 
}

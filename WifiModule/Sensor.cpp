#include "Arduino.h"
#include "Sensor.h"


Adafruit_ADS1015 ads1015;

Sensor::Sensor(void)
{
  m_name[0] = '\0';
  m_input = UNDEFINED_I;
  m_type = UNDEFINED_T;
  //buffer.clear();
  //bufferday.clear();
}

void Sensor::init(char* name, SensorInput_t input, SensorType_t type)
{
	strcpy(m_name,name);
	m_input = input;
	m_type = type;
	buffer.clear();
	bufferday.clear();
  
  if(m_input>=ADS1015_ADC0 && m_input<=ADS1015_ADC3)
  {
    Serial.println("init ADS1015");
    ads1015.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV
    ads1015.begin();
  }
}


void Sensor::update(void)
{
  float value = 0;
  int16_t i_value = 0;
  switch(m_input){
    case ESP8266_ADC:
      value = 0;//ads.readADC_SingleEnded(0);
    break;
    case ADS1015_ADC0:
      value = ads1015.readADC_SingleEnded(0);
    break;
    case ADS1015_ADC1:
      value = ads1015.readADC_SingleEnded(1);
    break;
    case ADS1015_ADC2:
      value = ads1015.readADC_SingleEnded(2);
    break;
    case ADS1015_ADC3:
      value = ads1015.readADC_SingleEnded(3);
    break;
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
      i_value = value;
      Serial.print("Voltage(mV): ");Serial.println(i_value);
    break;
    case NTC:
      float R = 10000 * (value*2 / (3260-value*2));
   
      float logR, T;
      float c1 = 0.8880739089e-03, c2 = 2.514251712e-04, c3 = 1.922794488e-07;
      logR = log(R);
      T = (1.0 / (c1 + c2*logR + c3*logR*logR*logR));
      T = T - 273.15;
      i_value = (int16_t)((T+0.05)*10); //in tenth of °C and rounded
      Serial.print("Temperature(NTC): "); Serial.print(T);Serial.println(" C");
    break;
    //default:
    //  i_value = 0;
    // break;
  }
  buffer.push(i_value);
}

int16_t Sensor::getValue(void)
{
  return buffer[buffer.size()-1];
}

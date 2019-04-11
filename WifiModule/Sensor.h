#ifndef __SENSOR_H
#define __SENSOR_H

#include <Arduino.h>
#include <CircularBuffer.h>
#include <Adafruit_ADS1015.h>

#define SAMPLING_T      15  //in seconds
#define DAY_HISTORY_T   600//600 //in seconds

typedef enum
{
  UNDEFINED_I = 0,
  ESP8266_ADC,
	ADS1015_ADC0,
	ADS1015_ADC1,
	ADS1015_ADC2,
	ADS1015_ADC3,
}SensorInput_t;

typedef enum
{
  UNDEFINED_T = 0,
	ANALOG_mV,
	PT1000, //Not used need amplifier
	NTC,
	DIGITAL
}SensorType_t;

class Sensor
{
 protected:
	//int m_sampling_rate;			//sec = SAMPLING_T
	//int m_history_sampling_rate;	//sec = x*SAMPLING_T
	SensorInput_t m_input;
	SensorType_t m_type;
	//float params[3]; //SensorType_t dependant (to be done)
 public:
  char m_name[32];
  //buffer
  CircularBuffer<int16_t, (3600/SAMPLING_T)> buffer; //sampling period 15s (240*15s = 1H) 
  CircularBuffer<int16_t, (86400/DAY_HISTORY_T)> bufferday; //sampling period 10min (144*10m = 1day) 
  Sensor(void);
  void init(char* name, SensorInput_t input, SensorType_t type);
  void update(void);
  int16_t getValue(void);
 private:
}; 


#endif /* __SENSOR_H */

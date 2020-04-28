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
  //ADS using Wire/I2C (pin D1,D2)
  ADS1015_ADC0,
	ADS1015_ADC1,
	ADS1015_ADC2,
  ADS1015_ADC3,

  //sync GPIO with arr_gpiodef
  ESP8266_D0, //GPIO16 (warning:high at boot)
  ESP8266_D3, //GPIO0 (warning: used at boot)
  ESP8266_D4, //GPIO2, ESP8266 LED (warning: used at boot)
  ESP8266_D5, //GPIO14
  ESP8266_D6, //GPIO12
  ESP8266_D7, //GPIO13
  ESP8266_D8, //GPIO15 (warning: used at boot)

  DS18B20_1,  //use GPIO14 (D5)
  DS18B20_2,  //use GPIO14 (D5)
  DS18B20_3,  //use GPIO14 (D5)
  DS18B20_4  //use GPIO14 (D5)
  
  //RX //GPIO3
  //TX //GPIO1
  //SPI CLK  //GPIO6
  //SPI MISO //GPIO7
  //SPI CS   //GPIO11
  //SPI MOSI //GPIO8
  //GPIO9
  //GPIO10
}SensorInput_t;

typedef enum
{
  UNDEFINED_T = 0,
	ANALOG_mV,
  TEMP_NTC,
  TEMP_DS18B20,
  DIGITAL_IN,
  DIGITAL_OUT,
  PWM
}SensorType_t;



class Sensor
{
 protected:
	//int m_sampling_rate;			//sec = SAMPLING_T
	//int m_history_sampling_rate;	//sec = x*SAMPLING_T
	SensorInput_t m_input;
	SensorType_t m_type;
  int16_t m_value;
	//float params[3]; //SensorType_t dependant (to be done)
 public:
  char m_name[32];
  //buffer
  CircularBuffer<int16_t, (3600/SAMPLING_T)> buffer; //sampling period 15s (240*15s = 1H) 
  CircularBuffer<int16_t, (86400/DAY_HISTORY_T)> bufferday; //sampling period 10min (144*10m = 1day) 
  Sensor(void);
  void init(char* name, SensorInput_t input, SensorType_t type);
  void update(void);
  int16_t readValue(void); //read the data physically
  int16_t getValue(void); //read the buffered data
  void setValue(int16_t val);
 private:
}; 


#endif /* __SENSOR_H */

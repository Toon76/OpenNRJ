#include "Arduino.h"
#include "SenseData.h"
#include <TaskScheduler.h>

//Scheduler
void SamplingCallback();
void HistoryCallback();
Task SamplingTask(SAMPLING_T*1000, TASK_FOREVER, &SamplingCallback);
Task HistoryTask(DAY_HISTORY_T*1000, TASK_FOREVER, &HistoryCallback);
Scheduler sched;

Sensor SensorArray[MAX_SENSOR_NB];
  
void SamplingCallback() {
    for(int i = 0;i<MAX_SENSOR_NB;i++)
    {
      SensorArray[i].update();
    }    
}
void HistoryCallback() {
    Serial.println("HistoryCallback");
    //put last element of sensor buffer in bufferday
    for(int i = 0;i<MAX_SENSOR_NB;i++)
    {
      SensorArray[i].bufferday.push(SensorArray[i].buffer[SensorArray[i].buffer.size()-1]);
    }
}

SenseData::SenseData(void)
{
  
}

void SenseData::init(void)
{
  SensorArray[0].init("ADC0",ADS1015_ADC0,ANALOG_mV);
  SensorArray[1].init("ADC1",ADS1015_ADC0,ANALOG_mV);
  SensorArray[2].init("ADC2",ADS1015_ADC0,ANALOG_mV);
  SensorArray[3].init("ADC3",ADS1015_ADC0,ANALOG_mV);
  SensorArray[4].init("temp",ADS1015_ADC0,TEMP_NTC);
  SensorArray[5].init("D0IN",ESP8266_D0,DIGITAL_IN);
  SensorArray[6].init("D3IN",ESP8266_D3,DIGITAL_IN);
  SensorArray[7].init("D4OUT",ESP8266_D4,DIGITAL_OUT);
  SensorArray[8].init("D5PWM",ESP8266_D5,PWM);
  SensorArray[9].init("D6IN",ESP8266_D6,DIGITAL_IN);

	//Scheduler init
	sched.init();
	sched.addTask(SamplingTask);
	sched.addTask(HistoryTask);
	SamplingTask.enable();
	HistoryTask.enable();
  Serial.println("Sense init");
}

void SenseData::run(void)
{
	sched.execute();
/*  //can do some logic here
  if(SensorArray[0].getValue() > 128)
    SensorArray[7].setValue(1);
  else
    SensorArray[7].setValue(0);

  */
}

//void addSensor(Sensor sensor);
String SenseData::getData(char* name)
{
  String ret = "{ ";
  for(int i = 0;i<MAX_SENSOR_NB;i++)
  {
    if(SensorArray[i].m_name[0] != '\0')
    {
      if(strcmp(name,SensorArray[i].m_name) == 0 || strcmp("",name) == 0)
      {
        ret += "\""+String(SensorArray[i].m_name)+"\":"+SensorArray[i].getValue()+",";
      }
    }
  }
  ret.remove(ret.length()-1);
  ret += "}";
  return ret;
}


int16_t SenseData::getValue(char* name)
{
  int ret = 0;
  for(int i = 0;i<MAX_SENSOR_NB;i++)
  {
    if(SensorArray[i].m_name[0] != '\0')
    {
      if(strcmp(name,SensorArray[i].m_name) == 0 || strcmp("",name) == 0)
      {
        ret = SensorArray[i].getValue();
      }
    }
  }
  return ret;
}

void SenseData::setValue(char* name, int16_t value)
{
  Serial.print("setValue: ");Serial.print(name);Serial.print(" ");Serial.println(value);
  for(int i = 0;i<MAX_SENSOR_NB;i++)
  {
    if(strcmp(name,SensorArray[i].m_name) == 0)
    {
      SensorArray[i].setValue(value);
    }
  }
}

String SenseData::getHistory(char* name,int qty,int long_history)
{
  //manage qty and long_history argument
  String ret = "{";
  if(strcmp("",name) != 0)
  {
    for(int i = 0;i<MAX_SENSOR_NB;i++)
    {
      if(strcmp(name,SensorArray[i].m_name) == 0)
      {
        ret += "\""+String(SensorArray[i].m_name)+"\":[";
        if(long_history == 1)
        {
          for(int j=0; j< SensorArray[i].bufferday.size();j++)
          {
            ret += SensorArray[i].bufferday[j];
            if(j < (SensorArray[i].bufferday.size()-1))
              ret += ",";
            else
              ret += "]";
          }
        }
        else
        {
          for(int j=0; j< SensorArray[i].buffer.size();j++)
          {
            ret += SensorArray[i].buffer[j];
            if(j < (SensorArray[i].buffer.size()-1))
              ret += ",";
            else
              ret += "]";
          }
        }
      }
    }
  }
  ret += "}";
  return ret;
}


  

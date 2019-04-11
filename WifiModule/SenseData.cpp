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
  SensorArray[0].init("ana",ADS1015_ADC0,ANALOG_mV);
  SensorArray[1].init("ana2",ADS1015_ADC0,NTC);

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
//void getData(Sensor sensor);
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

  
  

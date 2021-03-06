#ifndef __DATA_H
#define __DATA_H

#include <Arduino.h>
#include <CircularBuffer.h>
#include "Sensor.h"

#define MAX_SENSOR_NB	10
#define PROCESS_T      1000  //in millisec

class SenseData
{
 protected:
   uint8_t   test;
 public:
  SenseData(void);
  void init(void);
  void run(void);
  //void addSensor(Sensor sensor);
  String getData(char* name);
  int16_t getValue(char* name);
  void setValue(char* name, int16_t val);
  //void getData(Sensor sensor);
  String getHistory(char* name,int qty,int long_history);
  //void getHistory(Sensor sensor,int qty);
 private:
};


#endif /* __DATA_H */

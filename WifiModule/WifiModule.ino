#include "FS.h" // for SPIFFS
//#include "FS.h"
#include <ESP8266WiFi.h>
#include <Adafruit_ADS1015.h>
#include <TaskScheduler.h>
#include <CircularBuffer.h>

//buffer
CircularBuffer<float, 360> buffer; //sampling period 10s (360*10s = 1H) 
CircularBuffer<float, 288> bufferday; //sampling period 5min (288*5m = 1day) 
CircularBuffer<float, 168> bufferweek; //sampling period 1h (168*1h = 1week) 

//Tasks
void ADCCallback();
void printCallback();
Task task1(10000, TASK_FOREVER, &ADCCallback);
Scheduler runner;

//PT1000 sensor charts
//http://embeddedooo.blogspot.com/2017/04/pt1000-temperature-sensor-chart.html
Adafruit_ADS1015 ads;
  
WiFiServer server(80);
File webFile;
const char* ssid = "TOON_WIFI";
const char* password = "menerval";

unsigned long prevTime;
/////////////////////////
void ADCCallback() {
  static int value = 0;
    int16_t adc0 = ads.readADC_SingleEnded(0);//random(0, 100);
    float R = 10000 * ((float)adc0*2 / (3260-(float)adc0*2));
   
    float logR, T;
    float c1 = 0.8880739089e-03, c2 = 2.514251712e-04, c3 = 1.922794488e-07;
    logR = log(R);
    T = (1.0 / (c1 + c2*logR + c3*logR*logR*logR));
    T = T - 273.15;
    Serial.print("Temperature: "); Serial.print(T);Serial.println(" C");
    buffer.push(T);          
}

//////////////////////////////
////////////SETUP/////////////
//////////////////////////////
void setup()
{  
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Serial.begin(115200);
  
  //Scheduler initialization
  runner.init();
  runner.addTask(task1);
  task1.enable();

 //External ADC initialization
  ads.setGain(GAIN_ONE);        // 1x gain   +/- 4.096V  1 bit = 2mV
  ads.begin();

  //SPIFFS initialization
  bool result = SPIFFS.begin();
  Serial.print("SPIFFS opened: ");
  Serial.println(result);
  /*String str = "";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next()) {
      str += dir.fileName();
      str += " / ";
      str += dir.fileSize();
      str += "\r\n";
  }
  Serial.println(str);*/

  //Wifi initialization
  Serial.print("Connect to "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  server.begin();
  Serial.print("\n\rat IP:");
  Serial.println(WiFi.localIP());
  
  digitalWrite(2, HIGH);

}


//////////////////////////////
////////////LOOP//////////////
//////////////////////////////  
void loop()
{
  runner.execute();
  WiFiClient client = server.available();  // try to get client
  
  digitalWrite(2, HIGH);
  if (client) {
    
    digitalWrite(2, LOW);
    /*String req,line;
    while (client.connected() || client.available()) {
      delay(1); // wait before read to skip data
      while (client.available()) {
        line = client.readStringUntil('\n');
        //Serial.println(line);
        if(line.indexOf("GET /") != -1) req = line;
      }*/
      
      //client.setTimeout(5000); // default is 1000
      // Read the first line of the request
      String req = client.readStringUntil('\r');
      while (client.available()) {
        client.readStringUntil('\n');
      }
      
      //delay(10);
      //manage request
      if(req.indexOf("GET /") != -1) {
        Serial.println(req);
        String path = req.substring(req.indexOf("/"),req.indexOf(" HTTP/1.1"));
        if(path.endsWith("/")) path += "index.html";
        
        if(path.indexOf(".") == -1) //not a file
        {
          if (path == "/getADC"){
            Serial.println("serving ADC");
            int a = random(0, 100);
            String adc = String(a);
            Serial.println(adc);
            client.print(F("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n\r\n"));
            client.println(adc);
          }
          else if(path == "/getData"){
            Serial.println("serving data");
            using index_t = decltype(buffer)::index_t;
            client.print(F("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"));
            client.print("{\"data\":[");
            using index_t = decltype(buffer)::index_t;
            for (index_t i = 0; i < buffer.size(); i++) {
              client.print(buffer[i]);
              if(i < (buffer.size()-1))
                client.print(",");
            }
            /*while (!buffer.isEmpty()) {
              client.print(buffer.pop());
              if(!buffer.isEmpty())
                client.print(",");
            }*/
            client.println("]}");
          }
        }
        else
        {
          //select dataType
          String dataType = "text/plain";
          if(path.endsWith(".html")) dataType = "text/html";
          else if(path.endsWith(".htm")) dataType = "text/html";
          else if(path.endsWith(".css")) dataType = "text/css";
          else if(path.endsWith(".js")) dataType = "application/javascript";
          else if(path.endsWith(".jpg")) dataType = "image/jpeg";
          //send file
          webFile = SPIFFS.open(path, "r");
          if (webFile) {
            Serial.println("Serve file :"+path);
            client.println("HTTP/1.1 200 OK");
            //client.print("Content-Type: ");client.println(dataType);
            client.println("Connnection: close\r\n");
            prevTime=millis();
            client.write(webFile);
            webFile.close();
            Serial.print("time to serve: ");Serial.println((millis()-prevTime)/1000);
          }
        }
      }
      else
      {
        Serial.println("####request error");Serial.println(req);
      }
      delay(1);
      client.stop(); // close the connection
   // } // end while (client.connected())
  } // end if (client)
}

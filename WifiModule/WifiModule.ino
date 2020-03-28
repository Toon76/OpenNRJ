#include "FS.h" // for SPIFFS
//#include "FS.h"
#include <ESP8266WiFi.h>

#include "web.h"
#include "SenseData.h"
#include <ArduinoJson.h>
 
WiFiServer server(80);
File webFile;
File confFile;
const char* ssid = "HUAWEI";
const char* password = "menerval";

SenseData sense_data;
//////////////////////////////
////////////SETUP/////////////
//////////////////////////////
void setup()
{  
  pinMode(D4, OUTPUT);
  digitalWrite(D4, LOW);
  Serial.begin(115200);


  sense_data.init();

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
  digitalWrite(D4, HIGH);
}


//////////////////////////////
////////////LOOP//////////////
////////////////////////////// 

String payload; 
bool got_config = false;
void loop()
{
  //scheduler.execute();
  sense_data.run();
  WiFiClient client = server.available();  // try to get client
  
  if(got_config)
  {
    got_config = false;
    // Allocate the JSON document and Deserialize the JSON document
    //DynamicJsonDocument doc(2048);
    StaticJsonDocument<1024> doc;
    DeserializationError error = deserializeJson(doc, payload);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.c_str());
      return;
    }
    else
    {
      //save file
      confFile = SPIFFS.open("/my_data.json", "w+");
      if (confFile) {
        confFile.print(payload);
        confFile.close();
      }
      else
      {
        Serial.println("##### error while saving configuration");
      }
      //const char* test = doc["config"]["WifiName"] ;
    }
  }

  //digitalWrite(D4, HIGH);
  if (client) {
    
    //digitalWrite(D4, LOW);
    //client.setTimeout(5000); // default is 1000
    // Read the first line of the request
    String req = client.readStringUntil('\r');
    while (client.available()) {
      payload = client.readStringUntil('\n');
    }
    //manage request
    if(req.indexOf("GET /") != -1) {
      //Serial.println(req);
      String path = req.substring(req.indexOf("/"),req.indexOf(" HTTP/1.1"));
      if(path.endsWith("/")) path += "index.html";

      Serial.println(path);
      
      if(path.indexOf(".") == -1) //not a file
      {
        char buf_value[32];
        int long_history = 0;
        int history_qty = 0;
        int value = 0;
        //get request parameters
        char param_name[32];
        WEB_parse(path,"name",param_name);
        WEB_parse(path,"mode",buf_value);
        if(strcmp(buf_value, "1") == 0)
          long_history = 1;
        WEB_parse(path,"qty",buf_value);
        String str(buf_value);
        history_qty = str.toInt(); 
        WEB_parse(path,"value",buf_value);
        str = buf_value;
        value = str.toInt(); 
        //WEB_parse(path, &long_history, &history_qty, param_name);

        //execute request
        if(path.indexOf("/getData") != -1){
          client.print(F("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"));
          client.println(sense_data.getData(param_name));
        }
        else if(path.indexOf("/getHistory") != -1){
          client.print(F("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"));
          client.println(sense_data.getHistory(param_name,history_qty,long_history));
        }
        else if(path.indexOf("/setData") != -1){
          client.print(F("HTTP/1.1 200 OK\r\n"));
          sense_data.setData(param_name,value);
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
          client.println("HTTP/1.1 200 OK");
          //client.print("Content-Type: ");client.println(dataType);
          client.println("Connnection: close\r\n");
          client.write(webFile);
          webFile.close();
        }
      }
    }
    else if(req.indexOf("POST /") != -1) {
      String path = req.substring(req.indexOf("/"),req.indexOf(" HTTP/1.1"));
      Serial.print("POST ");Serial.println(path);
      Serial.println(payload);
      client.print(F("HTTP/1.1 200 OK\r\n"));
      client.println("Connnection: close\r\n");
      got_config = true;
    }
    else
    {
      Serial.println("####request error");Serial.println(req);
    }
    delay(1);
    client.stop(); // close the connection
  } // end if (client)
}

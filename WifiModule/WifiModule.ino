#include "FS.h" // for SPIFFS
//#include "FS.h"
#include <ESP8266WiFi.h>

#include "web.h"
#include "SenseData.h"

 
WiFiServer server(80);
File webFile;
const char* ssid = "TOON_WIFI";
const char* password = "menerval";

SenseData sense_obj;
//////////////////////////////
////////////SETUP/////////////
//////////////////////////////
void setup()
{  
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  Serial.begin(115200);


  sense_obj.init();

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
  //scheduler.execute();
  sense_obj.run();
  WiFiClient client = server.available();  // try to get client
  
  digitalWrite(2, HIGH);
  if (client) {
    
    digitalWrite(2, LOW);
    //client.setTimeout(5000); // default is 1000
    // Read the first line of the request
    String req = client.readStringUntil('\r');
    while (client.available()) {
      client.readStringUntil('\n');
    }
    //manage request
    if(req.indexOf("GET /") != -1) {
      //Serial.println(req);
      String path = req.substring(req.indexOf("/"),req.indexOf(" HTTP/1.1"));
      if(path.endsWith("/")) path += "index.html";

      Serial.println(path);
      
      if(path.indexOf(".") == -1) //not a file
      {
        //get request parameters
        int long_history;
        int history_qty;
        char param_name[32];
        WEB_parse(path, &long_history, &history_qty, param_name);

        //execute request
        if(path.indexOf("/getData") != -1){
          client.print(F("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"));
          client.println(sense_obj.getData(param_name));
        }
        else if(path.indexOf("/getHistory") != -1){
          client.print(F("HTTP/1.1 200 OK\r\nContent-Type: application/json\r\n\r\n"));
          client.println(sense_obj.getHistory(param_name,history_qty,long_history));
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
    else
    {
      Serial.println("####request error");Serial.println(req);
    }
    delay(1);
    client.stop(); // close the connection
  } // end if (client)
}

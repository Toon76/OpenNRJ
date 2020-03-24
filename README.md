[comment]: <> (use https://dillinger.io/ to edit)
# Project Overview
## Introduction
This is an open-source project to spread and facilitate renweable and efficient energy production.
Application such as thermal solar panels management or double flux ventilation in homes are targeted
## Software
Arduino based for embedded application to ease deployement and adoption by the community
HTML5 and open source libraries for the web side mainly working on client side
Lightweight solution to have a simple, clear and reactive interface.
## Hardware
* Low cost Wifi module based on ESP8266
* I/O board (TBD) to plug sensors and actuators (schematics and manufacturing files open source)
* Idea is to create ready-to-use kits (BOMs)
* [Setup](#setup)
* IO list 
https://learn.sparkfun.com/tutorials/esp8266-thing-hookup-guide/using-the-arduino-addon
| ESP8266    | NodeMCU | Use     |
| :--------: |:-------:|:-------:|
| GPIO 0     |    D3   | Output  |
| GPIO 2     |    D4   | Output  |
| GPIO 4     |    D2   | Input/Output |
| GPIO 5     |    D1   | Input/Output |
| GPIO 12    |    D6   | Input/Output |
| GPIO 13    |    D7   | Input/Output |
| GPIO 14    |    D5   | Input/Output |
| GPIO 15    |    D8   | Output  |
put 9 & 10 for I2C ? conflict with flash ?

Full tuto and pinout here : https://create.arduino.cc/projecthub/electropeak/getting-started-w-nodemcu-esp8266-on-arduino-ide-28184f


# Get Started
TBD
# API Documentation
## General info
API documentation describes comunication between web server hosted on the wifi module and the web interface on the client side. It provides the web interface itself and also an access to the data and configration in a RESTFULL format
[comment]: <> (look at https://gist.github.com/iros/3426278)
## GET /index.html
root of the web interface wich loads other needed files (/ is also valid)
## GET /getData[&name=value_name]
**parameters**
name : optional parameter to select only one value
**body**
returns data in a JSON format
```
{
    "value_name" : value, 
    "value_name" : value
}
```
## GET /getHistory?mode=(1/2)[&qty=1][&name=value_name]
**parameters**
mode : 1 means last hour, 2 means last day(s)
qty : optional parameter, means how many days are required (default is 1)
name : optional parameter to select only one history (to prevent long request on big history)
**body**
returns data in a JSON format
```
{
    "period" : xxx, (in seconds)
    "value_name" : [value1,value2,value3....], //last value is the newest
    "value_name" : [value1,value2,value3....]
}
```
## GET /getConfig
TBD
## PUT /setConfig
TBD

#include "web.h"
#include <Regexp.h>

void print_hello(char* text) {
    Serial.print("Hello ");
    Serial.println(text);
}



// called for each match
bool long_history;		//ref value for history type
int history_qty;		//ref value for quantity (number of days)
char param_name[32];	//ref value variable name
char url_buffer[64];
void url_callback(const char * match,const unsigned int length,const MatchState & ms)
{
  char param[16];
  char value[32];
  ms.GetCapture (param, 0);
  ms.GetCapture (value, 1);
  if(strcmp(param, "name") == 0){
    strcpy(param_name,value);
  }
  else if(strcmp(param, "mode") == 0){
    if(strcmp(value, "1") == 0)
      long_history = true;
    else
      long_history = false; 
  }
  else if(strcmp(param, "qty") == 0){
    String str(value);
    history_qty = str.toInt(); 
  }
}


void WEB_parse(String url,int* mode,int* qty, char* name)
{
    url.toCharArray(url_buffer, 64);
	//set default values
	long_history = false;
	history_qty = 1;
	param_name[0] = '\0';
	//parse url
    MatchState ms(url_buffer);
    int match_count = ms.GlobalMatch ("[?&]([0-9a-z]*)=([0-9a-z-_]*)", url_callback);
    //set output
	*mode = long_history;
	*qty = history_qty;
 strcpy(name,param_name);
}

String WEB_getData(char* name)
{
	
}
void WEB_getHistory(){}
void WEB_getConfig(){}

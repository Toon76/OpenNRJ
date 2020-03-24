#include "web.h"
#include <Regexp.h>

void print_hello(char* text) {
    Serial.print("Hello ");
    Serial.println(text);
}

char url_buffer[64];
char l_key[32];
char l_value[32];
void url_callback(const char * match,const unsigned int length,const MatchState & ms)
{
  char param[32];
  ms.GetCapture (param, 0);
  if(strcmp(l_key, param) == 0){
    ms.GetCapture (l_value, 1);
  }
}

void WEB_parse(String url,char* key, char* value)
{
  url.toCharArray(url_buffer, 64);
  strcpy(l_key,key);
  value[0] = '\0';
  //parse url
  MatchState ms(url_buffer);
  int match_count = ms.GlobalMatch ("[?&]([0-9a-zA-Z]*)=([0-9a-zA-Z-_]*)", url_callback);
  //set output
  if(match_count)
    strcpy(value,l_value);
}

void WEB_getData()
//String WEB_getData(char* name)
{
	
}
void WEB_getHistory(){}
void WEB_getConfig(){}

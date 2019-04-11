#ifndef __WEB_H
#define __WEB_H

#include <Arduino.h>

void print_hello(char* text);
void WEB_parse(String url,int* mode,int* qty, char* name);
void WEB_getData();
void WEB_getHistory();
void WEB_getConfig();


#endif /* __WEB_ */
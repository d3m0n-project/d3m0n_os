#ifndef SETTINGSFILEHANDLER_H_
#define SETTINGSFILEHANDLER_H_

char* getSetting(char* name,  char* path);
void writeSetting(char* name, char* value, char* path);

#endif
#ifndef SETTINGSFILEHANDLER_H
#define SETTINGSFILEHANDLER_H

char* getSetting(char* name,  char* path);
void writeSetting(char* name, char* value, char* path);

#endif
#ifndef UTILS_H
#define UTILS_H
#pragma once


#include <list>
#include <regex>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>
#include <fstream>
// #include <display.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits>
#include <Type.h>
#include <vector>
#include <locale>
#include <codecvt>

using namespace std;
    
// def vars
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
// debug window name
extern string window_name;

// debug
#define DEBUG 1

bool isTestingMode();

bool isDebugMode();
string bashColor(int color);
void RGB565toRGB(uint16_t rgb565, uint8_t& r, uint8_t& g, uint8_t& b);
uint16_t RGBtoRGB565(uint8_t r, uint8_t g, uint8_t b);
bool isNumber(const string& str);

namespace Colors {
    extern ushort Red            ;
    extern ushort DarkRed        ;
    extern ushort Orange         ;
    extern ushort DarkOrange     ;
    extern ushort DarkYellow     ;
    extern ushort Yellow         ;
    extern ushort DarkGreen      ;
    extern ushort Green          ;
    extern ushort DarkBlue       ;
    extern ushort Blue           ;
    extern ushort Cyan           ;
    extern ushort DarkCyan       ;
    extern ushort DarkMagenta    ;
    extern ushort Magenta        ;
    extern ushort DarkGrey       ;
    extern ushort Grey           ;
    extern ushort White          ;
    extern ushort Reset          ;
    extern ushort Black          ;
};

ushort GetColor(string color);

void replaceAll(string& str, const string& from, const string& to);

bool startsWith(const string& str, const string& prefix);
bool endsWith(const string& str, const string& suffix);

bool contains(const string& str, const string& substr);
string Split(const string& str, char delimiter, bool beforeDelimiter = true);
string delete_space(string input);

namespace ConsoleColor {
    extern const string Reset         ;
    extern const string Black         ;
    extern const string Red           ;
    extern const string Green         ;
    extern const string Yellow        ;
    extern const string Blue          ;
    extern const string Magenta       ;
    extern const string Cyan          ;
    extern const string White         ;
    extern const string Orange        ;
    extern const string DarkRed       ;
    extern const string DarkOrange    ;
    extern const string DarkYellow    ;
    extern const string DarkGreen     ;
    extern const string DarkBlue      ;
    extern const string DarkCyan      ;
    extern const string DarkMagenta   ;
    extern const string DarkGrey      ;
    extern const string Grey          ;
 
    extern string get(string color);
}

struct Application
{
    string name;
    string description;
    string package;
    string icon;
    string perms;
    string start_path;
    string category;
    string temp_path;
    vector<DataTypes::Window*> windows;
    DataTypes::Window* mainWindow;
};

namespace LogStatus {
    extern const int Success;
    extern const int Loading;
    extern const int Info;
    extern const int Warning;
    extern const int Error;
    extern const int Critical;
}

bool readAllBytes(const string& filePath, vector<unsigned char>& data);

string logn(string text, string color);

string log(string text, int status);
string exec(const char* cmd);
string Trim(string str);

string removeUnwantedChars(string input);

string GetAppPath();


string GetPath();

char* GetConfig();

string Split(string s, string delimiter);

void removeQuotes(string& str);
void deleteLeadingSpaces(string& str);
string printInfos();
u16string to_u16string(const string& str);
string bashGradient(int baseColor, string text);
vector<char> stringToUtf8(const string& str);

struct UIEvent
{  
    public:
    struct Listener {
        DataTypes::Rect rect;
        function<void(Application)> callback;
        short type;
        Application app;
    };
    
    static const short OnClick=0;
    static const short OnLongClick=1;
    static const short OnTouch=2;
    static const short OnRelease=3;
    static const short OnMove=4;
    static const short OnScroll=5;
}; 

#endif // !UTILS_H
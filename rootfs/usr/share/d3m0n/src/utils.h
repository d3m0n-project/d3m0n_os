#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits>

using namespace std;

// debug
#define DEBUG 1

void replaceAll(string& str, const string& from, const string& to) {
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', advance start_pos to avoid infinite loop
    }
}

bool startsWith(const string& str, const string& prefix) {
    return str.compare(0, prefix.size(), prefix) == 0;
}

namespace ConsoleColor {
    const string Reset   = "\033[0m";
    const string Black   = "\033[0;30m";
    const string Red     = "\033[0;31m";
    const string Green   = "\033[0;32m";
    const string Yellow  = "\033[0;33m";
    const string Blue    = "\033[0;34m";
    const string Magenta = "\033[0;35m";
    const string Cyan    = "\033[0;36m";
    const string White   = "\033[0;37m";
}

struct Point {
    int X;
    int Y;
    Point(int x, int y) {
        this->X = x;
        this->Y = y;
    }
    Point() : X(0), Y(0) {}
};
struct Size {
    int Width;
    int Height;
    Size(int x, int y) {
        this->Width = x;
        this->Height = y;
    }
    Size() : Width(0), Height(0) {}
};

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
};

namespace Colors {
	ushort Red = 0xf800;
    ushort Green = 0x07e0;
    ushort Blue  = 0x001f;
    ushort Black = 0x0000;
    ushort White = 0xffff;
    ushort Gray  = 0x8c51;
    ushort Yellow = 0xFFE0;
    ushort Cyan  = 0x07FF;
    ushort Magenta = 0xF81F;
    ushort Transparent = 0x0000;
};

ushort GetColor(string color) {
    try {
        unsigned long ulongValue = stoul(color);
        
        if (ulongValue > numeric_limits<unsigned short>::max()) {
            throw out_of_range("Value exceeds range of unsigned short");
        }
        
        return static_cast<unsigned short>(ulongValue);
    } catch (const exception& e) {
        if (color == "black") return Colors::Black;
        if (color == "green") return Colors::Green;
        if (color == "blue") return Colors::Blue;
        if (color == "red") return Colors::Red;
        if (color == "white") return Colors::White;
        if (color == "gray") return Colors::Gray;
        if (color == "yellow") return Colors::Yellow;
        if (color == "cyan") return Colors::Cyan;
        if (color == "magenta") return Colors::Magenta;
        if (color == "transparent") return Colors::Transparent;
    }
    return Colors::Red; // default is red.
}
uint16_t RGBtoRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}

struct Rect {
    int X;
    int Y;
    int X2;
    int Y2;
    Rect(int x, int y, int x2, int y2) {
        this->X = x;
        this->Y = y;
        this->X2 = x2;
        this->Y2 = y2;
    }
};


namespace LogStatus {
    const int Success = 0;
    const int Loading = 1;
    const int Info = 2;
    const int Warning = 3;
    const int Error = -1;
    const int Critical = -99;
}

void logn(string text, string color)
{
    printf((color+text+ConsoleColor::Reset+(string)"\n").c_str());
}

void log(string text, int status) 
{
    switch (status)
    {
        case LogStatus::Success:
            logn((string)"[o] "+text, ConsoleColor::Green);
            break;
        case LogStatus::Loading:
            logn((string)"[~] "+text, ConsoleColor::Yellow);
            break;
        case LogStatus::Info:
            logn((string)"[^] "+text, ConsoleColor::Magenta);
            break;
        case LogStatus::Warning:
            logn((string)"[*] "+text, ConsoleColor::Cyan);
            break;
        case LogStatus::Error:
            logn((string)"[x] "+text, ConsoleColor::Red);
            break;
        case LogStatus::Critical:
            logn((string)"========================================", ConsoleColor::Red);
            size_t length = (size_t)38-((string)"| "+text).length();
            string spaces(length, ' ');
            logn((string)"| "+text+spaces+(string)" |", ConsoleColor::Red);
            logn((string)"========================================", ConsoleColor::Red);
            break;
    }
}

string GetPath() {
    #ifdef DEBUG
        return "/home/kali/d3m0n_os_debian/rootfs/usr/share/d3m0n";
    #endif // DEBUG
    return "/usr/share/d3m0n";
}

char* GetConfig() {
    #ifdef DEBUG
        return "/home/kali/d3m0n_os_debian/rootfs/usr/share/d3m0n/config";
    #endif // DEBUG
    return "/usr/share/d3m0n/config";
}

string Split(string s, string delimiter) {
    return s.substr(0, s.find(delimiter));
}
string delete_space(string input) {
    input.erase(remove(input.begin(), input.end(), '	'), input.end());
    input.erase(remove(input.begin(), input.end(), ' '), input.end());
    return input;
}
void removeQuotes(string& str) {
    // Check if the string is empty or its length is less than 2
    if (str.empty() || str.length() < 2) {
        return;
    }

    // Check if the first and last characters are double quotes
    if (str.front() == '"' && str.back() == '"') {
        // Erase the first and last characters
        str.erase(0, 1); // Erase first character
        str.pop_back();  // Erase last character
    }
}
void deleteLeadingSpaces(string& str) {
    // Find the position of the first non-space character
    size_t pos = str.find_first_not_of(" \t");

    // Erase leading spaces
    if (pos != string::npos) {
        str = str.substr(pos);
    } else {
        // If string contains only spaces, make it empty
        str.clear();
    }
}

#endif
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <string>
#include <algorithm>
#include <stdexcept>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <limits>
#include <Type.h>
#include <vector>

using namespace std;

// def vars
int SCREEN_WIDTH=240;
int SCREEN_HEIGHT=320;


// debug
#define DEBUG 1

bool isDebugMode() { 
    #ifdef DEBUG
        return true;
    #endif
    return false;
}
uint16_t RGBtoRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
bool isNumber(const std::string& str) {
    std::istringstream iss(str);
    double value;
    iss >> std::noskipws >> value; // Skip white spaces and attempt to read a double

    // Check if the entire string was consumed and if the stream is in a good state
    return iss.eof() && !iss.fail();
}
namespace Colors {
    // colors used are the sames as those in shell
    ushort Red            = RGBtoRGB565(205,49,50);
    ushort DarkRed        = RGBtoRGB565(240,76,75);
    ushort Orange         = RGBtoRGB565(255,135,0);
    ushort DarkOrange     = RGBtoRGB565(255,135,0);
    ushort DarkYellow     = RGBtoRGB565(228,230,15);
    ushort Yellow         = RGBtoRGB565(245,246,67);
    ushort DarkGreen      = RGBtoRGB565(245,246,67);
    ushort Green          = RGBtoRGB565(35,209,138);
    ushort DarkBlue       = RGBtoRGB565(36,114,199);
    ushort Blue           = RGBtoRGB565(59,142,234);
    ushort Cyan           = RGBtoRGB565(42,183,220);
    ushort DarkCyan       = RGBtoRGB565(16,168,205);
    ushort DarkMagenta    = RGBtoRGB565(188,64,187);
    ushort Magenta        = RGBtoRGB565(214,112,214);
    ushort DarkGrey       = RGBtoRGB565(102,102,102);
    ushort Grey           = RGBtoRGB565(121,119,105);
    ushort White          = RGBtoRGB565(229,229,229);
    ushort Reset          = RGBtoRGB565(204,204,204);
    ushort Black          = RGBtoRGB565(0, 0, 0);
};
ushort GetColor(string color) 
{
    try {
        unsigned long ulongValue = stoul(color);

        if (ulongValue > numeric_limits<unsigned short>::max()) {
            throw out_of_range("Value exceeds range of unsigned short");
        }

        return static_cast<unsigned short>(ulongValue);
    } catch (const exception& e) {
        if (color == "black")   return Colors::Black;
        if (color == "dark_grey") return Colors::DarkGrey;
        if (color == "grey")    return Colors::Grey;
        if (color == "white")   return Colors::White;
        if (color == "reset")   return Colors::Reset;
        if (color == "red")     return Colors::Red;
        if (color == "dark_red")  return Colors::DarkRed;
        if (color == "orange")   return Colors::Orange;
        if (color == "dark_orange") return Colors::DarkOrange;
        if (color == "dark_yellow") return Colors::DarkYellow;
        if (color == "yellow")   return Colors::Yellow;
        if (color == "dark_green") return Colors::DarkGreen;
        if (color == "green")   return Colors::Green;
        if (color == "dark_blue") return Colors::DarkBlue;
        if (color == "blue")    return Colors::Blue;
        if (color == "dark_cyan") return Colors::DarkCyan;
        if (color == "cyan")    return Colors::Cyan;
        if (color == "dark_magenta") return Colors::DarkMagenta;
        if (color == "magenta") return Colors::Magenta;
    }
    return Colors::Red; // default is red.
}

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
bool endsWith(const string& str, const string& suffix) {
    if (str.length() < suffix.length()) {
        return false;
    }
    return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool contains(const string& str, const string& substr) {
    return str.find(substr) != string::npos;
}
string Split(const string& str, char delimiter, bool beforeDelimiter = true) {
    size_t pos = str.find(delimiter);
    if (pos != string::npos) {
        if (beforeDelimiter) {
            return str.substr(0, pos);
        } else {
            return str.substr(pos + 1);
        }
    }
    return str; // Return an empty string if delimiter is not found
}
string delete_space(string input) {
    input.erase(remove(input.begin(), input.end(), '	'), input.end());
    input.erase(remove(input.begin(), input.end(), ' '), input.end());
    return input;
}

namespace ConsoleColor {
    const string Reset         = "\e[0m";
    const string Black         = "\e[0;30m";
    const string Red           = "\e[1;31m";
    const string Green         = "\e[1;32m";
    const string Yellow        = "\e[1;33m";
    const string Blue          = "\e[1;34m";
    const string Magenta       = "\e[1;35m";
    const string Cyan          = "\e[1;36m";
    const string White         = "\e[1;37m";
    const string Orange        = "\e[38;5;208m";
    const string DarkRed       = "\e[0;31m";
    const string DarkOrange    = "\e[1;38;5;208m";
    const string DarkYellow    = "\e[0;33m";
    const string DarkGreen     = "\e[0;32m";
    const string DarkBlue      = "\e[0;34m";
    const string DarkCyan      = "\e[0;36m";
    const string DarkMagenta   = "\e[0;35m";
    const string DarkGrey      = "\e[1;30m";
    const string Grey          = "\e[0;30m";

    static string get(string color) {
        color = delete_space(color);
        if(color == "black")        return Black;
        if(color == "orange")       return Orange;
        if(color == "reset")        return Reset;
        if(color == "red")          return Red;
        if(color == "green")        return Green;
        if(color == "yellow")       return Yellow;
        if(color == "blue")         return Blue;
        if(color == "white")        return White;
        if(color == "dark_red")     return DarkRed;
        if(color == "dark_orange")  return DarkOrange;
        if(color == "dark_yellow")  return DarkYellow;
        if(color == "dark_green")   return DarkGreen;
        if(color == "dark_blue")    return DarkBlue;
        if(color == "dark_cyan")    return DarkCyan;
        if(color == "cyan")         return Cyan;
        if(color == "dark_magenta") return DarkMagenta;
        if(color == "magenta")      return Magenta;
        if(color == "dark_grey")    return DarkGrey;
        if(color == "grey")         return Grey;
        return Reset;
    }
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
            logn((string)"======================================================================", ConsoleColor::Red);
            size_t length = (size_t)68-((string)"| "+text).length();
            string spaces(length, ' ');
            logn((string)"| "+text+spaces+(string)" |", ConsoleColor::Red);
            logn((string)"======================================================================", ConsoleColor::Red);
            break;
    }
}

string GetPath() {
    // added these lines for testing purpose
    #ifdef TESTING
        return "/home/d3m0n/d3m0n-testing";
    #endif // TESTING
    #ifdef DEBUG
        return "/home/kali/d3m0n_os_debian/rootfs/usr/share/d3m0n";
    #endif // DEBUG
    return "/usr/share/d3m0n";
}

char* GetConfig() {
    // added these lines for testing purpose
    #ifdef TESTING
        return "/home/d3m0n/d3m0n-testing/config";
    #endif // TESTING
    
    #ifdef DEBUG
        return "/home/kali/d3m0n_os_debian/rootfs/usr/share/d3m0n/config";
    #endif // DEBUG
    return "/usr/share/d3m0n/config";
}

string Split(string s, string delimiter) {
    return s.substr(0, s.find(delimiter));
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
#endif // !UTILS_H
#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
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

// void listDirectory(char* path) {
    // string command = (string)"dir /n /b /s "+path+(string)" > output.txt";
    // system(command.c_str());
    // char ch;
    // std::fstream myStream("output.txt", std::fstream::in);
    // while (myStream.get(ch))
    // {
    //     std::cout << ch;
    // }
// }

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


void FileReadLines(char* path) {
    // ifstream input(path);

    // for(string line; getline(input, line);)
    // {
    //     printf(line.c_str());
    // }
}

string Split(string s, string delimiter) {
    return s.substr(0, s.find(delimiter));
}


#endif
#ifndef LAYOUTHANDLER_H
#define LAYOUTHANDLER_H

#include "../../settings.h"
#include <utils.h>
#include <string>
#include <fstream>
#include <Type.h>
#include <filesystem>
#include <memory>

namespace fs = std::filesystem;
using namespace std;
using namespace DataTypes;

// Forward declaration of Application and Control classes if not defined in settings.h or utils.h
class Application;
// class Control;

namespace Controls {
    const int Window = 0;
	const int Text = 1;
	const int RawHtml = 2;
	const int TextBox = 3;
	const int ListView = 4;
	const int ProgressBar = 5;
	const int CheckBox = 6;
	const int RadioButton = 7;
	const int Rect = 8;
	const int Switch = 9;
	const int Image = 10;
	const int Button = 11;
	const int Vscroll = 12;
	const int Hscroll = 13;
	const int RoundButton = 14;
}

template<typename T>
extern void parseAttributes(T* ctrl, string controlAttributes, void (&function)(T* ctrl2, string key2, string value2));
extern Control* loadControl(string controlName, string controlAttributes);

namespace layoutHandler {
    // Parses the layout file at the given path
    extern void parseLayout(const string& path);
    
    // Runs the layout file for the given application
    extern void runLayout(Application* app, const string& path);
};

#endif // LAYOUTHANDLER_H

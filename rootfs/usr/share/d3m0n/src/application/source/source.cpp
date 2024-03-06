#include "../../settings.h"
#include <utils.h>
#include "script.cpp"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <fstream>
#include <filesystem>
#include <list>

namespace fs = filesystem;
using namespace std;

void WriteSource(string path, string data) {
    ofstream outputFile(path);

    if (!outputFile.is_open()) {
        log("Error opening Event def sourceFile for writing!", LogStatus::Error);
        return;
    }
    outputFile << data.c_str() << endl;
    outputFile.close();
}

void loadEvent(Application app, string eventName, string eventCode)
{
    log("Saving Event: \""+eventName+"\"", LogStatus::Loading);
    system(("mkdir '"+app.temp_path+"/events/'").c_str());
    WriteSource(app.temp_path+"/events/"+eventName, eventCode);
}
void execute(Application app, string line)
{
	string app_path = app.temp_path;
    // printf((line+"\n").c_str());

	if(line[0] == '#')
    {}
    else if(startsWith(line, "def "))
    {
    	// string def_name = line.Replace("def ", "").TrimEnd('}').Split("{")[0];
    	// string def_script = line.Replace("def ", "").TrimEnd('}').Split("{")[1].Replace("  ", "\r\n");
    	// if(!Directory.Exists(app_path+"/defs"))
    	// {
    	// 	Directory.CreateDirectory(app_path+"/defs");
    	// }
    	// File.WriteAllText(app_path+"/defs/"+def_name, def_script);
    	// logn("[o] created def "+def_name, ConsoleColor::Green);
        printf("TODO: def script\n");
    }
    else if(startsWith(line, "use"))
    {
        printf("TODO: use > script\n");
        string path="TODO";
    	// string path = line.Split(">")[1];
        log("using "+path, LogStatus::Info);
    	// script.importLib(getString(path).Replace(";", ""));
    }
    else if(contains(line, "="))
    {
    	string name = delete_space(Split(line, '='));
    	string value = Split(line, '=', false);
        log("Def new var '"+name+"' => '"+value+"'", LogStatus::Warning);
    	// script.setVariable(name, value);
    }
    else if(contains(line, "(") && endsWith(line, ")"))
    {
    	string name = Split(line, '(');
    	string value2 = Split(line, '(', false);
        replaceAll(value2, ")", "");
    	
        std::list<string> value;
        // get two first args
        value.push_back(Split(value2, ','));
        value.push_back(Split(value2, ',', false));

    	script::callFunction(name, value);
        // printf("Calling function %s with args %s\n", name.c_str(), value.front().c_str());
    }   
    	
}
void callEvent(Application app, string eventName)
{
    string eventPath = app.temp_path+"/events/"+eventName;
    if(!filesystem::exists(eventPath))
    {
        log("Event '"+eventName+"' does not exists in app '"+app.name+"'", LogStatus::Error);
        return;
    }
    ifstream inputFile(eventPath);

    if (!inputFile.is_open()) {
        log("Error opening Event def sourceFile for reading!", LogStatus::Error);
        return;
    }

    string line;
    while (getline(inputFile, line)) {
        istringstream iss(line);
        string token;

        // extract  attributes from the attribute string
        while (getline(iss, token, ';')) {
            deleteLeadingSpaces(token);
            execute(app, token);
        }
    }

    inputFile.close();
}
string getString(string raw)
{
	// string app_path = Interpreter.currentRunningAppPath;
	// string ressources = app_path+"/ressources";
	// string source = app_path+"/src";
	// string layouts = app_path+"/layouts";
	// string to_return=raw.TrimStart(' ').TrimStart('"').TrimEnd('"').Replace("$d3m0n", utils.getPath()).Replace("$source", source).Replace("$layouts", layouts).Replace("$ressources", ressources);
	// // utils.logn(to_return, ConsoleColor.Magenta);
	// foreach(string word in to_return.Split(" "))
	// {
	// 	if(word.StartsWith('$') && varExists(word.TrimStart('$')))
	// 	{
	// 		// MessageBox.Show(word);
	// 		to_return=to_return.Replace(word, getVariable(word.TrimStart('$')));
	// 	}
	// }
	return "";
	// return to_return;
}
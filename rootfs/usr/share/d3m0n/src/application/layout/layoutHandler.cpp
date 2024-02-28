#include "../../settings.h"
#include "../../utils.h"
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

namespace fs = filesystem;
using namespace std;

class layoutHandler {
public:
    void parseLayout(string path)
    {
        fstream file(path, ios::in | ios::out); // Open file in read-write mode
        if (!file.is_open()) {
            log("Failed to parse (error while opening file)", LogStatus::Error);
            return;
        }

        stringstream buffer;
        buffer << file.rdbuf(); 
        string fileContent = buffer.str();
       
        // parse into one-liners
        replaceAll(fileContent, ";\r\n", ";");
        replaceAll(fileContent, ":\r\n", ":");
        file.close();


        // write out
        system(((string)"rm -rf '"+path+(string)"' && cat /dev/null > '"+path+"'").c_str());
        fstream file2(path, ios::in | ios::out); // Open file in read-write mode
        if (!file2.is_open()) {
            log("Failed to parse (error while opening file)", LogStatus::Error);
            return;
        }

        file2 << fileContent << std::endl;
        file2.close();
        
        logn(fileContent, ConsoleColor::Green);
        log((string)"Parsed '"+path+(string)"'", LogStatus::Success);
    }
    void runLayout(string path)
    {
        // parsing layout
        parseLayout(path);


        ifstream file(path); 
        if (!file.is_open()) {
            log("Failed to open the layout file.", LogStatus::Error);
            return;
        }

        string line;
        while (getline(file, line)) {
            if(line[0] == '#') continue; // Skip comments
            if(line.empty() || line[0] == '\n' || line[0] == '\r') continue; // Empty lines are skipped as well
            

            
            cout << "Processing line: " << line << endl;
        }

        file.close(); // Close the file

    }
	int width = 240;
	int height = 320;
	short backColor = 0x0;
};

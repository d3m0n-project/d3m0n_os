#include "layoutHandler.h"



namespace layoutHandler {
    void parseLayout(const string& path)
    {
        fstream file(path, ios::in | ios::out); // Open file in read-write mode
        if (!file.is_open()) {
            log("Failed to parse (error while opening file)", LogStatus::Error);
            return;
        }
        stringstream buffer;
        buffer << file.rdbuf(); 
        string fileContent = buffer.str();
        if(startsWith(fileContent, "#alrparsed")) {
            // already parched file
            return;
        }
    
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
        file2 << "#alrparsed (header, do not remove)" << endl << fileContent << endl;
        file2.close();
        
        // logn(fileContent, ConsoleColor::Green);
        log((string)"Parsed '"+path+(string)"'", LogStatus::Success);
    }
    void runLayout(Application* app, const string& path)
    {
        // parsing layout
        layoutHandler::parseLayout(path);
        ifstream file(path); 
        if (!file.is_open()) {
            log("Failed to open the layout file.", LogStatus::Error);
            return;
        }
        string line;
        int line_number = 0;
        while (getline(file, line)) {
            line_number++;
            if(line[0] == '#') continue; // Skip comments
            if(line.empty() || line[0] == '\n' || line[0] == '\r') continue; // Empty lines are skipped as well
            
            string delimiter = ":";
            size_t pos = line.find(delimiter);

            if (pos != string::npos) { // If delimiter found
                // Extract substrings
                string controlName = line.substr(0, pos);
                string controlAttributes = line.substr(pos + delimiter.length());
                
                // loads controls and add it to current window
                Control* control = loadControl(controlName, controlAttributes);
                if (control != nullptr) {
                    app->mainWindow->Add(control); 
                    // log("New CTRL -> " + control->name, LogStatus::Critical);
                }
            } else {
                log("Error in layoutFile '"+path+"', invalid Control definition structure\nCause by '"+line+"' at l."+to_string(line_number), LogStatus::Error);
            }
        }
        file.close(); // Close the file
    }
};

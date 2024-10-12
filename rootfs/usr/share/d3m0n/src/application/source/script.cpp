#include "sourceHandler.h"


void script::callFunction(string name, list<string> args) {
    try {
        if(name == "log") { log(args.front(), LogStatus::Info); return; }
        else if(name == "logn") { logn(args.front(), ConsoleColor::get(args.back())); return; }
        else if(name == "print") { printf((args.front()+"\n").c_str()); return; }
        else if (name == "sleep") { this_thread::sleep_for(chrono::milliseconds(stoi(args.front()))); return;  }
    }
    catch(exception e)
    {
        log("Error while calling Function \""+name+"\": "+e.what(), LogStatus::Error);
    }
}
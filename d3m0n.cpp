#include <stdio.h>
#include "settings.h"
#include <display.cpp>
#include "application/appManager.cpp"
#include <string.h>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <utils.h>
#include <arpa/inet.h>


#define COMMAND_HELP "d3m0n shell - help\n| command                        | permissions | description                      |\n|--------------------------------|-------------|----------------------------------|\n| help                           | none        | display this list                |\n| exit                           | none        | quit                             |\n| config [get/set] name (value)  | admin       | config your phone                |\n| load <path>                    | none        | loads new app                    |\n| list                           | none        | list currently loaded apps       |\n| run <package>                  | none        | run app by package name          |\n| update [all/package_installer] | none        | update d3m0n OS components       |\n| fetch <package>                | none        | download package from repository |\n| clear                          | none        | clear console                    |\n| clean                          | none        | clean scripts & applications     |\n| version                        | none        | get current version              |\n| display [clear/draw]           | none        | manual display debug commands    |\n| info                           | none        | get debug informations           |\n| shutdown                       | none        | stop d3m0n shell                 |"
#define VERSION (string)"1.5 beta"

void ShellHandle(display* display1, appManager manager) {
    // Create socket
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        log("Error creating d3m0n shell server", LogStatus::Error);
        return;
    }
    // Fill in the sockaddr_in structure
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // default port
    int port = 3849;
    try {
        port = stoi(getSetting("shell_port", GetConfig()));
    }
    catch(exception e) {}
    // close service running
    log("Stopping services running on port "+to_string(port), LogStatus::Info);
    system(("sudo fuser -k "+to_string(port)+"/tcp").c_str());
     
    serverAddr.sin_port = htons(port); // Port to listen on

    // Bind socket
    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        log("Error binding d3m0n shell server", LogStatus::Error);
        close(serverSocket);
        return;
    }

    // Listen for incoming connections
    if (listen(serverSocket, 5) == -1) {
        log("Error listening for d3m0n shell connections", LogStatus::Error);
        close(serverSocket);
        return;
    }

    // get server IP
    char srvIP[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(serverAddr.sin_addr), srvIP, INET_ADDRSTRLEN);

    log("Listening on "+(string)srvIP+":"+to_string(port), LogStatus::Error);

    bool serverRunning=true;

    while (serverRunning) {
        int clientSocket = accept(serverSocket, NULL, NULL);
        if (clientSocket == -1) {
            log("Can't accept connection", LogStatus::Error);
            continue;
        }

        // move cursor one line up: \033[F
        // move cursor char 5: \033[5G
        // color \033[38;5;$(echo $iter)m

        char buffer[1024];
        int bytesReceived;

        // Send welcome&login message to client
        string welcomeMessage = "\xE2\x98\xA0  Welcome to the server\n"+log("d3m0n shell loaded, type 'help' to display main commands\n\n", LogStatus::Info)+
        bashGradient(196, "     USERNAME     \n")+
        bashGradient(196, "╔════════════════╗\n")+
        bashGradient(196, "╚> ");

        send(clientSocket, welcomeMessage.data(), welcomeMessage.size(), 0);

        // recieve username
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[bytesReceived] = '\0';
        string username=removeUnwantedChars((string)buffer);
        username=Trim(username);

        printf("requesting password...\n");
        // recieve password
        welcomeMessage = bashGradient(196, "\n     PASSWORD     \n")+
        bashGradient(196, "╔════════════════╗\n")+
        bashGradient(196, "╚> ");
        send(clientSocket, welcomeMessage.data(), welcomeMessage.size(), 0);
        bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0);
        buffer[bytesReceived] = '\0';
        string password=removeUnwantedChars((string)buffer);
        password=Trim(password);
        log("new login => '"+username+":"+password+"'", LogStatus::Info);

        if((string)getSetting(username.data(), (GetPath()+"/configs/users").data()) == password)
        {
            send(clientSocket, "Connected successfully!\n\e[1;32m[d3m0n shell]>\e[0m ", strlen("Connected successfully!\n\e[1;32m[d3m0n shell]>\e[0m "), 0);
        }
        else {
            send(clientSocket, "Invalid username or password\n", strlen("Invalid username or password\n"), 0);
            close(clientSocket);
        }



        bool running=true;
        char command[64];
        string downloader_path = GetPath()+"/d3m0n_package_installer.sh";

        // send(clientSocket, "\e[1;32m[d3m0n shell]>\e[0m ", strlen("\e[1;32m[d3m0n shell]>\e[0m "), 0);

        string output;
        while (running && (bytesReceived = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0) {
            // end carriage
            buffer[bytesReceived] = '\0';
            
            string temp = Trim((string)buffer).data();
            temp.copy(command, std::min(temp.size(), sizeof(command) - 1)); // Ensure null termination
            command[std::min(temp.size(), sizeof(command) - 1)] = '\0'; // Ensure null termination
            
            if(command != "") {
            if(delete_space(command)=="exit") running=false;
            else if(delete_space(command)=="shutdown") { send(clientSocket, "Session closed\n", strlen("Session closed\n"), 0); running=false; serverRunning=false;}
            else if(startsWith(command, "display")) {
                output += display1->displayCommand(command);
            }
            else if(delete_space(command)=="info") { output = printInfos(); }
            else if(delete_space(command)=="version") output = log("Current version: "+VERSION, LogStatus::Info);
            else if(delete_space(command)=="clear") output = exec("clear");
            else if(delete_space(command)=="help") output = logn(COMMAND_HELP, ConsoleColor::DarkGreen);
            else if(delete_space(command)=="list") output = manager.GetAppList();
            else if(command == "clean") { 
                string remove_files = "rm -rf '"+downloader_path+"' && rm -rf '"+GetPath()+"/temp/*'";
                output = log("Cleaning scripts & loaded applications...", LogStatus::Loading);
                manager.appsPackages = {};
                manager.appsPaths = {};
                output += exec(remove_files.c_str());
                output += log("Done!", LogStatus::Success);
            }
            else if(startsWith(command, "update")) {
                string arg1 = Split(command, ' ', false);
                if(arg1 == "package_installer") {
                    output = log("Updating d3m0n package installer...", LogStatus::Loading);
                    int status = system(("rm -rf '"+downloader_path+"' && curl -o '"+downloader_path+"' 'https://raw.githubusercontent.com/d3m0n-project/d3m0n_os/main/rootfs/usr/share/applications/d3m0n_package_installer.sh' && chmod 777 '"+downloader_path+"'").c_str());
                    if(status != 0) { output += log("Update failed!", LogStatus::Error); }
                }
                else {
                    output = log("Usage 'update [all/package_installer]", LogStatus::Error);
                }
            } else if (startsWith(command, "load")) {
                string path = Split(command, ' ', false);
                manager.loadApp(path.data());
                output = log("Loading '"+path+"'...", LogStatus::Loading);
            } else if (startsWith(command, "run")) {
                string package = Split(command, ' ', false);
                Application newApp;
                newApp.temp_path = manager.GetAppPathByPackage(package.data());
                manager.runApp(display1, &newApp);
                output = log("Running app...", LogStatus::Loading);
            } else if (startsWith(command, "fetch")) {

                string package_or_name = Split(command, ' ', false);
                output = logn("   Downloading ==> "+package_or_name, ConsoleColor::Blue); 

                // downloads installer if missing
                if(!filesystem::exists(downloader_path))
                {
                    output += log("Downloading package installer script...", LogStatus::Loading);
                    output += exec(("curl -o '"+downloader_path+"' 'https://raw.githubusercontent.com/d3m0n-project/d3m0n_os/main/rootfs/usr/share/applications/d3m0n_package_installer.sh'").c_str());
                }

                int status = system(("chmod 777 '"+downloader_path+"' &&  '"+downloader_path+"' '"+package_or_name+"'").c_str());
                // display run command
                output += logn("Run this app by loading it using '\e[1;33mload "+GetPath()+"/../applications/"+package_or_name+"\e[38;5;208m' \nand then run it using '\e[1;33mrun "+package_or_name+"\e[38;5;208m'", ConsoleColor::Orange);
                // handle errors
                if(status != 0)
                {
                    output += log("Failed to fetch remote repository: are you offline?", LogStatus::Error);
                }
            }
            else { output = log("Invalid command '"+(string)command+"' (help for list of available commands)", LogStatus::Error); }
            }
            output+="\n\e[1;32m[d3m0n shell]>\e[0m ";
            if(output!="") {
                send(clientSocket, output.c_str(), strlen(output.c_str()), 0);
            }
        }

        // Close client socket
        close(clientSocket);
    }

    close(serverSocket);
}

int main(int argc, char *argv[])
{
    // init path
    string d3m0n_path = GetPath();

    // create configs folder
    system(("sudo mkdir '"+d3m0n_path+"/configs'").c_str());

    // if no users, create one "d3m0n:d3m0n"
    if(!filesystem::exists(d3m0n_path+"/configs/users")) {
        // create user
        // format:  username: password
        // default:    d3m0n: d3m0n
        string users_command = "echo 'd3m0n: d3m0n' > '"+d3m0n_path+"/configs/users'";
        system(users_command.c_str());

    }

    system("clear");
    string command = (string)"rm -rf '"+d3m0n_path+(string)"/temp/' && mkdir '"+d3m0n_path+(string)"/temp/'";
    system(command.c_str());

    // init display
	display *display1;



    // loads apps launcher
    //    load topbar
    //    load background


    // init banner
    logn("    ██████╗ ██████╗ ███╗   ███╗ ██████╗ ███╗   ██╗", ConsoleColor::Reset);
    logn("    ██╔══██╗╚════██╗████╗ ████║██╔═████╗████╗  ██║", ConsoleColor::Reset);
    logn("    ██║  ██║ █████╔╝██╔████╔██║██║██╔██║██╔██╗ ██║", ConsoleColor::Reset);
    logn("    ██║  ██║ ╚═══██╗██║╚██╔╝██║████╔╝██║██║╚██╗██║", ConsoleColor::Reset);
    logn("    ██████╔╝██████╔╝██║ ╚═╝ ██║╚██████╔╝██║ ╚████║", ConsoleColor::Reset);
    logn("    ╚═════╝ ╚═════╝ ╚═╝     ╚═╝ ╚═════╝ ╚═╝  ╚═══╝", ConsoleColor::Reset);
    logn("                  made by 4re5 group              ", ConsoleColor::Reset);
    logn("           the first hacking cellular phone       ", ConsoleColor::Reset);
    printf("\n");
	


    if(isDebugMode()) {
        string input;
        log("DEBUG mode is enabled wan't to continue? [y/N] ", LogStatus::Critical);
        cin >> input;
        if(input == "n" || input == "N")
        {
            return 0;
        }
    }

    sleep(0.5f);
    display1->init(SCREEN_WIDTH, SCREEN_HEIGHT);
    // setup display
    // std::thread displayThread(&display::init, display1, SCREEN_WIDTH, SCREEN_HEIGHT);
    // std::thread wmThread(display1.init(SCREEN_WIDTH, SCREEN_HEIGHT));
    // displayThread.join();
    
    
    display1->drawImage(DataTypes::Point(0, 0), GetPath()+"/splash/splash.png", SCREEN_WIDTH, SCREEN_HEIGHT, false);
    display::PlaySound("boot");
    log("Splash loaded... Waiting "+(string)getSetting("splash_time", GetConfig())+"ms", LogStatus::Loading);
    // setting up splash image
    sleep(stoi(getSetting("splash_time", GetConfig()))/1000);

    // hide splashscreen
    display1->Clear();
    


    // load apps in memory
    appManager manager;
    manager.mainScreenInit(display1);

    
    // display1->drawLine(100, 100, 100, 200, Colors::Red);
    display1->drawRect(10, 10, 100, 100, Colors::Green, false);

    // Application gallery = manager.loadApp("./test/test.d3m0n");
    // manager.runApp(&display1, &gallery);

    // callEvent(&gallery, "Window.OnCreate");

    // gallery.mainWindow->Update();
    // log("Updated!", LogStatus::Success);

    ShellHandle(display1, manager);


    // logn("Name: "+gallery.mainWindow->Controls.at(0)->name, ConsoleColor::Cyan);
    // logn("Width: "+to_string(gallery.mainWindow->Controls.at(0)->Width), ConsoleColor::Cyan);
    // logn("Hight: "+to_string(gallery.mainWindow->Controls.at(0)->Height), ConsoleColor::Cyan);
    // logn("Location.X: "+to_string(gallery.mainWindow->Controls.at(0)->Location.X), ConsoleColor::Cyan);
    // logn("Location.Y: "+to_string(gallery.mainWindow->Controls.at(0)->Location.Y), ConsoleColor::Cyan);
    // logn("BG_Color: "+to_string(gallery.mainWindow->Controls.at(0)->BackColor), ConsoleColor::Cyan);

    // load apps in grid


	printf("\n");

    return 0;
}
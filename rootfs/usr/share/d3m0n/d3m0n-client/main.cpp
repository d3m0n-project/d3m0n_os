#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <codecvt>
#include <locale>
#include <termios.h>
#include <filesystem>
#include "settings.h"
#include <dirent.h>
#include <fstream>
#include <sstream>

// namespace filesystem = filesystem;



#define MAX_BUFFER_SIZE 4096
#define VERSION (string)"1.0"
using namespace std;

#define color1 196
#define color2 208

string lastID;

void writeFile(string path, string content) {
    // Open a file for writing
    ofstream outfile(path);

    // Check if the file was opened successfully
    if (!outfile) {
        cerr << "Error opening file for writing!" << endl;
    }

    // Write to the file
    outfile << content;

    // Close the file
    outfile.close();

    cout << "File written successfully." << endl;
}

char getch() {
    char buf = 0;
    struct termios old = {0};
    fflush(stdout);
    if(tcgetattr(0, &old) < 0) {
        perror("tcsetattr()");
    }
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if(tcsetattr(0, TCSANOW, &old) < 0) {
        perror("tcsetattr ICANON");
    }
    if(read(0, &buf, 1) < 0) {
        perror("read()");
    }
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if(tcsetattr(0, TCSADRAIN, &old) < 0) {
        perror ("tcsetattr ~ICANON");
    }
    return (buf);
}

void bashGradient(int baseColor, string text) {
    int i = baseColor;
    int a = 1;
    string to_return;

    wstring_convert<codecvt_utf8<char32_t>, char32_t> convert;
    u32string ustr = convert.from_bytes(text);

    // iterate over each character in the UTF-32 string
    for (char32_t c : ustr) {
        string s = convert.to_bytes(&c, &c + 1);
        string color_code = "\033[38;5;" + to_string(i) + "m";
        to_return += color_code + s;

        if (i == baseColor) {
            a = 1;
        }
        if (i >= baseColor + 5) {
            a = -1;
        }
        i = i + (1 * a);
    }


    to_return += "\033[0m"; // Reset color at the end of the string
    printf("%s", to_return.c_str());
}

void connectToServer(char ip_address[16], string user="", string pass="") {
    // Create a TCP socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("socket");
        return;
    }

    // Create a sockaddr_in structure for the server address
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(3849); // Port 3849
    if (inet_pton(AF_INET, ip_address, &server_addr.sin_addr) <= 0) {
        printf("Ip address format error: ");
        printf(ip_address);
        printf("\n");
        close(sockfd);
        return;
    }

    // Connect to the server
    if (connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        close(sockfd);
        return;
    }

    // Connection successful, print a message
    cout << "Connected to " << ip_address << ":" << 3849 << endl;

    char buffer[MAX_BUFFER_SIZE];
    bool usernameEntered = false;
    while (true) {
	// Receive response from the server
        ssize_t bytes_received = recv(sockfd, buffer, MAX_BUFFER_SIZE, 0);
        if (bytes_received == -1) {
            perror("recv");
            close(sockfd);
            return;
        }

        // Null-terminate the received data to use it as a C-string
        buffer[bytes_received] = '\0';

        // Display the server's response
        cout << buffer;

        // break if can't login
        if((string)buffer == "Invalid username or password\n" || (string)buffer == "Session closed\n") {
            break;
        }
        // host register
        if((string)buffer == "Connected successfully!\n") {
            // Prompt the save this host
        }
        // username & password save
        // for(int i=0; i<30; i++) {
        //     printf("'");
        //     if(buffer[bytes_received-i] == '>') {
        //         printf((to_string(i)).c_str());
        //         printf("\n");
        //     }
        //     printf((to_string(buffer[bytes_received-i])).c_str());
        //     printf("'\n");
        // }
        
        if (buffer[bytes_received-5] == ' ' && buffer[bytes_received-17] == '>' && user != "" && pass != "") {
            if(usernameEntered) {
                printf(pass.c_str());
                send(sockfd, pass.c_str(), pass.length(), 0);
                printf("\n");
                continue;
            } else {
                printf(user.c_str());
                send(sockfd, user.c_str(), user.length(), 0);
                usernameEntered=true;
                printf("\n");
                continue;
            }
            printf("Auto filled username and password!\n");
        }



        // Prompt the user to enter a command
        string command;
        getline(cin, command);
        if(command == "exit") break;


        // Send the command to the server
        ssize_t bytes_sent = send(sockfd, command.c_str(), command.length(), 0);
        if (bytes_sent == -1) {
            perror("send");
            close(sockfd);
            return;
        }

        if(command == "shutdown") break;
    }

    bashGradient(color1, "d3m0n client finished successfully...\n");

    // Close the socket
    close(sockfd);
}

void about() {
    system("clear");
    string aboutText = "d3m0n client - V"+VERSION+R"(
d3m0n client is a simple client to interract with d3m0n shell for debug purposes.

It is licensed under the MIT license.

Author: 4re5 group
Github: https://github.com/d3m0n-project/d3m0n_os

Website: https://d3m0n-project.github.io/d3m0n_c1
Press any key to continue...)";
    printf(aboutText.c_str());
}

int main() {
    char ch;
    while(true) {
        system("clear");
        if(!filesystem::exists("./data"))  system("mkdir ./data");

        // 196-201, 208-213 bash colors
        printf("\n");
        bashGradient(color1, "     ┓┏┓   ┏┓           d3m0n client - V"+VERSION+R"(
    ┏┫ ┫┏┳┓┃┫┏┓            by 4re5 group       
    ┗┻┗┛┛┗┗┗┛┛┗     https://4re5group.github.io/)");
        printf("\n\n");
        // menu
        bashGradient(color2, "       ⚡ exit       ► 0\n");
        bashGradient(color2, "       ⚡ connect    ► 1\n");
        bashGradient(color2, "       ⚡ hosts      ► 2\n");
        bashGradient(color2, "       ⚡ website    ► 3\n");
        bashGradient(color2, "       ⚡ about      ► 4\n");
        printf("\n");
        // read input
        int choice;
        bashGradient(color2, R"(Enter choice:
╔══════════════════
╚> )");
        ch = getch();
        choice = ch - '0';

        if(choice == 0) break;
        else if(choice == 1) {
            printf("\n");
            // Prompt the user to enter the IP address
            char ip_address[16];
            cout << "Enter the IP address or host id: ";
            cin.getline(ip_address, sizeof(ip_address));
            // check if that's an id
            char* path = ("./data/"+(string)ip_address).data();
            
            if(filesystem::exists(path))
            {
                strcpy(ip_address, getSetting("host", path));
                string user = getSetting("user", path);
                string pass = getSetting("pass", path);
                connectToServer(ip_address, user, pass);
            } else {
                printf("Invalid id.\n");
                connectToServer(ip_address);
            }
            
            printf("Press any key to continue...");
            ch = getch();
        } else if(choice == 2) {
            while(true) {
            system("clear");
string text=R"(    ╔══════════════════════════════════════════════════════════════════════════╗
    ║                               Saved  Hosts                               ║
    ╠════╦══════════════════════╦═════════════╦════════════════════════════════╣
    ║ id ║ host                 ║ date        ║ user:pass                      ║
    ╠════╬══════════════════════╬═════════════╬════════════════════════════════╣
)";
            string directory_path = "./data/";
            string host;
            string date;
            string user;
            string pass;
            string userpluspass;
            string id="0";
            stringstream ss;
            char* path;

            // Iterate through the files in the directory
            for (const auto& entry : filesystem::directory_iterator(directory_path)) {
                if (filesystem::is_regular_file(entry)) {
                    ss=stringstream();
                    path=entry.path().string().data();
                    host = getSetting("host", path);
                    date = getSetting("date", path);
                    user = getSetting("user", path);
                    pass = getSetting("pass", path);
                    userpluspass = user+":"+pass;
                    filesystem::path path_obj(entry);
                    id=path_obj.filename().string();
                    // Pad the input string and store it in the stringstream
                    ss << left << setw(20) << setfill(' ') << host;
                    host = ss.str();
                    // reset stream
                    ss=stringstream();
                    ss << left << setw(11) << setfill(' ') << date;
                    date = ss.str();
                    // reset stream
                    ss=stringstream();
                    ss << left << setw(30) << setfill(' ') << userpluspass;
                    userpluspass = ss.str();
                    // reset stream
                    ss=stringstream();
                    ss << right << setw(2) << setfill('0') << id;
                    id = ss.str();
                    text+="    ║ "+id+" ║ "+host+" ║ "+date+" ║ "+userpluspass+" ║\n";
                }
            }
            text+="    ╚════╩══════════════════════╩═════════════╩════════════════════════════════╝\n";
            bashGradient(color2, text);

            // set last id
            lastID = id;


            bashGradient(color2, R"(
       ⚡ back       ► 0
       ⚡ add host   ► 1
       ⚡ edit host  ► 2
       ⚡ erase host ► 3

)");
            choice;
            bashGradient(color2, R"(Enter choice:
    ╔══════════════════
    ╚> )");
            ch = getch();
            printf("\n");
            choice = ch - '0';
            // exit
            if(choice == 0) {
                break;
            } 
            // add host
            else if (choice == 1)
            {
                string ip_address;
                string date;
                string username;
                string password;
                time_t current_time = time(nullptr);
                // Convert to local time
                tm* local_time = localtime(&current_time);
                // Format the date as "mm/dd/yyyy"
                char formatted_date[11]; // Length should be at least 11 to hold the formatted date
                strftime(formatted_date, sizeof(formatted_date), "%m/%d/%Y", local_time);
                date=(string)formatted_date;

                system("clear");
                cout << "Enter new host's ip address: ";
                getline(cin, ip_address);
                printf("\n");
                cout << "Username: ";
                getline(cin, username);
                printf("\n");
                cout << "Password: ";
                getline(cin, password);
                // cin.getline(password, sizeof(password));
                string tempID= to_string(stoi(lastID)+1);
                ss=stringstream();
                ss << right << setw(2) << setfill('0') << tempID;
                tempID = ss.str();
                writeFile("./data/"+tempID, "host: "+ip_address+"\ndate: "+date+"\nuser: "+username+"\npass: "+password);
            }
            // edit host
            else if(choice == 2) {
                string edit_id;
                string date;
                time_t current_time = time(nullptr);
                // Convert to local time
                tm* local_time = localtime(&current_time);
                // Format the date as "mm/dd/yyyy"
                char formatted_date[11]; // Length should be at least 11 to hold the formatted date
                strftime(formatted_date, sizeof(formatted_date), "%m/%d/%Y", local_time);
                date=(string)formatted_date;

                cout << "Enter host's id: ";
                getline(cin, edit_id);
                char* path = ("./data/"+edit_id).data();
                string ip_address = getSetting("host", path);
                string username = getSetting("user", path);
                string password = getSetting("pass", path);

                string temp;

                system("clear");
                if(!filesystem::exists("./data/"+edit_id)) { printf("Invalid id.\nPress any key to continue..."); getch(); continue; }
                // host
                cout << "New host ['"+ip_address+"', null to pass]: ";
                getline(cin, temp);
                if(temp.length() > 0) ip_address = temp;
                // username
                cout << "New username ['"+username+"', null to pass]: ";
                getline(cin, temp);
                if(temp.length() > 0) username = temp;
                // password
                cout << "New password ['"+password+"', null to pass]: ";
                getline(cin, temp);
                if(temp.length() > 0) password = temp;
                
                writeFile("./data/"+edit_id, "host: "+ip_address+"\ndate: "+date+"\nuser: "+username+"\npass: "+password);
            }
            else if(choice == 3) {
                string edit_id;
                string temp;

                cout << "Enter host's id: ";
                getline(cin, edit_id);
                string path = "./data/"+edit_id;

                system("clear");
                if(!filesystem::exists("./data/"+edit_id)) { printf("Invalid id.\nPress any key to continue..."); getch(); continue; }

                cout << "Are you sure to delete host #"+edit_id+" [Y/N]? ";
                getline(cin, temp);
                if(temp == "y" || temp == "Y") {
                    system(("rm -rf '"+path+"'").c_str());
                    printf("Deleted successfully!\n");
                } else { 
                    printf("Aborted.\n");
                }

                getch();
            }
            }
        } else if(choice == 3) {
            system("clear");
            bashGradient(color2, "► https://github.com/d3m0n-project/d3m0n_os\n");
            printf("Press any key to continue...");
            getch();
        }  else if(choice == 4) {
            about();
            getch();
        }
    }
    printf("bye.\n");
    return 0;
}
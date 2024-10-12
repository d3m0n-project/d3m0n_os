#include "utils.h"

using namespace std;

int SCREEN_WIDTH=240;
int SCREEN_HEIGHT=320;
// debug window name
string window_name = "d3m0n emulator";

namespace Colors {
    // colors used are the sames as those in shell
    ushort Red            = RGBtoRGB565(255, 0, 0);
    ushort DarkRed        = RGBtoRGB565(139, 0, 0);
    ushort Orange         = RGBtoRGB565(255, 165, 0);
    ushort DarkOrange     = RGBtoRGB565(255, 140, 0);
    ushort DarkYellow     = RGBtoRGB565(255, 215, 0);
    ushort Yellow         = RGBtoRGB565(255, 255, 0);
    ushort DarkGreen      = RGBtoRGB565(0, 100, 0);
    ushort Green          = RGBtoRGB565(0, 128, 0);
    ushort DarkBlue       = RGBtoRGB565(0, 0, 139);
    ushort Blue           = RGBtoRGB565(0, 0, 255);
    ushort Cyan           = RGBtoRGB565(0, 255, 255);
    ushort DarkCyan       = RGBtoRGB565(0, 139, 139);
    ushort DarkMagenta    = RGBtoRGB565(139, 0, 139);
    ushort Magenta        = RGBtoRGB565(255, 0, 255);
    ushort DarkGrey       = RGBtoRGB565(169, 169, 169);
    ushort Grey           = RGBtoRGB565(128, 128, 128);
    ushort White          = RGBtoRGB565(255, 255, 255);
    ushort Reset          = RGBtoRGB565(204, 204, 204);
    ushort Black          = RGBtoRGB565(0, 0, 0);
};

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

    string get(string color) {
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

// struct Application
// {
//     string name;
//     string description;
//     string package;
//     string icon;
//     string perms;
//     string start_path;
//     string category;
//     string temp_path;
//     vector<DataTypes::Window*> windows;
//     DataTypes::Window* mainWindow;
// };

namespace LogStatus {
    const int Success = 0;
    const int Loading = 1;
    const int Info = 2;
    const int Warning = 3;
    const int Error = -1;
    const int Critical = -99;
}

bool isTestingMode() { 
    #ifdef TESTING
        return true;
    #endif
    return false;
}

bool isDebugMode() { 
    #ifdef DEBUG
        return true;
    #endif
    return false;
}


string bashColor(int color) {
    return "\033[38;5;"+to_string(color)+"m";
}
void RGB565toRGB(uint16_t rgb565, uint8_t& r, uint8_t& g, uint8_t& b) {
    // Extract red, green, and blue components from RGB565
    r = (rgb565 >> 8) & 0xF8;   // 5 bits for red
    g = (rgb565 >> 3) & 0xFC;   // 6 bits for green
    b = (rgb565 << 3) & 0xF8;   // 5 bits for blue

    // Scale components to 8-bit range (0-255)
    // r >>= 3;    // Scale 5-bit value to 8-bit range
    // g >>= 2;    // Scale 6-bit value to 8-bit range
    // b >>= 3;    // Scale 5-bit value to 8-bit range


}
uint16_t RGBtoRGB565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}
bool isNumber(const string& str) {
    istringstream iss(str);
    double value;
    iss >> noskipws >> value; // Skip white spaces and attempt to read a double

    // Check if the entire string was consumed and if the stream is in a good state
    return iss.eof() && !iss.fail();
}

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
string Split(const string& str, char delimiter, bool beforeDelimiter) {
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



bool readAllBytes(const string& filePath, vector<unsigned char>& data) {
    // Open the file in binary mode
    ifstream file(filePath, ios::binary | ios::ate);
    if (!file.is_open()) {
        cerr << "Failed to open file: " << filePath << endl;
        return false;
    }

    // Get the size of the file
    streamsize fileSize = file.tellg();
    if (fileSize < 0) {
        cerr << "Failed to get file size." << endl;
        return false;
    }

    // Resize the vector to hold all bytes
    data.resize(fileSize);

    // Seek back to the beginning of the file
    file.seekg(0, ios::beg);
    if (file.fail()) {
        cerr << "Failed to seek to beginning of file." << endl;
        return false;
    }

    // Read the entire file into the vector
    file.read(reinterpret_cast<char*>(data.data()), fileSize);
    if (file.fail()) {
        cerr << "Failed to read data from file." << endl;
        return false;
    }

    // Close the file
    file.close();
    return true;
}

string logn(string text, string color)
{
    printf((color+text+ConsoleColor::Reset+(string)"\n").c_str());
    return(color+text+ConsoleColor::Reset+(string)"\n"); 
}

string log(string text, int status) 
{
    switch (status)
    {
        case LogStatus::Success:
            return logn((string)"[o] "+text, ConsoleColor::Green);
            break;
        case LogStatus::Loading:
            return logn((string)"[~] "+text, ConsoleColor::Yellow);
            break;
        case LogStatus::Info:
            return logn((string)"[^] "+text, ConsoleColor::Magenta);
            break;
        case LogStatus::Warning:
            return logn((string)"[*] "+text, ConsoleColor::Cyan);
            break;
        case LogStatus::Error:
            return logn((string)"[x] "+text, ConsoleColor::Red);
            break;
        case LogStatus::Critical:
            string tmp="";
            int totalLen = 100;
            size_t length = (size_t)totalLen-((string)"║ "+text).length();

            string bar="";
            for (int i = 0; i < totalLen-2; i++) {
                bar+="═";
            }
            tmp+=logn("╔"+bar+"╗", ConsoleColor::Red);


            string spaces(length, ' ');
            tmp+=logn((string)"║ "+text+spaces+(string)" ║", ConsoleColor::Red);
            return tmp+logn((string)"╚"+bar+"╝", ConsoleColor::Red);
            break;
    }
    return "";
}
string exec(const char* cmd) {
    array<char, 128> buffer;
    string result;
    unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}
string Trim(string str) {
    str.erase(remove(str.begin(), str.end(), '\n'), str.cend());
    str.erase(remove(str.begin(), str.end(), '\r'), str.cend());
    return str;
}

string removeUnwantedChars(string input) {
    // Define regular expression pattern to match unwanted characters
    // regex pattern("[^a-zA-Z0-9[:space:][:punct:]]"); // Matches any character that is not a letter, number, space, or punctuation
    
    string::size_type i = 0;
    while (i < input.length()) {
        i = input.find('\n', i);
        if (i == string::npos) {
            break;
        }
        input.erase(i);
    }
    return input;

    // Replace unwanted characters with an empty string
    // return regex_replace(input, pattern2, "");
    // return regex_replace(input, pattern, "");
}

string GetAppPath() {
    // added these lines for testing purpose
    #ifdef TESTING
        return "/home/d3m0n/d3m0n-testing-applications";
    #endif // TESTING
    #ifdef DEBUG
        return "/home/kali/d3m0n_os_debian/rootfs/usr/share/applications";
    #endif // DEBUG
    return "/usr/share/applications";
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
string printInfos() {
    string to_return="";
    // displays system infos
    to_return+=logn("d3m0n OS - system informations", ConsoleColor::Cyan);
    to_return+=logn("# CREDITS", ConsoleColor::DarkCyan);
    to_return+=logn("  4re5 group - 2024 @ all rights reserved", ConsoleColor::Cyan);
    to_return+="\n";
    to_return+=logn("# DEBUG DATA", ConsoleColor::DarkCyan);
    to_return+=logn("  DEBUG                     ==>  '"+(string)(isDebugMode()?"true":"false")+"'", ConsoleColor::Cyan);
    to_return+=logn("  TESTING                   ==>  '"+(string)(isTestingMode()?"true":"false")+"'", ConsoleColor::Cyan);
    to_return+=logn("  d3m0n path                ==>  '"+(string)GetPath()+"'", ConsoleColor::Cyan);
    to_return+=logn("  d3m0n config path         ==>  '"+(string)GetConfig()+"'", ConsoleColor::Cyan);
    to_return+="\n";
    to_return+=logn("# ONLINE RESSOURCES", ConsoleColor::DarkCyan);
    to_return+=logn("  github repository         ==>  'https://github.com/d3m0n-project/d3m0n_os'", ConsoleColor::Cyan);
    to_return+=logn("  shell documentaion        ==>  'https://github.com/d3m0n-project/d3m0n_os/tree/main/rootfs/usr/share/d3m0n/documentation/shell'", ConsoleColor::Cyan);
    to_return+=logn("  api documentaion          ==>  'https://github.com/d3m0n-project/d3m0n_os/blob/main/rootfs/usr/share/d3m0n/documentation/api.md'", ConsoleColor::Cyan);
    to_return+=logn("  forum                     ==>  'https://forum.ducksploit.com'", ConsoleColor::Cyan);
    to_return+=logn("  discord server            ==>  'https://discord.com/invite/sTVWespH4M'", ConsoleColor::Cyan);
    
    
    return to_return;
}
u16string to_u16string(const string& str) {
    wstring_convert<codecvt_utf8<char16_t>, char16_t> converter;
    return converter.from_bytes(str);
}
string bashGradient(int baseColor, string text) {
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


    return to_return + "\033[0m"; // Reset color at the end of the string
}
vector<char> stringToUtf8(const string& str) {
    vector<char> utf8Bytes;
    for (char c : str) {
        if ((c & 0x80) == 0) {
            // ASCII character (0xxxxxxx)
            utf8Bytes.push_back(c);
        } else if ((c & 0xE0) == 0xC0) {
            // 2-byte UTF-8 character (110xxxxx 10xxxxxx)
            utf8Bytes.push_back(0xC0 | ((c >> 6) & 0x1F));
            utf8Bytes.push_back(0x80 | (c & 0x3F));
        } else if ((c & 0xF0) == 0xE0) {
            // 3-byte UTF-8 character (1110xxxx 10xxxxxx 10xxxxxx)
            utf8Bytes.push_back(0xE0 | ((c >> 12) & 0x0F));
            utf8Bytes.push_back(0x80 | ((c >> 6) & 0x3F));
            utf8Bytes.push_back(0x80 | (c & 0x3F));
        } else if ((c & 0xF8) == 0xF0) {
            // 4-byte UTF-8 character (11110xxx 10xxxxxx 10xxxxxx 10xxxxxx)
            utf8Bytes.push_back(0xF0 | ((c >> 18) & 0x07));
            utf8Bytes.push_back(0x80 | ((c >> 12) & 0x3F));
            utf8Bytes.push_back(0x80 | ((c >> 6) & 0x3F));
            utf8Bytes.push_back(0x80 | (c & 0x3F));
        }
    }
    return utf8Bytes;
}

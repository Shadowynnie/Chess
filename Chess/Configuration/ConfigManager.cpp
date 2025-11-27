#include "ConfigManager.h"
#include <fstream>
#include <sstream>
#include <iostream>

using std::string;
// -------- Default values --------
string ConfigManager::serverIP = "127.0.0.1";
unsigned short ConfigManager::serverPort = 7777;

bool ConfigManager::Load(const string& path)
{
    std::ifstream f(path);
    if (!f.is_open())
    {
        std::cerr << "ConfigManager: Could not open " << path << ". Using defaults.\n";
        return false;
    }

    string line;
    while (std::getline(f, line))
    {
        std::istringstream iss(line);
        string key, eq, value;
        if (!(iss >> key >> eq >> value))
            continue;
        if (eq != "=")
            continue;

        if (key == "serverIP")
            serverIP = value;
        else if (key == "serverPort")
            serverPort = std::stoi(value);
    }
    return true;
}

bool ConfigManager::Save(const string& path)
{
    std::ofstream f(path);
    if (!f.is_open())
    {
        std::cerr << "ConfigManager: Could not save " << path << "\n";
        return false;
    }
    f << "serverIP = " << serverIP << "\n";
    f << "serverPort = " << serverPort << "\n";
    return true;
}

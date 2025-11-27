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

bool ConfigManager::IsValidIPv4(const std::string& ip)
{
    int parts = 0;
    int num = -1;

    for (size_t i = 0; i <= ip.size(); ++i)
    {
        if (i == ip.size() || ip[i] == '.')
        {
            if (num < 0 || num > 255)
                return false;
            parts++;
            num = -1;
        }
        else if (isdigit(ip[i]))
        {
            if (num < 0)
                num = 0;
            num = num * 10 + (ip[i] - '0');
            if (num > 255)
                return false;
        }
        else return false;
    }

    return parts == 4;
}

bool ConfigManager::IsValidPort(const std::string& s)
{
    if (s.empty()) return false;
    for (char c : s) if (!isdigit(c)) return false;

    int p = std::stoi(s);
    return p >= 0 && p <= 65535;
}
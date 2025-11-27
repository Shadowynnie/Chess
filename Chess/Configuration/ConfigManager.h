#pragma once
#include <string>
#include "Textbox.h"

class ConfigManager
{
public:
    // Global settings
    static std::string serverIP;
    static unsigned short serverPort;

    // Load / Save whole config file
    static bool Load(const std::string& path = "Settings.cfg");
    static bool Save(const std::string& path = "Settings.cfg");
};

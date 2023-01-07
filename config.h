#pragma once
#include <string>

struct Config {
    std::string dbHost;
    std::string dbPort;
    std::string dbName;
    std::string username;
    std::string password;

    std::string bindAddr;
    uint16_t port;
};

extern Config config;

void setup();
void tryToCreateConfig();
void populateConfigFromFile(std::ifstream& config);
std::string readConfigLine(std::ifstream& config, const std::string& key);
std::uint16_t getPort(std::ifstream& inputConfig);

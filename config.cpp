#include "config.h"
#include <iostream>
#include <string>
#include "database.h"

void setup() {
    while (true) {
        std::ifstream configFile("config.txt");

        // If the config file does not exist, attempt to create it
        if (!configFile) {
            tryToCreateConfig();

            configFile.open("config.txt");
            if (!configFile) {
                std::cout << "The config file still could not be opened. Quitting the program." << std::endl;
                throw std::runtime_error("Unable to create or read the configuration file");
            }
        }

        populateConfigFromFile(configFile);
        if (config.bindAddr.empty() || config.port == 0 || database.tryToConnect(config)) {
            std::string input;
            do {
                std::cout << "There is an issue with the configuration or connecting to the database. Enter 'y' to " \
                    "reload the configuration and try again, 'n' to quit with an error, or 'w' to save a new blank " \
                    "configuration file: ";
                std::cin >> input;
            } while (input != "y" && input != "n" && input != "w");

            if (input == "n") {
                throw std::runtime_error("Error with the configuration or internet connection.");
            } else if (input == "w") {
                configFile.close();
                tryToCreateConfig();
            }
        } else {
            // Success! The config file opened successfully and a database connection was created
            configFile.close();
            break;
        }
    }
}

void tryToCreateConfig() {
    std::string userInput;
    std::cout << "Unable to find a config file. A config file should exist in the same directory as the " \
        "executable." << std::endl;

    do {
        std::cout << "Enter y to create the file or q to quit.";
        std::cin >> userInput;
    } while (userInput != "y" && userInput != "n");

    if (userInput == "q") {
        return;
    }

    std::ofstream configOutput("config.txt");
    if (!configOutput) {
        std::cout << "Error creating a new configuration. Do you have the required permissions? Quitting now.";
        throw std::runtime_error("The configuration could not be created.");
    }

    configOutput.clear();
    configOutput << "Configuration File" << std::endl;
    configOutput << "Database Host: " << std::endl;
    configOutput << "Database Port: " << std::endl;
    configOutput << "Database Name: " << std::endl;
    configOutput << "Database Username: " << std::endl;
    configOutput << "Database Password: " << std::endl;
    configOutput << "Bind Address: " << std::endl;
    configOutput << "Server Port: ";
    configOutput.close();

    std::cout << "Press enter once you have entered values in the config file.";
    std::cin >> userInput;
    std::cout << std::endl;
}

void populateConfigFromFile(std::ifstream& configFile) {
    readConfigLine(configFile, "Configuration File");
    config.dbHost = readConfigLine(configFile, "Database Host");
    config.dbPort = readConfigLine(configFile, "Database Port");
    config.dbName = readConfigLine(configFile, "Database Name");
    config.username = readConfigLine(configFile, "Database Username");
    config.password = readConfigLine(configFile, "Database Password");
    config.bindAddr = readConfigLine(configFile, "Bind Address");
    config.port = getPort(configFile);
    config.dbHost = readConfigLine(configFile, "Database Host");
}

// Gets the value from a line in the config with the format <key>:<value> and validates the key matches the expected
// value. An empty string indicates that the key could not be found, the value was blank, or there was another IO error
std::string readConfigLine(std::ifstream& inputConfig, const std::string& key) {
    std::string line;
    getline(inputConfig, line);  // A failure will set badbit or failbit in the input stream

    // Checks if the read operation failed
    if (!inputConfig.good()) {
        return "";
    }

    unsigned long startLocation = line.find(key + ":");  // This should be 0
    unsigned long keyLength = key.length() + 1;  // The +1 is for the colon

    if (startLocation != std::string::npos) {
        if (line.at(keyLength) == ' ') {
            return line.substr(startLocation + keyLength + 1);
        } else {
            return line.substr(startLocation + keyLength);
        }
    } else {
        return "";
    }
}

std::uint16_t getPort(std::ifstream& inputConfig) {
    std::string portAsStr = readConfigLine(inputConfig, "Server Port");
    if (portAsStr.empty()) {
        return 0;
    }

    try {
        int port = std::stoi(portAsStr);

        if (0 < port and port < std::numeric_limits<std::uint16_t>::max()) {
            return port;
        } else {
            return 0;
        }
    } catch (std::invalid_argument& exception) {
        return 0;  // General parse error
    } catch (std::out_of_range& exception) {
        return 0;  // The number in the file was too large
    }
}

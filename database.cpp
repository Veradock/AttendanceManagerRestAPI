#include <fstream>
#include <string>
#include <crow.h>
#include "database.h"

// The single instance used to communicate with the SQL attendance database
Database database;

/*
 * General database connectivity methods
 */

// You must free the sql::Statement, sql::Connection, and sql::ResultSet objects explicitly using delete.
Database::~Database() {
    if (statement != nullptr) {
        statement->close();
        delete statement;
    }

    if (connection != nullptr) {
        connection->close();
        delete connection;
    }
}

Database::Database() noexcept = default;

// Tries to open a connection to the database with the given credentials
bool Database::tryToConnect(const std::string& host, const std::string& port, const std::string& databaseName,
                            const std::string& username,const std::string& password) {
    delete this->connection;

    sql::mysql::MySQL_Driver *driver = sql::mysql::get_driver_instance();
    try {
        this->connection = driver->connect(host + ":" + port, username, password);
        this->connection->setSchema(databaseName);
        this->statement = this->connection->createStatement();
    } catch (sql::SQLException& exception) {
        return false;
    }

    return this->connection->isValid();
}

// Checks if there is a working connection to the database. Attempts to reopen the connection if it is closed
bool Database::ensureLiveConnection() const {
    if (this->statement == nullptr) {
        return false;
    } else {
        if (!this->connection->isValid()) {
            this->connection->reconnect();
            return this->connection->isValid();
        } else {
            return true;
        }
    }
}

// Creates a connection to the database in the global database variable using credentials in the config.txt file
bool Database::createDatabaseConnection() {
    // Try to load the configuration file
    std::ifstream config("config.txt");
    Database::emptyEncodings.open("EmptyEncodings.dat");
    if(!config) {
        // The configuration either does not exist or cannot be read. Try to create a new file
        std::cout << "Unable to find a configuration file! Creating a new file now." << std::endl;
        std::ofstream outputConfig("config.txt");

        outputConfig << "Configuration File" << std::endl;
        outputConfig << "Host: " << std::endl;
        outputConfig << "Port: " << std::endl;
        outputConfig << "Database name: " << std::endl;
        outputConfig << "Username: " << std::endl;
        outputConfig << "Password: ";

        outputConfig.close();
        config.open("config.txt");
        if (!config) {
            // Give up on trying to open the database connection – the configuration cannot be found
            return false;
        }
    }
    readConfigLine(config, "Configuration File");  // Read the first line

    this->tryToConnect(readConfigLine(config, "Host"), readConfigLine(config, "Port"),
                                  readConfigLine(config, "Database name"), readConfigLine(config, "Username"),
                                  readConfigLine(config, "Password"));
    return this->ensureLiveConnection();
}

/*
 * Helper functions
 */

// Gets the value from a line in the config with the format <key>:<value> and validates the key matches the expected value
std::string readConfigLine(std::ifstream& config, const std::string& key) {
    std::string line;
    getline(config, line);

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

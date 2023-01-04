#pragma once
#include <string>
#include <mysql/jdbc.h>

// The database class
class Database {
public:
    sql::Statement *statement = nullptr;
    sql::Connection *connection = nullptr;
    std::ifstream emptyEncodings;

    Database() noexcept;
    ~Database();

    bool tryToConnect(const std::string& host, const std::string& port, const std::string& databaseName,
                            const std::string& username, const std::string& password);
    bool ensureLiveConnection() const;
    bool createDatabaseConnection();
};

// The single instance used to communicate with the SQL attendance database
extern Database database;

// Helpers
std::string readConfigLine(std::ifstream& config, const std::string& key);
void sendDatabaseErrorResponse(crow::response& response);

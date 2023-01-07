#pragma once
#include <string>
#include <mysql/jdbc.h>
#include <crow.h>
#include "config.h"

// The database class
class Database {
public:
    sql::Statement *statement = nullptr;
    sql::Connection *connection = nullptr;
    std::ifstream emptyEncodings;

    Database() noexcept;
    ~Database();

    bool tryToConnect(const Config& configuration);
    bool ensureLiveConnection() const;
};

// The single instance used to communicate with the SQL attendance database
extern Database database;

// Helpers
void sendDatabaseErrorResponse(crow::response& response);

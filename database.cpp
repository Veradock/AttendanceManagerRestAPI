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
bool Database::tryToConnect(const Config& configuration) {
    delete this->connection;

    sql::mysql::MySQL_Driver *driver = sql::mysql::get_driver_instance();
    try {
        this->connection = driver->connect(configuration.dbHost + ":" + configuration.dbPort,
                                           configuration.username, configuration.password);
        this->connection->setSchema(configuration.dbName);
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

#include <mysql/jdbc.h>
#include <crow.h>
#include "requesthandler.h"

/* TODO list:
 * Fix cmake to support building on multiple platforms
 * Improve documentation
 * Make sure the GET functions are protected from sql injection
 * Write unit tests
 * Test the POST functions
 * Create some sort of config to save server parameters
 * Add more error checking, especially:
 *      - around the config file
 *      - around the EmptyEncodings.dat file
 *      - input from POST commands
 *      - all database operations
 */

void runServer();

int main() {
    std::cout << "REST API for roster management software" << std::endl << "Created by Sandbox and the Student Government Association operations team" << std::endl;
    if (!database.createDatabaseConnection()) {
        std::cout << "The database connection could not be created! Please check the database credentials and the "
                  << "network connection and run the program again" << std::endl;
        return -1;
    } else {
        std::cout << "The database connection has been opened successfully." << std::endl;
    }

    runServer();  // Run forever – the program will exit gracefully on SIGINT signal
    std::cout << "Web server closed successfully. Have a good day!";
    return 0;
}

void runServer() {
    crow::SimpleApp app;
    // TODO port and bind address should be set
    app.port(81);

    // Register the routes
    CROW_ROUTE(app, "/persons").methods(crow::HTTPMethod::GET)(getAllPeople);
    CROW_ROUTE(app, "/persons/create").methods(crow::HTTPMethod::POST)(postCreateNewPerson);
    CROW_ROUTE(app, "/persons/<string>").methods(crow::HTTPMethod::GET)(getSinglePerson);
    CROW_ROUTE(app, "/persons/update").methods(crow::HTTPMethod::POST)(postUpdatePerson);

    CROW_ROUTE(app, "/events").methods(crow::HTTPMethod::GET)(getAllEvents);
    CROW_ROUTE(app, "/events/create").methods(crow::HTTPMethod::POST)(postCreateNewEvent);
    CROW_ROUTE(app, "/events/<string>").methods(crow::HTTPMethod::GET)(getSingleEvent);
    CROW_ROUTE(app, "/events/update").methods(crow::HTTPMethod::POST)(postUpdateEvent);

    CROW_ROUTE(app, "/attendance-change").methods(crow::HTTPMethod::GET)(getAllRequests);
    CROW_ROUTE(app, "/attendance-change/create").methods(crow::HTTPMethod::POST)(postCreateNewRequest);
    CROW_ROUTE(app, "/attendance-change/<string>/").methods(crow::HTTPMethod::GET)(getSingleRequest);
    CROW_ROUTE(app, "/attendance-change/update").methods(crow::HTTPMethod::POST)(postUpdateRequest);

    app.run();  // This is a blocking call that runs the server. SIGINT signal will cause it to end gracefully
    std::cout << "Closing the webserver" << std::endl;
    app.stop();
}

/*
 * Code to generate the EmptyEncodings.dat file
 *
 * sql::ResultSet* result = database.statement->executeQuery("SELECT encodings FROM Member WHERE uuid = '022cf6c5216a499aa826a9d059926ff9';");
 * result->next();
 * std::ofstream emptyEncodings("EmptyEncodings.dat");
 * emptyEncodings.clear();
 * std::istream* streamFromDB = result->getBlob(1);
 * emptyEncodings << streamFromDB->rdbuf();
 * emptyEncodings.close();
 */
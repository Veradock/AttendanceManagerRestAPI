#include <mysql/jdbc.h>
#include <crow.h>
#include "requesthandler.h"
#include "config.h"

/* TODO list:
 * Improve documentation
 * Write unit tests
 * Test the POST functions
 * Add more error checking, especially:
 *      - around the EmptyEncodings.dat file
 *      - input from POST commands
 *      - all database operations
 */

void runServer();
Config config;

int main() {
    std::cout << "REST API for roster management software" << std::endl << "Created by Sandbox and the Student Government Association operations team" << std::endl;
    setup();
    std::cout << "The configuration was loaded successfully and the database connection was opened!" << std::endl;

    runServer();  // Run forever – the program will exit gracefully on SIGINT signal
    std::cout << "Web server closed successfully. Have a good day!";
    return 0;
}

void runServer() {
    // Create the app
    crow::SimpleApp app;
    app.port(config.port);
    app.bindaddr(config.bindAddr);

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
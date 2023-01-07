#include "requesthandler.h"

/*
 * GET command processors
 */

// Sends a JSON to the requester with all member data, excluding encodings
void getAllPeople(crow::response& response) {
    tryToSendJson([](crow::json::wvalue& resultsJson) {
        // Gets the results from the database
        sql::ResultSet *infoResult = database.statement->executeQuery("SELECT first_name, last_name, email, " \
            "active_member, voting_rights, receive_not_present_email, include_in_quorum, sign_in_blocked, " \
            "uuid FROM Member;");
        sql::ResultSet *groupResults = database.statement->executeQuery("SELECT * FROM MemberGroup;");
        sql::ResultSet *attendanceResults = database.statement->executeQuery("SELECT * FROM AttendanceRecord;");

        while (infoResult->next()) {
            createPersonJson(infoResult, resultsJson, infoResult->getString(9));
        }
        delete infoResult;

        // Add the groups list
        addListToJson(1, 2, groupResults, resultsJson, "groups");
        delete groupResults;

        // Adds the attendance record to the json
        while (attendanceResults->next()) {
            resultsJson[attendanceResults->getString(1)]["attendance_record"] \
            [attendanceResults->getString(2)] = attendanceResults->getString(3);
        }
        delete attendanceResults;
    }, response);
}

// Sends a single person's info to the requester with all member data, excluding encodings
void getSinglePerson(crow::response& response, const std::string& uuid) {
    tryToSendJson([&uuid](crow::json::wvalue& resultsJson) {
        // Gets the results from the database
        sql::PreparedStatement* getInfoStatement = database.connection->prepareStatement(
                "SELECT first_name, last_name, email, active_member, voting_rights, receive_not_present_email, " \
                "include_in_quorum, sign_in_blocked FROM Member WHERE uuid = ?;");
        getInfoStatement->setString(1, uuid);
        sql::ResultSet *infoResult = getInfoStatement->executeQuery();
        delete getInfoStatement;

        sql::PreparedStatement* getGroupsStatement = database.connection->prepareStatement(
                "SELECT * FROM MemberGroup WHERE person_id = ?;");
        getGroupsStatement->setString(1, uuid);
        sql::ResultSet *groupResults = getGroupsStatement->executeQuery();
        delete getGroupsStatement;

        sql::PreparedStatement* getAttendanceStatement = database.connection->prepareStatement(
                "SELECT event_id, attendance_status FROM AttendanceRecord WHERE person_id = ?;");
        getAttendanceStatement->setString(1, uuid);
        sql::ResultSet *attendanceResults = getAttendanceStatement->executeQuery();
        delete getAttendanceStatement;

        infoResult->next();
        createPersonJson(infoResult, resultsJson, uuid);
        delete infoResult;

        // Add the groups list
        addListToJson(1, 2, groupResults, resultsJson, "groups");
        delete groupResults;

        // Adds the attendance record to the json
        while (attendanceResults->next()) {
            resultsJson[uuid]["attendance_record"][attendanceResults->getString(1)] = \
            attendanceResults->getString(2);
        }
        delete attendanceResults;
    }, response);
}

// Creates an entry in a JSON containing information on a person
void createPersonJson(sql::ResultSet* result, crow::json::wvalue& personData, const std::string& uuid) {
    personData[uuid]["first_name"] = result->getString(1);
    personData[uuid]["last_name"] = result->getString(2);
    personData[uuid]["email"] = result->getString(3);
    personData[uuid]["active_member"] = result->getBoolean(4);
    personData[uuid]["voting_rights"] = result->getBoolean(5);
    personData[uuid]["receive_not_present_email"] = result->getBoolean(6);
    personData[uuid]["include_in_quorum"] = result->getBoolean(7);
    personData[uuid]["sign_in_blocked"] = result->getBoolean(8);
}

// Sends a JSON to the requester with information on all the events
void getAllEvents(crow::response& response){
    tryToSendJson([](crow::json::wvalue& resultsJson) {
        // Gets the results from the database
        sql::ResultSet *eventResults = database.statement->executeQuery(
                "SELECT event_name, start_time, end_time, "
                "sign_in_closed, uuid FROM Event;");
        sql::ResultSet *groupResults = database.statement->executeQuery("SELECT * FROM GroupExpectedAtEvent;");

        // Create the json
        while (eventResults->next()) {
            createEventJson(eventResults, resultsJson, eventResults->getString(5));
        }
        delete eventResults;

        addListToJson(1, 2, groupResults, resultsJson, "groups");
        delete groupResults;
    }, response);
}

// Sends a JSON to the requester with information on a single event
void getSingleEvent(crow::response& response, const std::string& uuid) {
    tryToSendJson([&uuid](crow::json::wvalue& resultsJson) {
        // Gets the results from the database
        sql::PreparedStatement* getEventStatement = database.connection->prepareStatement(
                "SELECT event_name, start_time, end_time, sign_in_closed FROM Event WHERE uuid = ?;");
        getEventStatement->setString(1, uuid);
        sql::ResultSet *eventResults = getEventStatement->executeQuery();
        delete getEventStatement;

        sql::PreparedStatement* getGroupStatement = database.connection->prepareStatement(
                "SELECT membership_group FROM GroupExpectedAtEvent WHERE event_id = ?;");
        getGroupStatement->setString(1, uuid);
        sql::ResultSet* groupResults = getGroupStatement->executeQuery();
        delete getGroupStatement;

        eventResults->next();
        createEventJson(eventResults, resultsJson, uuid);
        delete eventResults;

        // Adds the list of groups to the json
        std::vector<std::string> groupList;
        while(groupResults->next()) {
            groupList.push_back(groupResults->getString(1));
        }
        delete groupResults;
    }, response);
}

// Creates an entry in a JSON containing information on an event
void createEventJson(sql::ResultSet* result, crow::json::wvalue& eventData, const std::string& uuid) {
    eventData[uuid]["event_name"] = result->getString(1);
    eventData[uuid]["start_time"] = result->getString(2);
    eventData[uuid]["end_time"] = result->getString(3);
    eventData[uuid]["sign_in_closed"] = result->getBoolean(4);
}

// Sends a JSON to the requester with information on all requests
void getAllRequests(crow::response& response) {
    tryToSendJson([](crow::json::wvalue& resultsJson) {
        // Gets the results from the database
        sql::ResultSet *result = database.statement->executeQuery("SELECT name, time_submitted, date_of_change, " \
            "type, change_status, reason, time_arriving, time_leaving, uuid FROM AttendanceChangeRequest;");

        while (result->next()) {
            createRequestJson(result, resultsJson, result->getString(9));
        }
        delete result;
    }, response);
}

// Sends a JSON to the requester with information on a single request
void getSingleRequest(crow::response& response, const std::string& uuid) {
    tryToSendJson([&uuid](crow::json::wvalue& resultsJson) {
        // Gets the results from the database
        sql::PreparedStatement *requestStatement = database.connection->prepareStatement(
                "SELECT name, time_submitted, date_of_change, type, change_status, reason, time_arriving, " \
                "time_leaving FROM AttendanceChangeRequest WHERE uuid = ?;");
        requestStatement->setString(1, uuid);
        sql::ResultSet* result = requestStatement->executeQuery();
        delete requestStatement;

        result->next();
        createRequestJson(result, resultsJson, uuid);
        delete result;
    }, response);
}

// Creates an entry in a JSON containing information on a request
void createRequestJson(sql::ResultSet* result, crow::json::wvalue& personData, const std::string& uuid) {
    personData[uuid]["name"] = result->getString(1);
    personData[uuid]["time_submitted"] = result->getString(2);
    personData[uuid]["date_of_change"] = result->getString(3);
    personData[uuid]["type"] = result->getString(4);
    personData[uuid]["change_status"] = result->getString(5);
    personData[uuid]["reason"] = result->getString(6);
    personData[uuid]["time_arriving"] = result->getString(7);
    personData[uuid]["time_leaving"] = result->getString(8);
}

/*
 * POST command processors
 */

// Receives a JSON representation of a person and adds it to the database as a new entry
void postCreateNewPerson(const crow::request& request, crow::response& response) {
    tryToWriteJson([](const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*> allCommands, std::string& uuid) {
        sql::PreparedStatement* updateCommand = database.connection->prepareStatement(
                "INSERT INTO Member VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);");
        updateCommand->setString(1, uuid);
        updateCommand->setString(11, uuid);

        updateCommand->setString(2, (std::string) jsonBody["first_name"].s());
        updateCommand->setString(3, (std::string) jsonBody["last_name"].s());
        updateCommand->setString(4, (std::string) jsonBody["email"].s());
        updateCommand->setBlob(5, &database.emptyEncodings);
        updateCommand->setBoolean(6, jsonBody["active_member"].b());
        updateCommand->setBoolean(7, jsonBody["voting_rights"].b());
        updateCommand->setBoolean(8, jsonBody["receive_not_present_email"].b());
        updateCommand->setBoolean(9, jsonBody["include_in_quorum"].b());
        updateCommand->setBoolean(10, jsonBody["sign_in_blocked"].b());

        allCommands.push_back(updateCommand);

        addGroupAndAttendanceUpdateCommands(jsonBody, allCommands, uuid);
    }, request, response);
}

// Receives a JSON representation of a person and updates an existing entry with the new information
void postUpdatePerson(const crow::request& request, crow::response& response) {
    tryToWriteJson([](const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid) {
        sql::PreparedStatement* updateCommand = database.connection->prepareStatement(
                "UPDATE Member SET first_name = ?, last_name = ?, email = ?, active_member = ?, " \
                            "voting_rights = ?, receive_not_present_email = ?, include_in_quorum = ?, " \
                            "sign_in_blocked = ? WHERE uuid = ?;");
        updateCommand->setString(1, (std::string) jsonBody["first_name"].s());
        updateCommand->setString(2, (std::string) jsonBody["last_name"].s());
        updateCommand->setString(3, (std::string) jsonBody["email"].s());
        updateCommand->setBoolean(4, jsonBody["active_member"].b());
        updateCommand->setBoolean(5, jsonBody["voting_rights"].b());
        updateCommand->setBoolean(6, jsonBody["receive_not_present_email"].b());
        updateCommand->setBoolean(7, jsonBody["include_in_quorum"].b());
        updateCommand->setBoolean(8, jsonBody["sign_in_blocked"].b());
        updateCommand->setString(9, uuid);
        allCommands.push_back(updateCommand);

        addGroupAndAttendanceUpdateCommands(jsonBody, allCommands, uuid);
    }, request, response);
}

// Takes a JSON representation of a person and adds commands to update the group and attendance record tables to a vector
void addGroupAndAttendanceUpdateCommands(const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid) {
    sql::PreparedStatement* deleteGroupsCommand = database.connection->prepareStatement("DELETE * FROM MemberGroup WHERE person_id = ?;");
    deleteGroupsCommand->setString(1, uuid);
    allCommands.push_back(deleteGroupsCommand);

    for (const crow::json::rvalue& groupToAdd : jsonBody["groups"]) {
        sql::PreparedStatement* addGroupsCommand = database.connection->prepareStatement("INSERT INTO MemberGroup VALUES ?, ?;");
        addGroupsCommand->setString(1, uuid);
        addGroupsCommand->setString(2, (std::string) groupToAdd.s());
        allCommands.push_back(addGroupsCommand);
    }

    sql::PreparedStatement* deleteAttendanceCommand = database.connection->prepareStatement("DELETE * FROM AttendanceRecord WHERE person_id = ?;");
    deleteAttendanceCommand->setString(1, uuid);
    allCommands.push_back(deleteAttendanceCommand);

    for (const std::string& eventId : jsonBody["attendance_record"].keys()) {
        sql::PreparedStatement* addAttendanceCommand = database.connection->prepareStatement("INSERT INTO AttendanceRecord VALUES ?, ?, ?;");
        addAttendanceCommand->setString(1, uuid);
        addAttendanceCommand->setString(2, eventId);
        addAttendanceCommand->setString(3, (std::string) jsonBody["attendance_record"][eventId].s());
        allCommands.push_back(addAttendanceCommand);
    }
}

// Receives a JSON representation of an event and adds it to the database as a new entry
void postCreateNewEvent(const crow::request& request, crow::response& response) {
    tryToWriteJson([](const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid) {
        sql::PreparedStatement* updateCommand = database.connection->prepareStatement(
                "INSERT INTO Event VALUES ?, ?, ?, ?, ?;");
        updateCommand->setString(1, uuid);
        updateCommand->setString(2, (std::string) jsonBody["event_name"].s());
        updateCommand->setString(3, (std::string) jsonBody["start_time"].s());
        updateCommand->setString(4, (std::string) jsonBody["end_time"].s());
        updateCommand->setBoolean(5, jsonBody["sign_in_closed"].b());

        allCommands.push_back(updateCommand);

        addEventGroupUpdateCommands(jsonBody, allCommands, uuid);
    }, request, response);
}

// Receives a JSON representation of an event and updates an existing entry with the new information
void postUpdateEvent(const crow::request& request, crow::response& response) {
    tryToWriteJson([](const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid) {
        sql::PreparedStatement* updateCommand = database.connection->prepareStatement(
                "UPDATE Event SET event_name = ?, start_time = ?, end_time = ?, sign_in_closed = ?, WHERE uuid = ?;");
        updateCommand->setString(1, (std::string) jsonBody["event_name"].s());
        updateCommand->setString(2, (std::string) jsonBody["start_time"].s());
        updateCommand->setString(3, (std::string) jsonBody["end_time"].s());
        updateCommand->setBoolean(4, jsonBody["sign_in_closed"].b());
        updateCommand->setString(5, uuid);

        addEventGroupUpdateCommands(jsonBody, allCommands, uuid);

    }, request, response);
}

// Takes a JSON representation of an event and adds commands to update the group table to a vector
void addEventGroupUpdateCommands(const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid) {
    sql::PreparedStatement* deleteCommand = database.connection->prepareStatement(
            "DELETE * FROM GroupExpectedAtEvent WHERE event_id = ?;");
    deleteCommand->setString(1, uuid);
    allCommands.push_back(deleteCommand);

    crow::json::rvalue groupList = jsonBody["groups"];
    for (crow::json::rvalue& groupName : groupList.lo()) {
        sql::PreparedStatement *addGroupCommand = database.connection->prepareStatement(
                "INSERT INTO GroupExpectedAtEvent VALUES ?, ?;");
        addGroupCommand->setString(1, uuid);
        addGroupCommand->setString(2, (std::string) groupName.s());
        allCommands.push_back(addGroupCommand);
    }
}

// Receives a JSON representation of a request and adds it to the database as a new entry
void postCreateNewRequest(const crow::request& request, crow::response& response) {
    tryToWriteJson([](const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid) {
        sql::PreparedStatement* updateCommand = database.connection->prepareStatement(
                "INSERT INTO AttendanceChangeRequest VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);");
        updateCommand->setString(1, uuid);
        updateCommand->setString(2, (std::string) jsonBody["name"].s());
        updateCommand->setString(3, (std::string) jsonBody["time_submitted"].s());
        updateCommand->setString(4, (std::string) jsonBody["date_of_change"].s());
        updateCommand->setString(5, (std::string) jsonBody["type"].s());
        updateCommand->setString(6, (std::string) jsonBody["change_status"].s());
        updateCommand->setString(7, (std::string) jsonBody["reason"].s());
        updateCommand->setString(8, (std::string) jsonBody["time_arriving"].s());
        updateCommand->setString(9, (std::string) jsonBody["time_leaving"].s());

        allCommands.push_back(updateCommand);

    }, request, response);
}

// Receives a JSON representation of a request and updates an existing entry with the new information
void postUpdateRequest(const crow::request& request, crow::response& response) {
    tryToWriteJson([](const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid) {
        sql::PreparedStatement* updateCommand = database.connection->prepareStatement(
                "UPDATE AttendanceChangeRequest SET name = ?, time_submitted = ?, date_of_change = ?, type = ?, change_status = ?, reason = ?, time_arriving = ?, time_leaving = ? WHERE uuid = ?;");
        updateCommand->setString(1, (std::string) jsonBody["name"].s());
        updateCommand->setString(2, (std::string) jsonBody["time_submitted"].s());
        updateCommand->setString(3, (std::string) jsonBody["date_of_change"].s());
        updateCommand->setString(4, (std::string) jsonBody["type"].s());
        updateCommand->setString(5, (std::string) jsonBody["change_status"].s());
        updateCommand->setString(6, (std::string) jsonBody["reason"].s());
        updateCommand->setString(7, (std::string) jsonBody["time_arriving"].s());
        updateCommand->setString(8, (std::string) jsonBody["time_leaving"].s());
        updateCommand->setString(9, uuid);

        allCommands.push_back(updateCommand);

    }, request, response);
}

/*
 * Helper functions
 */

// Attempts to send a JSON created by a provided function jsonCreator to a requester through a response
void tryToSendJson(const std::function<void(crow::json::wvalue&)>& jsonCreator, crow::response& response) {
    crow::json::wvalue resultsJson;
    if (database.ensureLiveConnection()) {
        try {
            jsonCreator(resultsJson);

            // Send the response to the user
            response.write(resultsJson.dump());
            response.code = 200;
            response.end();
        } catch (sql::SQLException& exception) {
            std::cout << "The database encountered an error! " << exception.what() << std::endl;
            sendDatabaseErrorResponse(response);
        }
    } else {
        std::cout << "The server was unable to connect to the database! Returning error 500." << std::endl;
        sendDatabaseErrorResponse(response);
    }
}

// Attempts to take JSON and write the contained data to the database
void tryToWriteJson(const std::function<void(const crow::json::rvalue&, std::vector<sql::PreparedStatement*>&, std::string&)>& statementPreparer,
                    const crow::request& request, crow::response& response) {
    if (database.ensureLiveConnection()) {
        try {
            // Extracts the UUID value
            crow::json::rvalue jsonBody = crow::json::load(request.body);
            std::vector<std::string> uuids = jsonBody.keys();
            if (uuids.size() != 1) {
                sendDatabaseErrorResponse(response);

                // If the extraction occurred successfully
            } else {
                std::string uuid = uuids.at(0);
                std::vector<sql::PreparedStatement*> allCommands;

                statementPreparer(jsonBody[uuid], allCommands, uuid);

                bool failed = false;
                for (sql::PreparedStatement* command : allCommands) {
                    if (command->executeUpdate() != 0) {
                        sendDatabaseErrorResponse(response);
                        failed = true;
                        break;
                    }
                }

                for (sql::PreparedStatement* command : allCommands) {
                    delete command;
                }

                if (!failed) {
                    // Send the response to the user
                    response.code = 200;
                    response.end();
                }
            }
        } catch (sql::SQLException& exception) {
            sendDatabaseErrorResponse(response);
        }
    } else {
        sendDatabaseErrorResponse(response);
    }
}

// Creates a list of strings from the contents of dataColumn for each uuid in the uuidColumn. The results are then added to the resultsJson under [uuid][targetKey]
void addListToJson(const int& uuidColumn, const int dataColumn, sql::ResultSet* results,
                   crow::json::wvalue& resultsJson, const std::string& targetKey) {
    // Adds the list of groups to the json
    std::unordered_map<std::string, std::vector<std::string>> valueList;
    while(results->next()) {
        std::string uuid = results->getString(uuidColumn);
        if (valueList.contains(uuid)) {
            valueList.find(uuid)->second.push_back(results->getString(dataColumn));
        } else {
            std::vector<std::string> allMatches;
            allMatches.push_back(results->getString(dataColumn));

            std::pair<std::string, std::vector<std::string>> mapElement(uuid, allMatches);
            valueList.insert(mapElement);
        }
    }

    for(std::string& uuid : resultsJson.keys()) {
        if (valueList.contains(uuid)) {
            resultsJson[uuid][targetKey] = valueList.find(uuid)->second;
        } else {
            resultsJson[uuid][targetKey] = std::vector<std::string>();
        }
    }
}

// Sends a database error 500 response to the requester
void sendDatabaseErrorResponse(crow::response& response) {
    response.clear();
    response.code = 500;
    response.end();
}

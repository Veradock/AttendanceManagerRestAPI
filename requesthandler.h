#pragma once
#include <crow.h>
#include <mysql/jdbc.h>
#include "database.h"

/*
 * GET command processors
 */
void getAllPeople(crow::response& response);
void getSinglePerson(crow::response& response, const std::string& uuid);

void getAllEvents(crow::response& response);
void getSingleEvent(crow::response& response, const std::string& uuid);

void getAllRequests(crow::response& response);
void getSingleRequest(crow::response& response, const std::string& uuid);

void createPersonJson(sql::ResultSet* result, crow::json::wvalue& personData, const std::string& uuid);
void createEventJson(sql::ResultSet* result, crow::json::wvalue& eventData, const std::string& uuid);
void createRequestJson(sql::ResultSet* result, crow::json::wvalue& personData, const std::string& uuid);

/*
 * POST command processors
 */
void postCreateNewPerson(const crow::request& request, crow::response& response);
void postUpdatePerson(const crow::request& request, crow::response& response);
void addGroupAndAttendanceUpdateCommands(const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid);

void postCreateNewEvent(const crow::request& request, crow::response& response);
void postUpdateEvent(const crow::request& request, crow::response& response);
void addEventGroupUpdateCommands(const crow::json::rvalue& jsonBody, std::vector<sql::PreparedStatement*>& allCommands, std::string& uuid);

void postCreateNewRequest(const crow::request& request, crow::response& response);
void postUpdateRequest(const crow::request& request, crow::response& response);

// Helpers
void addListToJson(const int& uuidColumn, int dataColumn, sql::ResultSet* results, crow::json::wvalue& resultsJson,
                   const std::string& targetKey);
void tryToSendJson(const std::function<void(crow::json::wvalue&)>& jsonCreator, crow::response& response);
void tryToWriteJson(const std::function<void(const crow::json::rvalue&, std::vector<sql::PreparedStatement*>&, std::string&)>& statementPreparer,
    const crow::request& request, crow::response& response);

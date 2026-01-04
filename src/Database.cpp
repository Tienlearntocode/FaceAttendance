
#include "Database.hpp" // <-- QUAN TRỌNG: Phải include file hpp
#include <iostream>
#include <cstring> // Dùng cho memcpy

DatabaseManager::DatabaseManager(const std::string& dbPath) {
    if (sqlite3_open(dbPath.c_str(), &db) != SQLITE_OK) {
        std::cout << "Lỗi: Không thể mở database!" << std::endl;
        db = nullptr;
    }
}

DatabaseManager::~DatabaseManager() {
    if (db) sqlite3_close(db);
}

bool DatabaseManager::initDB() {
    if (!db) return false;
    const char* sql = 
        "CREATE TABLE IF NOT EXISTS users ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "name TEXT NOT NULL, "
        "embedding BLOB);"
        
        "CREATE TABLE IF NOT EXISTS attendance ("
        "id INTEGER PRIMARY KEY AUTOINCREMENT, "
        "user_id INTEGER, "
        "timestamp DATETIME DEFAULT CURRENT_TIMESTAMP, "
        "FOREIGN KEY(user_id) REFERENCES users(id));";

    char* errMsg = nullptr;
    if (sqlite3_exec(db, sql, nullptr, nullptr, &errMsg) != SQLITE_OK) {
        std::cerr << "Lỗi SQL Init: " << errMsg << std::endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

bool DatabaseManager::addUser(const std::string& name, const std::vector<float>& embedding) {
    if (!db) return false;
    const char* sql = "INSERT INTO users (name, embedding) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 2, embedding.data(), embedding.size() * sizeof(float), SQLITE_STATIC);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

bool DatabaseManager::logAttendance(int userId) {
    if (!db) return false;
    const char* sql = "INSERT INTO attendance (user_id) VALUES (?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) return false;
    sqlite3_bind_int(stmt, 1, userId);

    bool success = (sqlite3_step(stmt) == SQLITE_DONE);
    sqlite3_finalize(stmt);
    return success;
}

std::vector<User> DatabaseManager::getAllUsers() {
    std::vector<User> users;
    if (!db) return users;

    const char* sql = "SELECT id, name, embedding FROM users;";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            User u;
            u.id = sqlite3_column_int(stmt, 0);
            u.name = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));

            const void* blob = sqlite3_column_blob(stmt, 2);
            int bytes = sqlite3_column_bytes(stmt, 2);
            
            int floatCount = bytes / sizeof(float);
            u.faceEmbedding.resize(floatCount);
            if (blob && floatCount > 0) {
                std::memcpy(u.faceEmbedding.data(), blob, bytes);
            }
            
            users.push_back(u);
        }
    }
    sqlite3_finalize(stmt);
    return users;
}
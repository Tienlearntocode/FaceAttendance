#pragma once 
#include <string>
#include <vector>
#include <sqlite3.h> // Thư viện SQLite

// 1. Cập nhật struct User (Thêm faceEmbedding)
struct User {
    int id;
    std::string name;
    std::vector<float> faceEmbedding; // <--- Cần thêm dòng này mới hết lỗi ở dòng 91, 93
};

class DatabaseManager {
public:
    DatabaseManager(const std::string& dbPath);
    ~DatabaseManager();

    bool initDB();

    // Hàm thêm User (Đã có)
    bool addUser(const std::string& name, const std::vector<float>& embedding);

    // Hàm lấy danh sách (Đã có)
    std::vector<User> getAllUsers();

    // --- QUAN TRỌNG: Thêm khai báo hàm logAttendance ---
    // (Nếu thiếu dòng này, file .cpp sẽ báo lỗi "no declaration matches")
    bool logAttendance(int userId); 

private:
    sqlite3* db; // Biến kết nối CSDL
};
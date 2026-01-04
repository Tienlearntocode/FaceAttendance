#include "GUI.hpp"
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QDebug>
#include <QMessageBox> // Để hiện thông báo đẹp

// --- 1. CONSTRUCTOR ---
AttendanceWindow::AttendanceWindow(QWidget *parent) : QMainWindow(parent) {
    setWindowTitle("Hệ thống Chấm công AI - Pro Version (Có Database)");
    resize(800, 600);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    videoLabel = new QLabel("Đang khởi động...", this);
    videoLabel->setAlignment(Qt::AlignCenter);
    videoLabel->setMinimumSize(640, 480);
    
    btnCheckIn = new QPushButton("👤 Đăng ký & Lưu vào Database", this);
    btnCheckIn->setStyleSheet("background-color: #007bff; color: white; padding: 12px; font-weight: bold;");
    
    statusLabel = new QLabel("Trạng thái: ...", this);

    layout->addWidget(videoLabel);
    layout->addWidget(btnCheckIn);
    layout->addWidget(statusLabel);

    // Khởi tạo Database và AI
    initDatabase();
    engine = new FaceEngine("", "");

    // TỰ ĐỘNG TẢI KHUÔN MẶT CŨ LÊN (Tính năng nhớ dai)
    loadFaceFromDB();

    if(cap.open(0)) {
        timer = new QTimer(this);
        connect(timer, &QTimer::timeout, this, &AttendanceWindow::updateFrame);
        timer->start(30); 
    } else {
        statusLabel->setText("Lỗi: Không thể bật Camera!");
    }

    connect(btnCheckIn, &QPushButton::clicked, this, &AttendanceWindow::manualCheckIn);
}

AttendanceWindow::~AttendanceWindow() {
    if (cap.isOpened()) cap.release();
    if (db) sqlite3_close(db);
    delete engine;
}

// --- 2. XỬ LÝ DATABASE (Lưu/Đọc Vector) ---

void AttendanceWindow::initDatabase() {
    // Mở file Database
    if (sqlite3_open("attendance_data.db", &db) != SQLITE_OK) return;

    // Tạo bảng Users (Lưu vector khuôn mặt dưới dạng BLOB - Binary Large Object)
    const char* sql = "CREATE TABLE IF NOT EXISTS users (id INTEGER PRIMARY KEY AUTOINCREMENT, name TEXT, embedding BLOB);";
    sqlite3_exec(db, sql, 0, 0, 0);
}

// Hàm lưu khuôn mặt vào ổ cứng
void AttendanceWindow::saveFaceToDB(const std::vector<float>& feature) {
    if (!db || feature.empty()) return;

    const char* sql = "INSERT INTO users (name, embedding) VALUES (?, ?);";
    sqlite3_stmt* stmt;

    // Chuẩn bị câu lệnh (Prepare Statement)
    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        // Gán tên
        sqlite3_bind_text(stmt, 1, "Admin", -1, SQLITE_STATIC);
        
        // Gán vector (Quan trọng: Ép kiểu vector float sang mảng byte)
        sqlite3_bind_blob(stmt, 2, feature.data(), feature.size() * sizeof(float), SQLITE_STATIC);

        // Thực thi
        if (sqlite3_step(stmt) == SQLITE_DONE) {
            statusLabel->setText("✅ Đã lưu khuôn mặt vào Database thành công!");
        } else {
            statusLabel->setText("❌ Lỗi khi lưu vào Database!");
        }
        sqlite3_finalize(stmt);
    }
}

// Hàm đọc khuôn mặt cũ lên RAM
void AttendanceWindow::loadFaceFromDB() {
    if (!db) return;

    const char* sql = "SELECT embedding FROM users ORDER BY id DESC LIMIT 1;"; // Lấy người mới nhất
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, 0) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            // Lấy dữ liệu BLOB ra
            const void* blob = sqlite3_column_blob(stmt, 0);
            int bytes = sqlite3_column_bytes(stmt, 0);

            // Copy ngược lại vào vector
            registeredFace.resize(bytes / sizeof(float));
            memcpy(registeredFace.data(), blob, bytes);

            statusLabel->setText("📂 Đã tải dữ liệu khuôn mặt cũ từ Database!");
        }
        sqlite3_finalize(stmt);
    }
}

// --- 3. CÁC HÀM LOGIC CHÍNH ---

void AttendanceWindow::updateFrame() {
    if (cap.read(currentFrame)) {
        cv::Mat faces = engine->detectFace(currentFrame);

        if (!faces.empty()) {
            int x = static_cast<int>(faces.at<float>(0, 0));
            int y = static_cast<int>(faces.at<float>(0, 1));
            int w = static_cast<int>(faces.at<float>(0, 2));
            int h = static_cast<int>(faces.at<float>(0, 3));
            cv::rectangle(currentFrame, cv::Rect(x, y, w, h), cv::Scalar(0, 255, 0), 2);

            // So sánh nếu đã có dữ liệu trong RAM
            if (!registeredFace.empty()) {
                std::vector<float> currentFeature = engine->extractFeatures(currentFrame, faces);
                float score = engine->compareFaces(currentFeature, registeredFace);

                std::string text = (score > 0.363) ? "CHU NHAN" : "Unknown";
                cv::Scalar color = (score > 0.363) ? cv::Scalar(0, 255, 0) : cv::Scalar(0, 0, 255);
                
                cv::putText(currentFrame, text + " (" + std::to_string(score).substr(0,4) + ")", 
                            cv::Point(x, y - 10), cv::FONT_HERSHEY_SIMPLEX, 0.8, color, 2);
            }
        }

        cv::cvtColor(currentFrame, currentFrame, cv::COLOR_BGR2RGB);
        QImage qtImage((const unsigned char*)currentFrame.data, currentFrame.cols, currentFrame.rows, currentFrame.step, QImage::Format_RGB888);
        videoLabel->setPixmap(QPixmap::fromImage(qtImage));
        cv::cvtColor(currentFrame, currentFrame, cv::COLOR_RGB2BGR); 
    }
}

void AttendanceWindow::manualCheckIn() {
    cv::Mat faces = engine->detectFace(currentFrame);
    if (faces.empty()) {
        QMessageBox::warning(this, "Lỗi", "Không tìm thấy khuôn mặt nào!");
        return;
    }

    // 1. Lấy đặc trưng
    std::vector<float> newFeature = engine->extractFeatures(currentFrame, faces);
    
    // 2. Cập nhật vào RAM ngay
    registeredFace = newFeature;

    // 3. Lưu xuống ổ cứng (Database)
    saveFaceToDB(newFeature);

    btnCheckIn->setText("✅ Đã lưu! (Reset app để test)");
}
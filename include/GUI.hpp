#pragma once

#include <QMainWindow>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <vector>

#include <opencv2/opencv.hpp>
#include <sqlite3.h>
#include "FaceEngine.hpp" 

class AttendanceWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit AttendanceWindow(QWidget *parent = nullptr);
    ~AttendanceWindow();

private slots:
    void updateFrame();      
    void manualCheckIn(); // Vừa đăng ký, vừa lưu vào DB

private:
    // Giao diện
    QLabel *videoLabel;
    QLabel *statusLabel;
    QPushButton *btnCheckIn;

    // Logic
    QTimer *timer;
    cv::VideoCapture cap;
    cv::Mat currentFrame;
    sqlite3 *db;

    // AI
    FaceEngine *engine;
    std::vector<float> registeredFace; // Lưu vector đặc trưng (RAM)

    // Database Functions
    void initDatabase();
    void saveFaceToDB(const std::vector<float>& feature); // Lưu vào ổ cứng
    void loadFaceFromDB(); // Tải từ ổ cứng lên
};
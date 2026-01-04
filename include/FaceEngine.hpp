#pragma once
#include <opencv2/opencv.hpp>
#include <vector>
#include <string>

class FaceEngine {
public:
    // Constructor: Nạp 2 model AI
    FaceEngine(const std::string& modelPath, const std::string& configPath);
    ~FaceEngine();

    // Hàm 1: Phát hiện khuôn mặt
    cv::Mat detectFace(const cv::Mat& frame);

    // Hàm 2: Trích xuất đặc trưng (Vector hóa)
    std::vector<float> extractFeatures(const cv::Mat& frame, const cv::Mat& faceROI);

    // Hàm 3: So sánh 2 khuôn mặt
    float compareFaces(const std::vector<float>& vec1, const std::vector<float>& vec2);

private:
    // 2 biến quan trọng chứa "bộ não" AI
    cv::Ptr<cv::FaceDetectorYN> detector;
    cv::Ptr<cv::FaceRecognizerSF> recognizer;
};
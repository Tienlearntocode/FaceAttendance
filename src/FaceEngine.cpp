#include "FaceEngine.hpp"
#include <iostream>
#include <vector>

// Constructor: Nạp model AI
FaceEngine::FaceEngine(const std::string& modelPath, const std::string& configPath) {
    // ĐƯỜNG DẪN CỨNG ĐẾN 2 FILE MODEL (Bạn nhớ kiểm tra lại xem file có đúng nằm ở đây không)
    std::string detPath = "models/face_detection_yunet_2023mar.onnx";
    std::string recPath = "models/face_recognition_sface_2021dec.onnx";

    try {
        // 1. Khởi tạo model phát hiện khuôn mặt (YuNet)
        // input size (320, 320) là kích thước chuẩn để detect
        detector = cv::FaceDetectorYN::create(detPath, "", cv::Size(320, 320), 0.9f, 0.3f, 5000);
        
        // 2. Khởi tạo model nhận diện đặc trưng (SFace)
        recognizer = cv::FaceRecognizerSF::create(recPath, "");
        
        std::cout << "AI Models loaded successfully!" << std::endl;
    } catch (const cv::Exception& e) {
        std::cerr << "Loi load AI Model: " << e.what() << std::endl;
    }
}

FaceEngine::~FaceEngine() {}

// Hàm phát hiện khuôn mặt
cv::Mat FaceEngine::detectFace(const cv::Mat& frame) {
    if (frame.empty()) return cv::Mat();

    // Cập nhật kích thước ảnh đầu vào cho model detector
    detector->setInputSize(frame.size());

    cv::Mat faces;
    detector->detect(frame, faces); // Tìm mặt

    // Nếu tìm thấy ít nhất 1 mặt (rows >= 1) thì trả về kết quả
    if (faces.rows >= 1) {
        return faces; 
    }

    return cv::Mat(); // Không thấy thì trả về rỗng
}

// Hàm trích xuất đặc trưng (Vector hóa khuôn mặt)
std::vector<float> FaceEngine::extractFeatures(const cv::Mat& frame, const cv::Mat& faceROI) {
    cv::Mat aligned_face;
    std::vector<float> feature;

    if (faceROI.empty()) return feature;

    // Căn chỉnh khuôn mặt cho thẳng (Align) và cắt ra
    recognizer->alignCrop(frame, faceROI.row(0), aligned_face);

    // Trích xuất đặc trưng (Ra 1 vector 128 số)
    cv::Mat featureMat;
    recognizer->feature(aligned_face, featureMat);
    
    // Copy dữ liệu sang vector chuẩn C++
    feature = (std::vector<float>)featureMat.clone();
    return feature;
}

// Hàm so sánh 2 khuôn mặt (Cosine Similarity)
float FaceEngine::compareFaces(const std::vector<float>& vec1, const std::vector<float>& vec2) {
    if (vec1.empty() || vec2.empty()) return 0.0f;
    
    // Chuyển vector về dạng Mat để OpenCV tính toán
    cv::Mat f1(vec1), f2(vec2);
    
    // Tính điểm giống nhau (Càng cao càng giống, trên 0.363 là cùng 1 người)
    double score = recognizer->match(f1, f2, cv::FaceRecognizerSF::DisType::FR_COSINE);
    
    return (float)score; 
}
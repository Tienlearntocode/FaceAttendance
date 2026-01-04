# 📸 High-Performance Face Attendance System (C++)

> Hệ thống điểm danh nhận diện khuôn mặt thời gian thực sử dụng C++, OpenCV và SQLite theo mô hình Edge AI.

![C++](https://img.shields.io/badge/Language-C++17-blue.svg)
![OpenCV](https://img.shields.io/badge/Library-OpenCV-green.svg)
![Status](https://img.shields.io/badge/Status-In%20Development-yellow.svg)

## 📖 Giới thiệu
Dự án này là bài tập lớn môn **Phần mềm Mã nguồn mở**, nhằm xây dựng giải pháp điểm danh hoạt động độc lập (Standalone) trên máy tính cá nhân, tối ưu hóa hiệu năng để chạy trên các thiết bị cấu hình thấp.

**Tính năng chính:**
* Điểm danh bằng khuôn mặt (Real-time).
* Lưu trữ dữ liệu cục bộ bằng SQLite.
* Xuất báo cáo chấm công ra file CSV.

## 🛠️ Yêu cầu hệ thống (Prerequisites)

Trước khi bắt đầu, đảm bảo máy tính của bạn đã cài đặt các công cụ sau:

### 1. Công cụ biên dịch
* **Windows:** Visual Studio 2019/2022 (Chọn workload "Desktop development with C++").
* **Linux:** GCC/G++ (`sudo apt install build-essential`).

### 2. CMake (Bắt buộc)
* Hệ thống sử dụng CMake để tạo build file.
* Tải tại: [cmake.org](https://cmake.org/download/)
* *Lưu ý: Chọn "Add CMake to system PATH" khi cài đặt.*

### 3. Thư viện OpenCV
* **Windows:**
    1. Tải bản pre-built mới nhất tại [opencv.org/releases](https://opencv.org/releases/).
    2. Giải nén vào ổ đĩa (ví dụ: `C:\opencv`).
    3. Thêm đường dẫn `C:\opencv\build\x64\vc15\bin` vào biến môi trường **Path** của Windows.
* **Linux:**
    ```bash
    sudo apt update
    sudo apt install libopencv-dev
    ```

### 4. SQLite3
* **Windows:** Tải file DLL và copy vào thư mục `C:\Windows\System32` hoặc để cạnh file `.exe` khi chạy.
* **Linux:** `sudo apt install libsqlite3-dev`

---

## 🚀 Hướng dẫn Cài đặt & Build

Làm theo các bước sau để chạy dự án trên máy của bạn:

### Bước 1: Clone dự án
```bash
git clone [https://github.com/USERNAME/FaceAttendance_CPP.git](https://github.com/USERNAME/FaceAttendance_CPP.git)
cd FaceAttendance_CPP
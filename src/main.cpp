#include <QApplication>
#include "GUI.hpp" 

// BẮT BUỘC: Phải có argc, argv để Qt hoạt động
int main(int argc, char *argv[]) {
    // 1. Khởi động vòng đời ứng dụng Qt
    QApplication app(argc, argv);

    // 2. Tạo cửa sổ chính (Mọi logic Camera, DB, Engine sẽ chạy trong Constructor của class này)
    AttendanceWindow window;
    
    // 3. Hiển thị cửa sổ lên màn hình
    window.show();

    // 4. Vào vòng lặp sự kiện (Event Loop) và chờ người dùng bấm nút
    return app.exec();
}
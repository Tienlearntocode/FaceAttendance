cmake_minimum_required(VERSION 3.16) # Qt6 yêu cầu CMake mới hơn
project(FaceAttendance_App)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

# --- PHẦN QUAN TRỌNG NHẤT: TRỎ ĐƯỜNG DẪN QT ---
# Thay đường dẫn dưới đây bằng đường dẫn thực tế máy bạn (bỏ đoạn /bin đi)
set(CMAKE_PREFIX_PATH "D:/Qt/6.10.1/mingw_64") 

# Tìm gói Qt6 (Thay vì Qt5)
find_package(Qt6 REQUIRED COMPONENTS Widgets Core Gui)
find_package(OpenCV REQUIRED)
find_package(SQLite3 REQUIRED)
# --- QUAN TRỌNG: Báo cho CMake biết file .hpp nằm ở đâu ---
include_directories(include)
# Liệt kê các file code
add_executable(FaceApp 
    src/main.cpp 
    src/GUI.cpp 
    src/FaceEngine.cpp 
    src/Database.cpp

    include/GUI.hpp
    include/Database.hpp
    include/FaceEngine.hpp
)

# Link thư viện (Dùng Qt6::Widgets thay vì Qt5::Widgets)
target_link_libraries(FaceApp
    PRIVATE Qt6::Widgets
    PRIVATE ${OpenCV_LIBS}
    PRIVATE SQLite::SQLite3
)
project (coin_detector)
cmake_minimum_required(VERSION 3.15)

# configure OpenCV
find_package(OpenCV REQUIRED)

# create create individual projects
add_executable(coin_detector coin_detector.cpp)
target_link_libraries(coin_detector ${OpenCV_LIBS})



project(tx)

set(CMAKE_CXX_FLAGS "-O3")
add_executable(test test.cpp)

add_executable(test_IMU test_IMU.cpp)
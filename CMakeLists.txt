project(tx)

include_directories(include)

set(CMAKE_CXX_FLAGS "-O3")
add_executable(test src/test.cpp)

add_executable(test_IMU src/test_IMU.cpp)
#examples
cmake_minimum_required (VERSION 2.6)
project (kinovaZED)


include_directories ("${PROJECT_SOURCE_DIR}/kinovaArm" "${PROJECT_SOURCE_DIR}/TCPServer")
add_subdirectory (TCPServer)
add_subdirectory (kinovaArm)


add_executable(KinovaZED KinovaMain.cpp)

target_link_libraries (KinovaZED KinovaArm TCPServer)


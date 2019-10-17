cmake_minimum_required (VERSION 3.5.1)

project (kinovaZED)
find_package(Boost REQUIRED COMPONENTS system)

add_subdirectory(thirdparty/libkindrv)
add_subdirectory(thirdparty/spdlog)

add_library(kinovaArm_lib STATIC 
  src/KinovaArm.cpp
  src/TCPServer.cpp
  src/PositionHandling.cpp
  src/StateMachine.cpp
  src/State.cpp
  src/Sequence.cpp
)

set_property(TARGET kinovaArm_lib PROPERTY CXX_STANDARD 14)

target_include_directories(kinovaArm_lib
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(kinovaArm_lib
  PUBLIC
  libkindrv
  spdlog::spdlog_header_only
  ${Boost_LIBRARIES}
)

#Main Program
add_executable(KinovaZED src/KinovaMain.cpp)
target_link_libraries(KinovaZED
  PUBLIC
  kinovaArm_lib
)

set_property(TARGET KinovaZED PROPERTY CXX_STANDARD 14)

add_subdirectory(test)

#Copy autorun script and data points
configure_file(${PROJECT_SOURCE_DIR}/KinovaStartUp.sh ${PROJECT_BINARY_DIR} COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/CybathlonObjectives.dat ${PROJECT_BINARY_DIR} COPYONLY)


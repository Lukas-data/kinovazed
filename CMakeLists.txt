cmake_minimum_required (VERSION 3.5.1)

project (kinovaZED)
find_package(Boost REQUIRED COMPONENTS system)

add_subdirectory(thirdparty/libkindrv)

add_library(kinovaArm_lib STATIC 
src/KinovaArm.cpp
src/TCPServer.cpp
src/PositionHandling.cpp
src/StateMachine.cpp
src/State.cpp
src/CommandHandling.cpp
)

set_property(TARGET kinovaArm_lib PROPERTY CXX_STANDARD 17)

target_include_directories(kinovaArm_lib
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(kinovaArm_lib
  PUBLIC
  libkindrv
  ${Boost_LIBRARIES}
)

#Main Program
add_executable(KinovaZED src/KinovaMain.cpp)
target_link_libraries(KinovaZED
  PUBLIC
  kinovaArm_lib
  rt
)


add_subdirectory(test)



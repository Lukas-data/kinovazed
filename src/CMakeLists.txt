cmake_minimum_required (VERSION 3.5.1)

project (kinovaZED_prog)

#Shared Sources for kinovaZED_prog and kinovaZED_test
add_library(kinovaArm_lib STATIC  KinovaArm.cpp KinovaArm.h
                                  TCPServer.cpp TCPServer.h
                                  PositionHandling.cpp PositionHandling.h
                                  StateMachine.cpp StateMachine.h
                                  Transition.h
                                  State.cpp State.h
                                  CommandHandling.cpp CommandHandling.h)

target_include_directories(kinovaArm_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#Main Program
add_executable(KinovaZED KinovaMain.cpp)
target_link_libraries(KinovaZED PUBLIC libkindrv PRIVATE kinovaArm_lib)

#install (TARGETS KinovaZED DESTINATION bin)
#install (FILES KinovaArm.h  DESTINATION include)


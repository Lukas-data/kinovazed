cmake_minimum_required (VERSION 3.5.1)

project (kinovaZED_test)


#Test Program
add_executable(KinovaTest KinovaTest.cpp)
target_link_libraries(KinovaTest PUBLIC libkindrv PRIVATE kinovaArm_lib)



cmake_minimum_required (VERSION 3.5.1)

project (kinovaZED_test)


#Test Program
add_executable(KinovaTest 
  KinovaTest.cpp 
  IntegrationSuite.cpp
  MatrixSuite.cpp
  KinovaArmSuite.cpp
  PositionHandlingSuite.cpp
  SequenceSuite.cpp
)
target_include_directories(KinovaTest 
  PRIVATE
  ${PROJECT_SOURCE_DIR}/cute
)
target_link_libraries(KinovaTest 
  PUBLIC
  libkindrv
  PRIVATE
  kinovaArm_lib
)
set_property(TARGET KinovaTest PROPERTY CXX_STANDARD 14)

#Log Test
#add_library(logtest_lib
#  STATIC
#  LogTestSubClass.cpp
#)
#add_executable(LogTest
#  LogTest.cpp
#)
#target_link_libraries(LogTest
#  PUBLIC
#  kinovaArm_lib
#  PRIVATE
#  logtest_lib
#)

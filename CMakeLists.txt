cmake_minimum_required (VERSION 3.9.0)

project (kinovaZED
  LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD "14")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#Create logs directory
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")

add_subdirectory(thirdparty/libkindrv)
add_subdirectory(thirdparty/spdlog)

set(JSON_BuildTests OFF CACHE INTERNAL "")
add_subdirectory(thirdparty/json)

find_package(Boost REQUIRED COMPONENTS system)

# Kinova Arm Core
add_library(kinovaArm_core STATIC
  "src/KinovaArm.cpp"
  "src/Coordinates.cpp"
  "src/Objective.cpp"
  "src/PositionHandling.cpp"
  "src/Sequence.cpp"
  "src/State.cpp"
  "src/StateMachine.cpp"
  "src/TCPServer.cpp"

  "include/CommandHandling.h"
  "include/Coordinates.h"
  "include/Event.h"
  "include/Exceptions.h"
  "include/KinovaStatus.h"
  "include/Matrix.h"
  "include/Objective.h"
  "include/Prefix.h"
  "include/RoboRioProtocol.h"
  "include/Transition.h"
)

target_compile_options(
  kinovaArm_core
  PUBLIC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall
    -Wextra
    -Werror
#    -pedantic-errors
  >
)

target_include_directories(kinovaArm_core
  PUBLIC
  ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(kinovaArm_core
  PUBLIC
  "libkindrv"
  "spdlog::spdlog_header_only"
  "nlohmann_json::nlohmann_json"
  "rt"
  ${Boost_LIBRARIES}
)

get_target_property(kinovaArm_core_SOURCES "kinovaArm_core" SOURCES)
foreach(SOURCE_FILE IN LISTS kinovaArm_core_SOURCES)
  if(SOURCE_FILE MATCHES "src/.*\.cpp")
    string(REGEX REPLACE
      "src/(.*)\.cpp"
      "include/\\1.h"
      HEADER_FILE
      "${SOURCE_FILE}"
    )
    get_filename_component(HEADER_FILE_PATH "${HEADER_FILE}" REALPATH)
    if(EXISTS "${HEADER_FILE_PATH}")
      target_sources("kinovaArm_core"
        INTERFACE
        "${HEADER_FILE_PATH}"
      )
    endif()
  endif()
endforeach()

#Main Program
add_executable(KinovaZED
  "src/KinovaMain.cpp"
)
target_link_libraries(KinovaZED
  PUBLIC
  "kinovaArm_core"
)
target_compile_options(
  KinovaZED
  PUBLIC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    -Wall
    -Wextra
    -Werror
#    -pedantic-errors
  >
)

#Unit and Integration Tests
add_executable(KinovaTest
  "test/KinovaTest.cpp"
  "test/IntegrationSuite.cpp"
  "test/MatrixSuite.cpp"
  "test/KinovaArmSuite.cpp"
  "test/PositionHandlingSuite.cpp"
  "test/SequenceSuite.cpp"
)

target_include_directories("KinovaTest"
  PRIVATE
  "test/cute"
)

target_link_libraries("KinovaTest"
  PRIVATE
  "kinovaArm_core"
)

#Copy autorun script and data points
configure_file(${PROJECT_SOURCE_DIR}/KinovaStartUp.sh ${PROJECT_BINARY_DIR} COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/CybathlonObjectives.dat ${PROJECT_BINARY_DIR} COPYONLY)
configure_file(${PROJECT_SOURCE_DIR}/CybathlonObjectives.json ${PROJECT_BINARY_DIR} COPYONLY)

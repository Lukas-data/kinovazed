cmake_minimum_required(VERSION "3.9.0")
cmake_policy(SET "CMP0048" NEW)

project("kinovaZED"
  LANGUAGES CXX
  VERSION "2.0.0"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

set(CMAKE_CXX_STANDARD "14")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)

### System Dependencies Setup

find_package("Boost"
  REQUIRED
  COMPONENTS "system"
)

### Conan Dependencies Setup

include("Conan")
conan_check(REQUIRED)
conan_cmake_run(CONANFILE "conanfile.txt"
  BASIC_SETUP
  CMAKE_TARGETS
  BUILD "missing"
  NO_OUTPUT_DIRS
)

### Local Dependencies Setup

add_subdirectory("thirdparty/libkindrv")

#Create logs directory

file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/logs")


### Core Library

add_library("${PROJECT_NAME}Core"
  "src/KinovaArm.cpp"
  "src/Coordinates.cpp"
  "src/Objective.cpp"
  "src/PositionHandling.cpp"
  "src/Sequence.cpp"
  "src/State.cpp"
  "src/StateMachine.cpp"
  "src/TCPServer.cpp"
)

target_compile_options("${PROJECT_NAME}Core" PUBLIC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pedantic-errors"
  >
)

target_include_directories("${PROJECT_NAME}Core" PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)

target_link_libraries("${PROJECT_NAME}Core"
  PUBLIC
  "Boost::system"
  "CONAN_PKG::nlohmann_json"
  "CONAN_PKG::spdlog"
  "kindrv"
  "rt"
)

### Main Application

add_executable("${PROJECT_NAME}"
  "src/KinovaMain.cpp"
)

target_link_libraries("${PROJECT_NAME}"
  PUBLIC
  "${PROJECT_NAME}Core"
)

target_compile_options("${PROJECT_NAME}" PUBLIC
  $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:GNU>>:
    "-Wall"
    "-Wextra"
    "-Werror"
    "-pedantic-errors"
  >
)

# #Unit and Integration Tests

# add_executable(KinovaTest
#   "test/KinovaTest.cpp"
#   "test/IntegrationSuite.cpp"
#   "test/MatrixSuite.cpp"
#   "test/KinovaArmSuite.cpp"
#   "test/PositionHandlingSuite.cpp"
#   "test/SequenceSuite.cpp"
# )

# target_include_directories("KinovaTest"
#   PRIVATE
#   "test/cute"
# )

# target_link_libraries("KinovaTest"
#   PRIVATE
#   "kinovaArm_core"
# )

# #Copy autorun script and data points
# configure_file(${PROJECT_SOURCE_DIR}/KinovaStartUp.sh ${PROJECT_BINARY_DIR} COPYONLY)
# configure_file(${PROJECT_SOURCE_DIR}/CybathlonObjectives.dat ${PROJECT_BINARY_DIR} COPYONLY)
# configure_file(${PROJECT_SOURCE_DIR}/CybathlonObjectives.json ${PROJECT_BINARY_DIR} COPYONLY)

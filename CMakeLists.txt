cmake_minimum_required(VERSION "3.9.0")
cmake_policy(SET "CMP0048" NEW)

project("kinovaZED"
  LANGUAGES CXX
  VERSION "2.0.0"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake/Modules")

set(CMAKE_CXX_STANDARD "17")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION OFF)

include("CTest")
include("GNUInstallDirs")

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
  set(CMAKE_INSTALL_PREFIX "$ENV{HOME}/.local")
endif()

if(NOT DEFAULT_LOG_DIRECTORY)
  set(DEFAULT_LOG_DIRECTORY "$ENV{HOME}/.var/log")
endif()

set(GENERATED_FILE_REMARK "THIS FILE IS GENERATED AUTOMATICALLY! DON'T EDIT IT! DON'T COMMIT IT!")

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

### Core Library

add_library("${PROJECT_NAME}Core"
  # Communication
  "src/comm/Command.cpp"
  "src/comm/CommandInterface.cpp"
  "src/comm/TCPInterface.cpp"

  # Control
  "src/control/Objective.cpp"
  "src/control/ObjectiveId.cpp"
  "src/control/PositionHandling.cpp"
  "src/control/Sequence.cpp"
  # "src/control/State.cpp"
  "src/control/StateMachine.cpp"

  # Hardware
  "src/hw/Actor.cpp"
  "src/hw/Coordinates.cpp"
  "src/hw/KinovaArm.cpp"
  "src/hw/KinovaArmPrivate.cpp"
  "src/hw/Origin.cpp"

  # Support
  "src/support/Logging.cpp"
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
  "CONAN_PKG::asio"
  "CONAN_PKG::nlohmann_json"
  "CONAN_PKG::spdlog"
  "kindrv"
  "rt"
)

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_LESS "9.0")
  target_link_libraries("${PROJECT_NAME}Core" PUBLIC
    "stdc++fs"
  )
endif()

configure_file("${PROJECT_SOURCE_DIR}/include/support/Paths.h.in"
  "${PROJECT_SOURCE_DIR}/include/support/Paths.h"
  @ONLY
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

install(TARGETS
  "${PROJECT_NAME}Core"
  "${PROJECT_NAME}"
  LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
  RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
)

### Support Data and Scripts

configure_file("${PROJECT_SOURCE_DIR}/support/scripts/KinovaStartUp"
  "${PROJECT_BINARY_DIR}/KinovaStartUp"
  @ONLY
)

install(FILES
  "${PROJECT_BINARY_DIR}/KinovaStartUp"
  DESTINATION "${CMAKE_INSTALL_BINDIR}"
  PERMISSIONS
  GROUP_EXECUTE
  GROUP_READ
  OWNER_EXECUTE
  OWNER_READ
  OWNER_WRITE
  WORLD_EXECUTE
  WORLD_READ
)

install(FILES
  "${PROJECT_SOURCE_DIR}/support/objectives/CybathlonObjectives.dat"
  "${PROJECT_SOURCE_DIR}/support/objectives/CybathlonObjectives.json"
  DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}"
)

### Tests

if(NOT BUILD_TESTING)
  include("DiscoverTests")

  add_executable("${PROJECT_NAME}Test"
    "test/src/IntegrationSuite.cpp"
    "test/src/KinovaArmSuite.cpp"
    "test/src/KinovaTest.cpp"
    "test/src/MatrixSuite.cpp"
    "test/src/PositionHandlingSuite.cpp"
    "test/src/SequenceSuite.cpp"
  )

  target_include_directories("${PROJECT_NAME}Test" SYSTEM PUBLIC
    "test/thirdparty"
  )

  target_include_directories("${PROJECT_NAME}Test" PRIVATE
    "test/include"
  )

  target_link_libraries("${PROJECT_NAME}Test" PUBLIC
    "${PROJECT_NAME}Core"
    "CONAN_PKG::lyra"
  )

  discover_tests(TARGET "${PROJECT_NAME}Test")

  add_custom_target("run_all_tests"
    COMMAND ${CMAKE_CTEST_COMMAND} "--output-on-failure"
    WORKING_DIRECTORY ${PROJECT_BINARY_DIR}
    DEPENDS "${PROJECT_NAME}Test"
    COMMENT "Running unit tests..."
  )
endif()

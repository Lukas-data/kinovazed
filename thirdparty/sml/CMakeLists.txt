cmake_minimum_required(VERSION "3.9.0")

project("sml"
  LANGUAGES CXX
)

add_library("${PROJECT_NAME}" INTERFACE)
target_include_directories("${PROJECT_NAME}" SYSTEM
  INTERFACE
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
)
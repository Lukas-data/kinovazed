cmake_minimum_required (VERSION 3.5.1)

project(Jansson)


# find installed kindrv library and include dir

find_library (JANSON_LIB libjanson)

if (NOT JANSON_LIB)
  message(FATAL_ERROR "Could not find libjanson!")
endif (NOT JANSON_LIB)


add_library(ljanson INTERFACE)
target_link_libraries(ljanson INTERFACE ${LIBKINDRV_LIB})
#target_include_directories(ljanson INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)



#install (TARGETS libkindrv DESTINATION bin)
#install (FILES include/kindrv.h DESTINATION /usr/local/include)


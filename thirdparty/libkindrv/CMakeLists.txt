cmake_minimum_required (VERSION 3.5.1)

project(LibKinDrv)


# find installed kindrv library and include dir

find_library (LIBKINDRV_LIB libkindrv
	PATHS /usr/local/lib/libkindrv )

if (NOT LIBKINDRV_LIB)
  message(FATAL_ERROR "Could not find libkindrv!")
endif (NOT LIBKINDRV_LIB)


# find Boost
find_package(Boost COMPONENTS system)

add_library(libkindrv INTERFACE)
target_link_libraries(libkindrv INTERFACE ${LIBKINDRV_LIB} ${Boost_LIBRARIES})
target_include_directories(libkindrv INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)



#install (TARGETS libkindrv DESTINATION bin)
#install (FILES include/kindrv.h DESTINATION /usr/local/include)


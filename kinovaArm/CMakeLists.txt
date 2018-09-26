cmake_minimum_required(VERSION 2.6)

# LIBKINDRV entries should only be shown in advanced view
mark_as_advanced(LIBKINDRV_LIB LIBKINDRV_INCLUDE_DIR)

# find installed kindrv library and include dir
set (LIBKINDRV_FOUND FALSE CACHE STRING "FALSE" FORCE)
find_library (LIBKINDRV_LIB libkindrv
	PATHS /usr/local/lib/libkindrv
)

find_path (LIBKINDRV_INCLUDE_DIR kindrv.h
  PATHS
    /usr/include
    /usr/local/include
    /opt/local/include
  PATH_SUFFIXES
    libkindrv
)

if (LIBKINDRV_LIB AND LIBKINDRV_INCLUDE_DIR)
  set (LIBKINDRV_FOUND TRUE CACHE STRING "TRUE" FORCE)
endif (LIBKINDRV_LIB AND LIBKINDRV_INCLUDE_DIR)

if (LIBKINDRV_FOUND)
  include_directories (${LIBKINDRV_INCLUDE_DIR} ${KINFUNC_INCLUDE_DIR} )
else (LIBKINDRV_FOUND)
  message(FATAL_ERROR "Could not find libkindrv!")
endif (LIBKINDRV_FOUND)

# find Boost
find_package(Boost COMPONENTS system)





# add KinovaArm library
add_library(KinovaArm KinovaArm.cpp)

include_directories (../positionHandling)
add_subdirectory (${CMAKE_CURRENT_SOURCE_DIR}/../positionHandling ${CMAKE_CURRENT_BINARY_DIR}/positionHandling)

target_link_libraries (KinovaArm positionHandling ${LIBKINDRV_LIB} ${Boost_LIBRARIES})

install (TARGETS KinovaArm DESTINATION bin)
install (FILES KinovaArm.h DESTINATION include)

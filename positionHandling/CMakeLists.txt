cmake_minimum_required(VERSION 2.6)

#file(GLOB positionHandling_FILES *.cpp *.h)

#add_library(positionHandling ${positionHandling_FILES})
add_library(positionHandling PositionHandling.cpp)

install (TARGETS positionHandling DESTINATION bin)
install (FILES PositionHandling.h DESTINATION include)

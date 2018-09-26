cmake_minimum_required(VERSION 2.6)


add_library(TCPServer TCPServer.cpp)


install (TARGETS TCPServer DESTINATION bin)
install (FILES TCPServer.h DESTINATION include)

cmake_minimum_required(VERSION 3.15)

project(sz)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/sz/include)

FILE(GLOB SZ_SRCS ${CMAKE_CURRENT_SOURCE_DIR}/sz/src/*.c)

add_library(sz STATIC ${SZ_SRCS})

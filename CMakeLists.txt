cmake_minimum_required(VERSION 2.8)
project(SIOX)

set(SIOX_SOURCES ${CMAKE_SOURCE_DIR})
set(SIOX_INCLUDE ${CMAKE_SOURCE_DIR}/include)
set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -fPIC")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SIOX_SOURCES}/tools/cmake/cmake-glib2)
set(BUILD_SHARED_LIBS ON)

add_subdirectory(core)
add_subdirectory(knowledge)
add_subdirectory(monitoring)


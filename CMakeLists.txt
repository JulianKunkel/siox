project(SIOX)

cmake_minimum_required(VERSION 2.8)

set(SIOX_SOURCES ${CMAKE_SOURCE_DIR})
set(SIOX_INCLUDE ${CMAKE_SOURCE_DIR}/include)
set(SIOX_INCLUDE_DIRS ${SIOX_SOURCES} ${SIOX_INCLUDE})

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -fPIC")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SIOX_SOURCES}/tools/cmake)

set(BUILD_SHARED_LIBS ON)

# you can adjust the installation path here
set(CMAKE_INSTALL_PREFIX /usr/local)

# this is for self-compiled libraries that are not in the system's ld-path
set(CMAKE_LIBRARY_PATH
    ${CMAKE_LIBRARY_PATH}
#  /opt/boost-1.53/lib
)

# this is for the header files of self-compiled libraries
set(CMAKE_INCLUDE_PATH
    ${CMAKE_INCLUDE_PATH}
#    /opt/boost-1.53/include
)

include(FindProtobuf)
find_package(Protobuf REQUIRED)
find_package(Boost 1.53.0 COMPONENTS serialization system thread unit_test_framework REQUIRED)
find_package(GLIB2 REQUIRED)
find_package(PostgreSQL)
add_subdirectory(core)
add_subdirectory(knowledge)
add_subdirectory(monitoring)


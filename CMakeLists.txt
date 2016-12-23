project(SIOX C CXX)

cmake_minimum_required(VERSION 2.8)

include(CTest)
include(FeatureSummary)
include(CheckCSourceCompiles)

set(SIOX_SOURCES ${CMAKE_SOURCE_DIR})
set(SIOX_INCLUDE ${CMAKE_SOURCE_DIR}/src/include)
set(SIOX_INCLUDE_DIRS ${SIOX_SOURCES} ${SIOX_INCLUDE} ${CMAKE_SOURCE_DIR}/src/ ${PROJECT_BINARY_DIR} ${PROJECT_BINARY_DIR}/src CACHE STRING "SIOX INCLUDE DIRS")

SET (CMAKE_C_FLAGS_DEBUG          "-ggdb -O0 -Wno-unused-local-typedefs -DDEBUG_VERSION")
SET (CMAKE_CXX_FLAGS_DEBUG        "-ggdb -O0 -Wno-unused-local-typedefs -DDEBUG_VERSION")

set(CMAKE_C_FLAGS "-Wall -std=gnu99 -Wno-unused-local-typedefs -DRELEASE_VERSION")
set(CMAKE_C_FLAGS_RELEASE "-O3 -std=gnu99 -Wno-unused-local-typedefs")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wno-unused-local-typedefs -DRELEASE_VERSION")

set(CMAKE_EXE_LINKER_FLAGS "-Wl,--no-as-needed -Wno-unused-local-typedefs")
set(CMAKE_SHARED_LINKER_FLAGS "-Wl,--no-as-needed -Wno-unused-local-typedefs")
set(CMAKE_MODULE_LINKER_FLAGS "-Wl,--no-as-needed -Wno-unused-local-typedefs")

set(CMAKE_MODULE_PATH ${SIOX_SOURCES}/devel/cmake ${CMAKE_MODULE_PATH})

set(BUILD_SHARED_LIBS ON)

if ("${CMAKE_CXX_COMPILER_ID}" MATCHES "GNU")
    set(CMAKE_CXX_FLAGS "-Wall -std=gnu++11 -fPIC -Wno-unused-local-typedefs")
    execute_process(COMMAND ${CMAKE_CXX_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (NOT (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7))
        message(FATAL_ERROR "${PROJECT_NAME} requires g++ 4.7 or greater.")
    endif ()
elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
else ()
    message(FATAL_ERROR "Your C++ compiler does not support C++11. (or it is not supported by this build script)")
endif ()


# IF we set it here we cannot change it using for example ccmake
#### WARNING DO NOT USE THIS, use ccmake or configure to set the directory
#### AGAIN DO NOT USE THE FOLLOWING LINES, EXCEPT IF YOU ARE NOT COMMITTING THIS FILE
# set(CMAKE_INSTALL_PREFIX /usr/local/siox)
# set(NetCDF_INCLUDE_PATH /usr/local/include)


# this is for self-compiled libraries that are not in the system's ld-path
set(CMAKE_LIBRARY_PATH  ${CMAKE_LIBRARY_PATH})

# this is for the header files of self-compiled libraries
set(CMAKE_INCLUDE_PATH ${CMAKE_INCLUDE_PATH})

find_package(MPI)

include(CTest)
include(FeatureSummary)

include(SIOXMacros)

set_library_install_suffix()
set(SIOX_INSTALL_LIBRARY_DIR lib${LIBRARY_INSTALL_SUFFIX})

find_package(Boost 1.49.0 COMPONENTS regex serialization system thread program_options REQUIRED)
set_package_properties(Boost PROPERTIES URL "http://www.boost.org" TYPE REQUIRED PURPOSE "SIOX core dependency.")

find_package(GLIB 2.30 COMPONENTS gio gobject gmodule REQUIRED)
set_package_properties(GLIB PROPERTIES URL "http://www.gtk.org" TYPE REQUIRED PURPOSE "SIOX core dependency.")

find_package(Pqxx 3.0)
set_package_properties(Pqxx PROPERTIES URL "http://pqxx.org" TYPE OPTIONAL PURPOSE "PostgreSQL Backend.")

find_package(PostgreSQL)
set_package_properties(PostgreSQL PROPERTIES URL "http://www.postgresql.org" TYPE OPTIONAL PURPOSE "SIOX PostgreSQL backend support.")

find_package(PythonInterp 2.7 REQUIRED)
set_package_properties(PythonInterp PROPERTIES URL "http://www.python.org" TYPE REQUIRED PURPOSE "SIOX core dependency.")

find_package(LIKWID)
set_package_properties(LIKWID PROPERTIES URL "see Readme.md" TYPE OPTIONAL PURPOSE "CPU counters")


find_package(BDB)
set_package_properties(BDB PROPERTIES URL "http://www.oracle.com/technetwork/database/database-technologies/berkeleydb/overview/index.html" TYPE OPTIONAL PURPOSE "Alternative backend for activities")

if(PACKAGES_NOT_FOUND)
	feature_summary(INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES DESCRIPTION "\n-- The following mandatory packages couldn't be found on your system:\n" WHAT PACKAGES_NOT_FOUND)
endif()

add_subdirectory(src)


if (BUILD_WRAPPERS)
	add_subdirectory(tools)
endif (BUILD_WRAPPERS)

message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")

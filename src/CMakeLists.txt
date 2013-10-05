project(SIOX)

cmake_minimum_required(VERSION 2.8)

set(SIOX_SOURCES ${CMAKE_SOURCE_DIR})
set(SIOX_INCLUDE ${CMAKE_SOURCE_DIR}/src/include)
set(SIOX_INCLUDE_DIRS ${SIOX_SOURCES} ${SIOX_INCLUDE} ${CMAKE_SOURCE_DIR}/src/ ${PROJECT_BINARY_DIR})

set(CMAKE_CXX_FLAGS "-ggdb -Wall -std=gnu++11 -fPIC")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${SIOX_SOURCES}/devel/cmake)

set(BUILD_SHARED_LIBS ON)

# you can adjust the installation path here
# IF we set it here we cannot change it using for example ccmake
# set(CMAKE_INSTALL_PREFIX /usr/local)

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

include(CTest)
include(FeatureSummary)

include(SIOXMacros)

# find_package(Protobuf REQUIRED)
# set_package_properties(Protobuf PROPERTIES URL "http://code.google.com/p/protobuf" TYPE REQUIRED PURPOSE "Serialization using Google Protocol Buffers.")

find_package(Boost 1.49.0 COMPONENTS regex serialization system thread program_options REQUIRED)
set_package_properties(Boost PROPERTIES URL "http://www.boost.org" TYPE REQUIRED PURPOSE "SIOX core dependency.")

find_package(GLIB2 REQUIRED)
set_package_properties(GLIB2 PROPERTIES URL "http://www.gtk.org" TYPE REQUIRED PURPOSE "SIOX core dependency.")

find_package(GMODULE2 REQUIRED)
set_package_properties(GMODULE2 PROPERTIES URL "http://www.gtk.org" TYPE REQUIRED PURPOSE "SIOX core dependency.")

find_package(GIOUNIX2 REQUIRED)
set_package_properties(GIOUNIX2 PROPERTIES URL "http://www.gtk.org" TYPE OPTIONAL PURPOSE "GIO communication module.")

find_package(PostgreSQL)
set_package_properties(PostgreSQL PROPERTIES URL "http://www.postgresql.org" TYPE OPTIONAL PURPOSE "SIOX PostgreSQL backend support.")

find_package(NetCDF)
set_package_properties(NetCDF PROPERTIES URL "http://www.unidata.ucar.edu/software/netcdf" TYPE OPTIONAL PURPOSE "SIOX NetCDF examples.")

find_package(HDF5 NO_MODULE)
set_package_properties(HDF5 PROPERTIES URL "http://www.hdfgroup.org/HDF5" TYPE OPTIONAL PURPOSE "SIOX HDF5 examples.")

find_package(MPI)
set_package_properties(MPI PROPERTIES URL "http://www.open-mpi.org" TYPE OPTIONAL PURPOSE "SIOX Open MPI examples.")

find_package(PythonInterp 2.7 REQUIRED)
set_package_properties(PythonInterp PROPERTIES URL "http://www.python.org" TYPE REQUIRED PURPOSE "SIOX core dependency.")

# siox_generate_buffers()

feature_summary(WHAT PACKAGES_NOT_FOUND INCLUDE_QUIET_PACKAGES FATAL_ON_MISSING_REQUIRED_PACKAGES DESCRIPTION "\n-- The following packages couldn't be found on your system:\n") 

add_subdirectory(core)
add_subdirectory(knowledge)
add_subdirectory(monitoring)
add_subdirectory(util)
add_subdirectory(daemon)
add_subdirectory(tools)

install(DIRECTORY include/ DESTINATION include/)


cmake_minimum_required(VERSION 2.8)
project(otf2siox)

set(SRC_DIR "/home/aguilera/Projects/SIOX/aguilera/siox/src")

set(CMAKE_C_FLAGS "-ggdb -Wall")
set(CMAKE_C_COMPILE_FLAGS   ${CMAKE_C_COMPILE_FLAGS}   ${MPI_COMPILE_FLAGS})
set(CMAKE_C_LINK_FLAGS      ${CMAKE_C_LINK_FLAGS}      ${MPI_LINK_FLAGS})

include_directories(${SRC_DIR}
	${SRC_DIR}/otf2siox
	${SRC_DIR}/ontology
	${SRC_DIR}/siox-ll)

link_directories(${SRC_DIR}/otf2siox
	${SRC_DIR}/ontology
	${SRC_DIR}/siox-ll)

add_library(libotf2siox 
	libotf2siox.h 
	libotf2siox.c
	${SRC_DIR}/siox-ll/siox-ll.h
	${SRC_DIR}/siox-ll/siox-ll.c
	${SRC_DIR}/ontology/ontology.h
	${SRC_DIR}/ontology/ontology.c
	${SRC_DIR}/ontology/datatype.h
	${SRC_DIR}/ontology/datatype.c
	${SRC_DIR}/ontology/metric.h
	${SRC_DIR}/ontology/metric.c)

add_executable(otf2siox otf2siox.c)
target_link_libraries(otf2siox libotf2siox otf)

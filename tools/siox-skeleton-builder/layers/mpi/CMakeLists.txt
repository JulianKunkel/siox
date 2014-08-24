project(SIOX)

set(LAYER "mpi")

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}/tools/siox-skeleton-builder/layers/${LAYER}
	${CMAKE_CURRENT_BINARY_DIR}
	${GLIB_INCLUDE_DIRS}
	${MPI_CXX_INCLUDE_PATH}
)




siox_skeleton_builder(wrap  ${CMAKE_CURRENT_SOURCE_DIR}/${LAYER}-wrapper.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-wrap.c ${CMAKE_CURRENT_SOURCE_DIR}/template.py)
siox_skeleton_builder(dlsym ${CMAKE_CURRENT_SOURCE_DIR}/${LAYER}-wrapper.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-dlsym.c ${CMAKE_CURRENT_SOURCE_DIR}/template.py)

file(READ ${CMAKE_CURRENT_BINARY_DIR}/${LAYER}.wrap WRAP_LD_OPTS)
string(REPLACE "\n" " " WRAP_LD_OPTS ${WRAP_LD_OPTS})

add_library(
	siox-${LAYER}-wrap
	siox-${LAYER}-wrap.c
)

target_link_libraries(
	siox-${LAYER}-wrap
	siox-monitoring-siox-ll
	${GLIB_LIBRARIES}
	${MPI_CXX_LIBRARIES}
)

add_library(
	siox-${LAYER}-dlsym
	siox-${LAYER}-dlsym.c
)

target_link_libraries(
	siox-${LAYER}-dlsym
	siox-monitoring-siox-ll
	${GLIB_LIBRARIES}
	${MPI_CXX_LIBRARIES}
)



# check for CONST

set(CMAKE_REQUIRED_INCLUDES ${MPI_CXX_INCLUDE_PATH})
CHECK_C_SOURCE_COMPILES("
	#include <mpi.h>

	int MPI_File_delete( char * filename, MPI_Info info ){
		return MPI_SUCCESS;
	}

	int main(){
		return 0;
	}
	" 
	MPI_PROTOTYPE_DEFAULT )

CHECK_C_SOURCE_COMPILES("
	#include <mpi.h>

	int MPI_File_delete( const char * filename, MPI_Info info ){
		return MPI_SUCCESS;
	}

	int main(){
		return 0;
	}
	" 
	MPI_PROTOTYPE_CONST )


# Only one of them should be true

if ( (NOT MPI_PROTOTYPE_CONST AND NOT MPI_PROTOTYPE_DEFAULT) OR (MPI_PROTOTYPE_CONST AND MPI_PROTOTYPE_DEFAULT) )
	message(FATAL_ERROR "Could not determine the MPI prototype status correctly")
endif()

if ( MPI_PROTOTYPE_CONST )
	set_target_properties(siox-${LAYER}-wrap PROPERTIES COMPILE_FLAGS "-DOPTIONAL_CONST=const")
	set_target_properties(siox-${LAYER}-dlsym PROPERTIES COMPILE_FLAGS "-DOPTIONAL_CONST=const")
endif()

set_target_properties(siox-${LAYER}-wrap PROPERTIES LINK_FLAGS ${WRAP_LD_OPTS})
set_target_properties(siox-${LAYER}-dlsym PROPERTIES LINK_FLAGS "-ldl")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LAYER}.wrap DESTINATION ${SIOX_INSTALL_LIBRARY_DIR} RENAME libsiox-${LAYER}.wrap)
install(TARGETS siox-${LAYER}-wrap LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
install(TARGETS siox-${LAYER}-dlsym LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

add_subdirectory(test)

project(SIOX)

set(LAYER "netcdf")

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}/tools/siox-skeleton-builder/layers/${LAYER}4
	${CMAKE_CURRENT_BINARY_DIR}
	${GLIB_INCLUDE_DIRS}
	${MPI_CXX_INCLUDE_PATH}
)

siox_skeleton_builder(wrap  ${LAYER}-wrapper.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-wrap.c)
siox_skeleton_builder(dlsym ${LAYER}-wrapper.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-dlsym.c)

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

set_target_properties(siox-${LAYER}-wrap PROPERTIES LINK_FLAGS ${WRAP_LD_OPTS})

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

set_target_properties(siox-${LAYER}-dlsym PROPERTIES LINK_FLAGS "-ldl")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LAYER}.wrap DESTINATION ${SIOX_INSTALL_LIBRARY_DIR} RENAME libsiox-${LAYER}.wrap)
install(TARGETS siox-${LAYER}-wrap LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
install(TARGETS siox-${LAYER}-dlsym LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


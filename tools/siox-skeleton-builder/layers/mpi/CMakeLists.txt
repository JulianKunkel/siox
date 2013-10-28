project(SIOX)

set(LAYER "mpi")

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}/tools/siox-skeleton-builder/layers/${LAYER}
	${CMAKE_CURRENT_BINARY_DIR}
	${GLIB2_INCLUDE_DIRS}
)

siox_skeleton_builder(wrap  ${LAYER}-wrapper.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-wrap.c)
siox_skeleton_builder(dlsym ${LAYER}-wrapper.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-dlsym.c)

file(READ ${CMAKE_CURRENT_BINARY_DIR}/${LAYER}.wrap WRAP_LD_OPTS)
string(REPLACE "," " " WRAP_LD_OPTS ${WRAP_LD_OPTS})

add_library(
	siox-${LAYER}-wrap
	siox-${LAYER}-wrap.c
)

target_link_libraries(
	siox-${LAYER}-wrap
	siox-monitoring-siox-ll
	${GLIB2_LIBRARIES}
)

set_target_properties(siox-${LAYER}-wrap PROPERTIES LD_FLAGS ${WRAP_LD_OPTS} )


add_library(
	siox-${LAYER}-dlsym
	siox-${LAYER}-dlsym.c
)

target_link_libraries(
	siox-${LAYER}-dlsym
	siox-monitoring-siox-ll
	${GLIB2_LIBRARIES}
)


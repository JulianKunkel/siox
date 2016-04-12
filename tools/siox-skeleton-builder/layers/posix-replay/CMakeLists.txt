project(SIOX)

set(LAYER "posix-replay")

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}/tools/siox-skeleton-builder/layers/${LAYER}
	${CMAKE_CURRENT_BINARY_DIR}
	${GLIB_INCLUDE_DIRS}
)

siox_skeleton_builder(replayer  ${CMAKE_CURRENT_SOURCE_DIR}/${LAYER}-low-level-io.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-wrap.c  ${CMAKE_CURRENT_SOURCE_DIR}/template.py)
siox_skeleton_builder(replay-wrapper-header  ${CMAKE_CURRENT_SOURCE_DIR}/${LAYER}-low-level-io.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-wrap.c  ${CMAKE_CURRENT_SOURCE_DIR}/template.py)
siox_skeleton_builder(wreplay-wrapper  ${CMAKE_CURRENT_SOURCE_DIR}/${LAYER}-low-level-io.h ${SKELETON_TEMPLATE} ${LAYER} siox-${LAYER}-wrap.c  ${CMAKE_CURRENT_SOURCE_DIR}/template.py)

file(READ ${CMAKE_CURRENT_BINARY_DIR}/${LAYER}.wrap WRAP_LD_OPTS)
string(REPLACE "\n" " " WRAP_LD_OPTS ${WRAP_LD_OPTS})

add_library(
	siox-${LAYER}-wrap
	siox-${LAYER}-wrap.c
)

set_target_properties(siox-${LAYER}-wrap PROPERTIES LINK_FLAGS ${WRAP_LD_OPTS})

target_link_libraries(
	siox-${LAYER}-wrap
	siox-monitoring-siox-ll
	${GLIB_LIBRARIES}
)

add_library(
	siox-${LAYER}-dlsym
	siox-${LAYER}-dlsym.c
)

target_link_libraries(
	siox-${LAYER}-dlsym
	siox-monitoring-siox-ll
	${GLIB_LIBRARIES}
)

set_target_properties(siox-${LAYER}-dlsym PROPERTIES LINK_FLAGS "-ldl")


# add_library(
# 	siox-${LAYER}-null-wrap
# 	siox-${LAYER}-wrap.c
# )
# 
# set_target_properties(siox-${LAYER}-null-wrap PROPERTIES LINK_FLAGS ${WRAP_LD_OPTS})
# 
# target_link_libraries(
# 	siox-${LAYER}-null-wrap
# 	siox-monitoring-siox-ll-null
# 	${GLIB_LIBRARIES}
# )
# 
# 
# add_library(
# 	siox-${LAYER}-null-dlsym
# 	siox-${LAYER}-dlsym.c
# )
# 
# target_link_libraries(
# 	siox-${LAYER}-null-dlsym
# 	siox-monitoring-siox-ll-null
# 	${GLIB_LIBRARIES}
# )
# 
# set_target_properties(siox-${LAYER}-null-dlsym PROPERTIES LINK_FLAGS "-ldl")

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${LAYER}.wrap DESTINATION ${SIOX_INSTALL_LIBRARY_DIR} RENAME libsiox-${LAYER}.wrap)
install(TARGETS siox-${LAYER}-wrap LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})
install(TARGETS siox-${LAYER}-dlsym LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


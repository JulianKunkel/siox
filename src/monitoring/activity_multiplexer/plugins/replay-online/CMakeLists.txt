project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

set(LAYER "posix-player")
siox_skeleton_builder_relaxed(play ${CMAKE_CURRENT_SOURCE_DIR}/${LAYER}-low-level-io.h ${CMAKE_CURRENT_SOURCE_DIR}/wrapper-template.py ${LAYER} siox-${LAYER}-wrapper.c  ${CMAKE_CURRENT_SOURCE_DIR}/template.py)




set(PLUGIN_NAME ReplayPluginOnline)

add_library(
	siox-monitoring-activityPlugin-${PLUGIN_NAME}
	Replay.cpp
	ReplayOptions.cpp
	#siox-${LAYER}-wrapper.c
)

target_link_libraries(
	siox-monitoring-activityPlugin-${PLUGIN_NAME}
	siox-core-ModuleLoader
)

siox_run_serializer(ReplayOptions.hpp ReplayOptions.cpp)

# TODO: reenable test
#add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-${PLUGIN_NAME} LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})



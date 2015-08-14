project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

set(PLUGIN_NAME ReplayPluginOnline)

add_library(
	siox-monitoring-activityPlugin-${PLUGIN_NAME}
	Replay.cpp
	ReplayOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-${PLUGIN_NAME}
	siox-core-ModuleLoader
)

siox_run_serializer(ReplayOptions.hpp ReplayOptions.cpp)

# TODO: reenable test
#add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-${PLUGIN_NAME} LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})



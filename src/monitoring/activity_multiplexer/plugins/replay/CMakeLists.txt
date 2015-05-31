project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)
add_library(
	siox-monitoring-activityPlugin-ReplayPlugin
	Replay.cpp
	ReplayOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ReplayPlugin
	siox-core-ModuleLoader
)

siox_run_serializer(ReplayOptions.hpp ReplayOptions.cpp)

# TODO: reenable test
#add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ReplayPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


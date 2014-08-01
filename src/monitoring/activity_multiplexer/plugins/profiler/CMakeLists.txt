project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityProfiler
	ActivityProfiler.cpp
	ActivityProfilerPluginOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityProfiler
	siox-core-ContainerSerializer
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

siox_run_serializer(ActivityProfilerPluginOptions.hpp ActivityProfilerPluginOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityProfiler LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

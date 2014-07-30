project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-activity_multiplexer-ActivityProfiler
	Test.cpp
)

target_link_libraries(
	core-activity_multiplexer-ActivityProfiler
	siox-core-ModuleLoader 
	siox-monitoring-ActivityMultiplexerAsync
	siox-monitoring-activityPlugin-ActivityProfiler
	siox-util
)

add_test(core-activity_multiplexer-ActivityProfiler core-activity_multiplexer-ActivityProfiler)


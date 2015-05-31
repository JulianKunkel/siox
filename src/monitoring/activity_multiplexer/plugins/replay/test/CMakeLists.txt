project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	ActivityReplayTest
	ActivityReplayTest.cpp
)

target_link_libraries(
	ActivityReplayTest
	siox-core-ModuleLoader 
	siox-monitoring-ActivityMultiplexerAsync
	siox-monitoring-activityPlugin-Replay
	siox-util
)

add_test(core-activity_multiplexer-filewriter-ActivityReplayTest ActivityReplayTest)


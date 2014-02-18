project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${BDB_INCLUDE_DIRS}
)

add_executable(
	core-activity_multiplexer-bdfilewriter
	Test.cpp
)

target_link_libraries(
	core-activity_multiplexer-bdfilewriter
	siox-core-ModuleLoader 
	siox-monitoring-ActivityMultiplexerAsync
	siox-monitoring-activityPlugin-ActivityBDBWriter
	siox-util
)

add_test(core-activity_multiplexer-bdfilewriter core-activity_multiplexer-bdfilewriter)


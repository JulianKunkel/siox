project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-activity_multiplexer-binfilewriter
	Test.cpp
)

target_link_libraries(
	core-activity_multiplexer-binfilewriter
	siox-core-ModuleLoader 
	siox-monitoring-ActivityMultiplexerAsync
	siox-monitoring-activityPlugin-ActivityBinWriter
	siox-util
)

add_test(core-activity_multiplexer-binfilewriter core-activity_multiplexer-binfilewriter)


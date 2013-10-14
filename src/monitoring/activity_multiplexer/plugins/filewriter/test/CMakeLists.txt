project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	ActivityFileWriterTest
	ActivityFileWriterTest.cpp
)

target_link_libraries(
	ActivityFileWriterTest
	siox-core-ModuleLoader 
	siox-monitoring-ActivityMultiplexerAsync
	siox-monitoring-activityPlugin-ActivityFileWriter
)

add_test(core-activity_multiplexer-filewriter-ActivityFileWriterTest ActivityFileWriterTest)


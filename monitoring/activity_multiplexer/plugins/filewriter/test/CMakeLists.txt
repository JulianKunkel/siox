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
	siox-monitoring-ActivityMultiplexerAsyncThreaded 
	siox-core-ModuleLoader 
	siox-monitoring-activityPlugin-ActivityFileWriter
)

add_test(Activity-File-Writer ActivityFileWriterTest)


project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-reporting-ConsoleReporter
	test.cpp
)

target_link_libraries(
	core-reporting-ConsoleReporter
	siox-core-ModuleLoader
	siox-core-reporting-ConsoleReporter
)

add_test(core-reporting-ConsoleReporter core-reporting-ConsoleReporter)

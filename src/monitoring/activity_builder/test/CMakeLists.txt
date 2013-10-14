project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	ActivityBuilderTest
	activity_builder-test.cpp
)

target_link_libraries(
	ActivityBuilderTest
	siox-monitoring-ActivityBuilder
)

add_test(Activity-Builder ActivityBuilderTest)

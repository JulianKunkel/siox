project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	knowledge-reasoner-modules-reasoner-test
	reasoner-test.cpp
)
target_link_libraries(
	knowledge-reasoner-modules-reasoner-test
	siox-monitoring-activityPlugin-AnomalyInjector
	siox-core-ModuleLoader
	siox-knowledge-ReasonerStandardImplementation
	siox-core-comm-gio
)
add_test(
	knowledge-reasoner-modules-reasoner-test
	knowledge-reasoner-modules-reasoner-test
)


add_executable(
	knowledge-reasoner-modules-reasoner-test-raw
	reasoner-test-raw.cpp
)
target_link_libraries(
	knowledge-reasoner-modules-reasoner-test-raw
	siox-core-ModuleLoader
	siox-knowledge-ReasonerStandardImplementation
	siox-core-comm-gio
)
add_test(
	knowledge-reasoner-modules-reasoner-test-raw
	knowledge-reasoner-modules-reasoner-test-raw
)
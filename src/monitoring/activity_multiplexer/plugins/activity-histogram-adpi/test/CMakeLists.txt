project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_executable(
	core-monitoring-activity_multiplexer-activity-histogram-adpi-test
	activity-histogram-adpi-test.cpp
)

target_link_libraries(
	core-monitoring-activity_multiplexer-activity-histogram-adpi-test
	siox-monitoring-activityPlugin-histogramAdpi	
	siox-monitoring-FileBasedSystemInformation
	siox-knowledge-DereferencingFacade	
)

add_test(core-monitoring-activity_multiplexer-activity-histogram-adpi-test core-monitoring-activity_multiplexer-activity-histogram-adpi-test)


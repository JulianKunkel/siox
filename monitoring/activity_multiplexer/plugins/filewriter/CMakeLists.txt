project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityFileWriter
	ActivityFileWriter.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityFileWriter
	siox-core-ContainerSerializer 
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

add_subdirectory(test)


project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityForwarder
	ActivityForwarder.cpp
	BoostXMLSerializable.cpp
)

siox_run_serializer(ActivityForwarderOptions.hpp BoostXMLSerializable.cpp)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityForwarder
	siox-core-AutoConfigurator
)

add_subdirectory(test)


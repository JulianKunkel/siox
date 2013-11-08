project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityNetworkForwarder
	ActivityNetworkForwarderServer.cpp
	ActivityNetworkForwarderOptions.cpp
	ActivityNetworkForwarderClient.cpp
	ActivityBinarySerializable.hpp
)

siox_run_binary_serializer( ../../../../include/monitoring/datatypes/Activity.hpp ActivityBinarySerializable.hpp)

siox_run_serializer(ActivityNetworkForwarderOptions.hpp ActivityNetworkForwarderOptions.cpp)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityNetworkForwarder
	siox-core-AutoConfigurator
	siox-core-comm
)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityNetworkForwarder LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

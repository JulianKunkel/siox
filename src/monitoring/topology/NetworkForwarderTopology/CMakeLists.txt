project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

siox_run_binary_serializer(NetworkForwarderCommunication.hpp NetworkForwarderCommunicationBinarySerializable.hpp)

add_library(
	siox-monitoring-NetworkForwarderTopology
	NetworkForwarderTopology.cpp
	NetworkForwarderTopologyOptions.cpp
	NetworkForwarderCommunicationBinarySerializable.hpp
)

add_library(
	siox-monitoring-NetworkForwarderTopologyServer
	NetworkForwarderTopologyServer.cpp
	NetworkForwarderTopologyServerOptions.cpp
	NetworkForwarderCommunication.hpp
	NetworkForwarderCommunicationBinarySerializable.hpp
)

target_link_libraries(
	siox-monitoring-NetworkForwarderTopology
	siox-monitoring-NetworkForwarderTopologyServer
	siox-core-AutoConfigurator
	siox-monitoring-Topology
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
	siox-core-comm	
)


siox_run_serializer(NetworkForwarderTopologyOptions.hpp NetworkForwarderTopologyOptions.cpp)

siox_run_serializer(NetworkForwarderTopologyServerOptions.hpp NetworkForwarderTopologyServerOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-NetworkForwarderTopology siox-monitoring-NetworkForwarderTopologyServer LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

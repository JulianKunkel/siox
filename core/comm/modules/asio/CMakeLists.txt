project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PROTOBUF_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -lrt")

add_library(
	core-communication
	ConnectionMessage.cpp
	Connection.cpp
	ConnectionFactory.cpp
	ServerFactory.cpp
	ServiceServer.cpp
	ipc/IPCConnection.cpp
	ipc/IPCServer.cpp
	tcp/TCPConnection.cpp
	tcp/TCPServer.cpp
	ServiceClient.cpp
)

target_link_libraries(
	core-communication
	core-logger
	core-comm-buffers
	${Boost_LIBRARIES}
)

add_subdirectory(messages)
#add_subdirectory(test)

project(SIOX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -lpq")

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PROTOBUF_INCLUDE_DIRS}
	${PostgreSQL_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
)

add_library(
	monitoring-transactionsystem
	PostgreSQLBackend.cpp
	PostgreSQLPumpCallback.cpp
	TransactionService.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(
	monitoring-transactionsystem 
	core-communication
	core-logger
	${PROTOBUF_LIBRARIES}
	${PostgreSQL_LIBRARIES}
	${Boost_LIBRARIES}
)

add_library(
	psql_backend MODULE
	PostgreSQLBackend.cpp
	PostgreSQLPumpCallback.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(
	psql_backend
	core-communication 
	core-logger
	${PROTOBUF_LIBRARIES}
)

add_executable(
	transaction_server
	transaction_server.cpp
)

target_link_libraries(
	transaction_server
	monitoring-transactionsystem
	core-communication 
	core-logger
	${PROTOBUF_LIBRARIES}
	${Boost_LIBRARIES}
)


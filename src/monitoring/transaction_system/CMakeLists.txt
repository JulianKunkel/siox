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
)

target_link_libraries(
	monitoring-transactionsystem 
	core-communication
	core-comm-buffers
	core-logger
	${Boost_LIBRARIES}
)

add_library(
	psql_backend 
	PostgreSQLBackend.cpp
	PostgreSQLPumpCallback.cpp
)

target_link_libraries(
	psql_backend
	core-communication 
	core-comm-buffers
	core-logger
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
	${Boost_LIBRARIES}
)

add_subdirectory(test)


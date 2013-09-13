project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PROTOBUF_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(postgressucker
	PostgreSQLSucker.cpp
)

target_link_libraries(postgressucker
	monitoring-transactionsystem 
	core-logger
)

add_executable(postgresql_test
	postgres.cpp
)

target_link_libraries(postgresql_test
	postgressucker
	monitoring-transactionsystem 
	core-communication 
	core-logger
)

add_test(PostgreSQL postgresql_test)

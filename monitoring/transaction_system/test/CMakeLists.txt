project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${PROTOBUF_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_executable(postgresql_test
	postgres.cpp
	${PROTO_SRCS}
	${PROTO_HDRS}
)

target_link_libraries(postgresql_test 
	monitoring-transactionsystem 
	core-communication 
	core-logger
)

add_test(PostgreSQL postgresql_test)
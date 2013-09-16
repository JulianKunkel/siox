project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	core-logger
	BasicLogger.cpp
	Logger.cpp
	FileLogger.cpp
	SyslogLogger.cpp
# 	NetLogger.cpp
# 	NetLoggerServer.cpp
)

target_link_libraries(
	core-logger 
# 	core-communication 
# 	core-comm-buffers
	${Boost_LIBRARIES}
)

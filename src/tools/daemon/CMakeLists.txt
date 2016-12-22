project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_executable(
	siox-daemon
	Daemon.cpp
)

target_link_libraries(
	siox-daemon
	siox-util
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)


install(TARGETS siox-daemon RUNTIME DESTINATION bin)

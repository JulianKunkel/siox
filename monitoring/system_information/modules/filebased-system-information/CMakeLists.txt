project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-FileBasedSystemInformation
	FileBasedSystemInformation.cpp
)

target_link_libraries(
	siox-monitoring-FileBasedSystemInformation
	${Boost_LIBRARIES}
)

add_subdirectory(test)


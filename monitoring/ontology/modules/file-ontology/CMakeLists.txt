project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-FileOntology
	FileOntology.cpp
)

target_link_libraries(
	siox-monitoring-FileOntology
	${Boost_LIBRARIES}
)

add_subdirectory(test)

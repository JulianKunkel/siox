project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-TopologyOntology
	TopologyOntology.cpp
	TopologyOntologyOptions.cpp
)

target_link_libraries(
	siox-monitoring-TopologyOntology
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(TopologyOntologyOptions.hpp TopologyOntologyOptions.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-TopologyOntology LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

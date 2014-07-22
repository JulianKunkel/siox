project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-FileOntology
	FileOntology.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-FileOntology
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(FileOntologyOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-FileOntology LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityFileWriter
	ActivityFileWriter.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityFileWriter
	siox-core-ContainerSerializer
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

siox_run_serializer(ActivityFileWriterOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityFileWriter LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

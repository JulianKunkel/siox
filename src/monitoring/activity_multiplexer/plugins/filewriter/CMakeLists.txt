project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-activityPlugin-ActivityFileWriter
	ActivityFileWriter.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityFileWriter
	siox-core-AutoConfigurator
)

siox_run_serializer(ActivityFileWriterOptions.hpp BoostXMLSerializable.cpp)


add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityFileWriter LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

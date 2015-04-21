project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${BDB_INCLUDE_DIRS}
)

add_library(
	siox-monitoring-activityPlugin-ActivityBDBWriter
	ActivityBDBWriter.cpp
	ActivityBDBWriterPluginOptions.cpp
	ActivityBinarySerializable.hpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityBDBWriter
	siox-core-AutoConfigurator
)

siox_run_serializer(ActivityBDBWriterPluginOptions.hpp ActivityBDBWriterPluginOptions.cpp)

siox_run_binary_serializer( ../../../../include/monitoring/datatypes/Activity.hpp ActivityBinarySerializable.hpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityBDBWriter LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

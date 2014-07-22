project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityBinWriter
	ActivityBinWriter.cpp
	ActivityBinWriterPluginOptions.cpp
	ActivityBinarySerializable.hpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityBinWriter
	siox-core-ContainerSerializer
	siox-core-AutoConfigurator
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(ActivityBinWriterPluginOptions.hpp ActivityBinWriterPluginOptions.cpp)

siox_run_binary_serializer( ../../../../include/monitoring/datatypes/Activity.hpp ActivityBinarySerializable.hpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityBinWriter LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

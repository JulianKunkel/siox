project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

# set name of the plugin
set(PLUGIN_NAME ActivityModifyFilepath)

add_library(
	siox-monitoring-activityPlugin-${PLUGIN_NAME}
	ActivityMutator.cpp
	BoostXMLSerializable.cpp
)

siox_run_serializer(ActivityMutatorOptions.hpp BoostXMLSerializable.cpp)

target_link_libraries(
	siox-monitoring-activityPlugin-${PLUGIN_NAME}
	siox-core-AutoConfigurator
)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-${PLUGIN_NAME} LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

# set name of the plugin
set(PLUGIN_NAME ActivitySequalizer)

add_library(
	siox-monitoring-activityPlugin-${PLUGIN_NAME}
    Sequalizer.cpp
	BoostXMLSerializable.cpp
)

siox_run_serializer(SequalizerOptions.hpp BoostXMLSerializable.cpp)

target_link_libraries(
	siox-monitoring-activityPlugin-${PLUGIN_NAME}
	siox-core-AutoConfigurator
)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-${PLUGIN_NAME} LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

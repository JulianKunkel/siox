project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

add_library(
	siox-monitoring-activityPlugin-ActivityMutator
	ActivityMutator.cpp
	BoostXMLSerializable.cpp
)

siox_run_serializer(ActivityMutatorOptions.hpp BoostXMLSerializable.cpp)

target_link_libraries(
	siox-monitoring-activityPlugin-ActivityMutator
	siox-core-AutoConfigurator
)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-ActivityMutator LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

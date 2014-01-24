project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}
	${Boost_INCLUDE_DIR}
)


add_library(
	siox-monitoring-activityPlugin-AnomalyInjector
	AnomalyInjector.cpp
	AnomalyInjectorOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-AnomalyInjector
	siox-core-AutoConfigurator
	${Boost_LIBRARIES}
)

siox_run_serializer(AnomalyInjectorOptions.hpp AnomalyInjectorOptions.cpp)

# add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-AnomalyInjector LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

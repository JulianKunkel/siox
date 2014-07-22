project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}
	${Boost_INCLUDE_DIR}
)


add_library(
	siox-monitoring-activityPlugin-FileSurveyor
	FileSurveyor.cpp
	FileSurveyorOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-FileSurveyor
	siox-core-AutoConfigurator
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(FileSurveyorOptions.hpp FileSurveyorOptions.cpp)

# add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-FileSurveyor LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

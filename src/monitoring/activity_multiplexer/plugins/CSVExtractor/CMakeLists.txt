project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)
add_library(
	siox-monitoring-activityPlugin-CSVExtractorPlugin
	CSVExtractor.cpp
   CSVExtractorOptions.cpp
)


target_link_libraries(
	siox-monitoring-activityPlugin-CSVExtractorPlugin
	siox-core-ModuleLoader
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(CSVExtractorOptions.hpp CSVExtractorOptions.cpp)

install(TARGETS siox-monitoring-activityPlugin-CSVExtractorPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


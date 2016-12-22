project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${SIOX_SOURCES}
	${Boost_INCLUDE_DIR}
)


add_library(
	siox-monitoring-activityPlugin-histogramAdpi
	HistogramAdpi.cpp
	HistogramAdpiOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-histogramAdpi
	siox-core-AutoConfigurator
	#-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(HistogramAdpiOptions.hpp HistogramAdpiOptions.cpp)

add_subdirectory(test)

install(TARGETS siox-monitoring-activityPlugin-histogramAdpi LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

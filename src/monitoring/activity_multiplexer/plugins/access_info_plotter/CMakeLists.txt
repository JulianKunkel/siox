project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)
add_library(
	siox-monitoring-activityPlugin-AccessInfoPlotterPlugin
	AccessInfoPlotter.cpp
	AccessInfoPlotterOptions.cpp
)

target_link_libraries(
	siox-monitoring-activityPlugin-AccessInfoPlotterPlugin
	siox-core-ModuleLoader
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)

siox_run_serializer(AccessInfoPlotterOptions.hpp AccessInfoPlotterOptions.cpp)

install(TARGETS siox-monitoring-activityPlugin-AccessInfoPlotterPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


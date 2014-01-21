project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)
add_library(
	siox-tools-TraceReader-AccessInfoPlotterPlugin
	AccessInfoPlotter.cpp
)


target_link_libraries(
	siox-tools-TraceReader-AccessInfoPlotterPlugin
	siox-core-ModuleLoader
	${Boost_LIBRARIES}
)


install(TARGETS siox-tools-TraceReader-AccessInfoPlotterPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


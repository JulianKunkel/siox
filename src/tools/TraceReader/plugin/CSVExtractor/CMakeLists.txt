project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)
add_library(
	siox-tools-TraceReader-CSVExtractorPlugin
	CSVExtractor.cpp
)


target_link_libraries(
	siox-tools-TraceReader-CSVExtractorPlugin
	siox-core-ModuleLoader
	-L${Boost_LIBRARY_DIR}
	${Boost_LIBRARIES}
)


install(TARGETS siox-tools-TraceReader-CSVExtractorPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


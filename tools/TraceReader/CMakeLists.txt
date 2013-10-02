project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)


add_executable(
	siox-trace-reader
	TraceReaderMain.cpp
)

target_link_libraries(
	siox-trace-reader
	siox-tools-TraceReader
)

add_library(
	siox-tools-TraceReader
	TraceReader.cpp
)


target_link_libraries(
	siox-tools-TraceReader
	siox-core-ModuleLoader
	${Boost_LIBRARIES}
	siox-monitoring-FileAssociationMapper
	siox-monitoring-FileOntology
	siox-monitoring-FileBasedSystemInformation
	siox-core-ActivitySerializable
)

add_subdirectory(test)

install(TARGETS siox-tools-TraceReader siox-trace-reader LIBRARY DESTINATION lib RUNTIME DESTINATION bin)


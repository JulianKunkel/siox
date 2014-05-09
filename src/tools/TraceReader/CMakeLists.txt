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
	${Boost_LIBRARIES}
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
	siox-monitoring-activityPlugin-ActivityBinWriter
	siox-core-ActivitySerializable
	siox-monitoring-TopologyAssociationMapper
	siox-monitoring-TopologyOntology
	siox-monitoring-TopologySystemInformation
	siox-monitoring-Topology
)
#	siox-monitoring-DatabaseTopology


add_subdirectory(test)
add_subdirectory(plugin)

install(TARGETS siox-tools-TraceReader siox-trace-reader LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})


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
	siox-core-ModuleLoader
	siox-util
)

add_subdirectory(activity_stream)

install(TARGETS siox-trace-reader LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

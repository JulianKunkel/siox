project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${MPI_CXX_INCLUDE_PATH}
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

find_package(MPI)
if (MPI_FOUND)

add_executable(
	siox-trace-reader-parallel
	TraceReaderParallelMain.cpp
)

target_link_libraries(
	siox-trace-reader-parallel
	siox-core-ModuleLoader
	siox-util
	${MPI_CXX_LIBRARIES}
)

install(TARGETS siox-trace-reader-parallel LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

endif(MPI_FOUND)


add_subdirectory(activity_stream)

install(TARGETS siox-trace-reader LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

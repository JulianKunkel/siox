project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

add_library(
	siox-tools-activityInputStreamPlugin-FileAIStreamPlugin
	FileAIStream.cpp
	FileAIStreamOptions.cpp
)

target_link_libraries(
	siox-tools-activityInputStreamPlugin-FileAIStreamPlugin
	#siox-core-ModuleLoader
	${Boost_LIBRARIES}
	siox-monitoring-activityPlugin-ActivityBinWriter
	siox-core-ActivitySerializable
	siox-util
)

siox_run_serializer(FileAIStreamOptions.hpp FileAIStreamOptions.cpp)
install(TARGETS siox-tools-activityInputStreamPlugin-FileAIStreamPlugin LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR} RUNTIME DESTINATION bin ARCHIVE DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})

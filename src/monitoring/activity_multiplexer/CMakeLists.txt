project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
)

siox_run_serializer(ActivityMultiplexerAsyncOptions.hpp BoostXMLSerializable.cpp)

add_library(
	siox-monitoring-ActivityMultiplexerAsync
	ActivityMultiplexerAsync.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-ActivityMultiplexerAsync
	siox-core-AutoConfigurator 
)

install(TARGETS siox-monitoring-ActivityMultiplexerAsync LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})





siox_run_serializer(ActivityMultiplexerSyncOptions.hpp ActivityMultiplexerSyncOptions.cpp)

add_library(
	siox-monitoring-ActivityMultiplexerSync
	ActivityMultiplexerSync.cpp
	ActivityMultiplexerSyncOptions.cpp
)

target_link_libraries(
	siox-monitoring-ActivityMultiplexerSync
	siox-core-AutoConfigurator 
)

install(TARGETS siox-monitoring-ActivityMultiplexerSync LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})




add_subdirectory(plugins)

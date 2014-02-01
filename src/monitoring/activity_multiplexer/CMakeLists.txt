project(SIOX)

include_directories(
	${SIOX_INCLUDE_DIRS}
	${Boost_INCLUDE_DIR}
)

siox_run_serializer(ActivityMultiplexerAsyncOptions.hpp BoostXMLSerializable.cpp)

add_library(
	siox-monitoring-ActivityMultiplexerAsync
	ActivityMultiplexerAsync.cpp
	BoostXMLSerializable.cpp
)

target_link_libraries(
	siox-monitoring-ActivityMultiplexerAsync
	siox-core-ContainerSerializer 
	siox-core-AutoConfigurator 
	${Boost_LIBRARIES}
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
	siox-core-ContainerSerializer 
	siox-core-AutoConfigurator 
	${Boost_LIBRARIES}
)

install(TARGETS siox-monitoring-ActivityMultiplexerSync LIBRARY DESTINATION ${SIOX_INSTALL_LIBRARY_DIR})




add_subdirectory(plugins)
